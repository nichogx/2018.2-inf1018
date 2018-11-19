#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*funcp)(int x);

static void error(const char *msg, int line)
{
	fprintf(stderr, "erro %s na linha %d\n", msg, line);
	exit(EXIT_FAILURE);
}

/* essa funcao gerencia as insercoes e os reallocs no vetor de codigo.
recebe:
dest: vetor antigo de codigo
valores: vetor de codigo a inserir no final
qtd: tamanho do vetor valores
tamAtual: endereco da variavel que guarda o tamanho de dest
funcoes: o vetor que guarda o endereco das funcoes (para ser
atualizado com os novos enderecos)
nFuncs: tamanho do vetor funcoes
retorna:
o novo enderco do vetor que guarda o codigo */
static unsigned char *insere(unsigned char *dest, unsigned char *valores,
	int qtd, int *tamAtual, funcp *funcoes, int nFuncs)
{

	unsigned char *tmp = NULL;

	/* atualiza tamanho do vetor */
	(*tamAtual) += qtd;

	/* realoca um novo vetor */
	tmp = realloc(dest, *tamAtual);
	if (tmp == NULL) {
		free(dest);
		error("memoria insuficiente", -1);
	}

	/* adiciona no vetor novo */
	for (int i = 0; i < qtd; i++) {
		tmp[*tamAtual - (qtd - i)] = valores[i];
	}

	/* atualiza vetor de endereco das funcoes */
	for (int i = 0; i < nFuncs; i++) {
		funcoes[i] = funcoes[i] + (tmp - dest);
	}

	/* tmp eh o vetor novo (o antigo ja foi freeado pela realloc */
	return tmp;
}

void gera_codigo(FILE *f, void **code, funcp *entry)
{
	int line = 1;
	int c;

	int nFuncs = 0;
	int funcsFechadas = 0;
	funcp *funcoes = NULL;
	/* vetor que segura o endereco inicial de todas as
	funcoes */

	int tamAtual = 0;
	unsigned char *codea = NULL;
	while ((c = fgetc(f)) != EOF) {
		switch (c) {
		case 'f': { /* function */
			char c0;
			if (fscanf(f, "unction%c", &c0) != 1) {
				error("comando invalido", line);
			}

			/* preenche vetor de codigo */
			unsigned char vals[] = {
				0x55,                  /* push %rbp */
				0x48, 0x89, 0xe5,      /* mov %rsp, %rbp */
				0x48, 0x83, 0xec, 0x20 /* sub $32, %rsp */
			};
			codea = insere(codea, vals, 8, &tamAtual, funcoes, nFuncs);

			/* realoca vetor de enderecos das funcoes */
			nFuncs++;
			funcp *tmp = NULL;
			tmp = realloc(funcoes, nFuncs);
			if (tmp == NULL) {
				free(funcoes);
				error("memoria insuficente", line);
			}
			funcoes = (funcp *) tmp;

			/* insere endereco da atual no final do vetor de funcoes */
			funcoes[nFuncs - 1] = (funcp) &codea[tamAtual - 8];

			printf("function\n");
			break;
				  }
		case 'e': { /* end */
			char c0;
			if (fscanf(f, "nd%c", &c0) != 1) {
				error("comando invalido", line);
			}
			funcsFechadas++;
			printf("end\n");
			break;
				  }
		case 'r': {  /* retorno incondicional */
			int idx0 = 0;
			char var0;
			if (fscanf(f, "et %c%d", &var0, &idx0) != 2) {
				error("comando invalido", line);
			}

			if (var0 == '$') { /* literal */
				unsigned char *inteiroLido = (unsigned char *) &idx0;
				unsigned char instr[] = {
					0xb8 /* mov literal para %eax */
				};
				codea = insere(codea, instr, 1, &tamAtual, funcoes,
					nFuncs); /* insere instrucao */
				codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes,
					nFuncs); /* insere literal */
			} else if (var0 == 'v') { /* variavel */
				unsigned char vals[] = {
					0x8b, 0x45, 0xe0 + 4 * idx0 /* seta o valor de vi em %edx */
				};
				codea = insere(codea, vals, 3, &tamAtual, funcoes, nFuncs);
			} else if (var0 == 'p' && idx0 == 0) { /* parm */
				unsigned char vals[] = {
					0x89, 0xf8 /* mov %edi, %eax */
				};
				codea = insere(codea, vals, 2, &tamAtual, funcoes, nFuncs);
			} else {
				error("comando invalido", line);
			}

			/* reestabelece a pilha e retorna */
			unsigned char vals[] = {
				0x48, 0x89, 0xec, /* mov %rbp, %rsp */
				0x5d,             /* pop %rbp */
				0xc3              /* ret */
			};
			codea = insere(codea, vals, 5, &tamAtual, funcoes, nFuncs);

			printf("ret %c%d\n", var0, idx0);
			break;
				  }
		case 'z': {  /* retorno condicional */
			int idx0, idx1;
			char var0, var1;
			unsigned char fim[]  = {
					0xc9, 0xc3				/* leave e ret */
				};
			if (fscanf(f, "ret %c%d %c%d", &var0, &idx0, &var1, &idx1) != 4) {
				error("comando invalido", line);
			}

			if (var0 == '$') { /* literal */
				unsigned char *inteiroLido = (unsigned char *) &idx0;
				unsigned char instr[] = {
					0x41, 0xba /* mov literal para %r10d */
				};
				unsigned char compara[] = {
					0x41, 0x83, 0xfa, 0x0 /* cmpl $0, %r10d */
				};
				codea = insere(codea, instr, 2, &tamAtual, funcoes,
					nFuncs); /* insere instrucao */
				codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes,
					nFuncs); /* insere literal */
				codea = insere(codea, compara, 6, &tamAtual, funcoes,
					nFuncs);
			} else if (var0 == 'v') { /* variavel */
				unsigned char vals[] = {
					0x81, 0x7d, 0xe0 + 4 * idx0, /* cmpl $0, valor de vi */
					0x0, 0x0, 0x0, 0x0
				};

				codea = insere(codea, vals, 9, &tamAtual, funcoes, nFuncs);
			} else if (var0 == 'p' && idx0 == 0) { /* parm */
				unsigned char vals[] = {
					0x83, 0xff, 0x0 /* cmpl $0, %edi */
				};
				codea = insere(codea, vals, 5, &tamAtual, funcoes, nFuncs);
			} else {
				error("comando invalido", line);
			}

			if (var1 == '$') { /* literal */
				unsigned char *inteiroLido = (unsigned char *) &idx1;
				unsigned char instr[] = {
					0x75, 0x07,				/* jne endif */
					0xb8					/* mov literal para %eax */
				};
				codea = insere(codea, instr, 3, &tamAtual, funcoes,
					nFuncs); /* insere instrucao */
				codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes,
					nFuncs); /* insere literal */
			} else if (var1 == 'v') { /* variavel */
				unsigned char vals[] = {
					0x75, 0x05,					/* jne endif */
					0x8b, 0x45, 0xe0 + 4 * idx1 /* seta o valor de vi em %eax */
				};
				codea = insere(codea, vals, 5, &tamAtual, funcoes, nFuncs);
			} else if (var1 == 'p' && idx1 == 0) { /* parm */
				unsigned char vals[] = {
					0x75, 0x04,				/* jne endif */
					0x89, 0xf8				/* mov %edi, %eax */
				};
				codea = insere(codea, vals, 4, &tamAtual, funcoes, nFuncs);
			} else {
				error("comando invalido", line);
			}
			codea = insere(codea, fim, 2, &tamAtual, funcoes, nFuncs); /* insere leave e ret */

			printf("zret %c%d %c%d\n", var0, idx0, var1, idx1);
			break;
				  }
		case 'v': {  /* atribuicao */
			int idx0;
			char var0 = c, c0;
			if (fscanf(f, "%d = %c", &idx0, &c0) != 2) {
				error("comando invalido", line);
			}

			if (c0 == 'c') { /* call */
				int fCall, idx1;
				char var1;
				if (fscanf(f, "all %d %c%d\n", &fCall, &var1, &idx1) != 3) {
					error("comando invalido", line);
				}
				if (fCall >= 0)
				{
					unsigned char salvaParmPilha[] = {
						0x89, 0x7d, 0xf4			/* movl %edi, -12(%rbp) */
					};
					unsigned char retiraParmPilha[] = {
						0x8b, 0x7d, 0xf4			/* movl -12(%rbp), %edi */
					};
					unsigned char caller[] = {
							0xe8						/* call function */
						};
					codea = insere(codea, salvaParmPilha, 3, &tamAtual, funcoes,
							nFuncs); /* insere salvamento de parametro na pilha */
					if (var1 == '$')
					{
						unsigned char *inteiroLido = (unsigned char *) &idx1;
						unsigned char instr[] = {
							0xbf					/* mov literal para %edi */
						};
						
						codea = insere(codea, instr, 1, &tamAtual, funcoes,
							nFuncs); /* insere instrucao */
						codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes,
							nFuncs); /* insere literal */
					} else if (var1 == 'v')
					{
						unsigned char vals[] = {
							0x8b, 0x7d, 0xe0 + 4 * idx1 /* seta o valor de vi em %edi */
						};
						codea = insere(codea, vals, 3, &tamAtual, funcoes,
							nFuncs); /* insere instrucao */
					} else if (var1 != 'p')
					{
						error("comando invalido", line);
					}
					codea = insere(codea, caller, 1, &tamAtual, funcoes,
							nFuncs); /* insere chamada */
					codea = insere(codea, (unsigned char *)funcoes[fCall], 4, &tamAtual, funcoes,
							nFuncs); /* insere funcao */
					codea = insere(codea, retiraParmPilha, 3, &tamAtual, funcoes,
							nFuncs); /* insere retirada de parametro da pilha */
				} else {
					error("comando invalido", line);
				}
				printf("%c%d = call %d %c%d\n", var0, idx0, fCall, var1, idx1);
			} else { /* operacao aritmetica */
				int idx1 = 0, idx2 = 0;
				char var1 = c0, var2, op;
				if (fscanf(f, "%d %c %c%d", &idx1, &op, &var2, &idx2) != 4) {
					error("comando invalido", line);
				}

				/* seta primeiro parm da operacao em %esi */
				if (var1 == '$') { /* literal */
					unsigned char *inteiroLido = (unsigned char *) &idx1;
					unsigned char instr[] = {
						0xbe /* mov literal para %esi */
					};
					codea = insere(codea, instr, 1, &tamAtual, funcoes,
						nFuncs); /* insere instrucao */
					codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes,
						nFuncs); /* insere literal */
				} else if (var1 == 'v') { /* variavel */
					unsigned char vals[] = {
						0x8b, 0x75, 0xe0 + 4 * idx1 /* seta o valor de vi em %esi */
					};
					codea = insere(codea, vals, 3, &tamAtual, funcoes, nFuncs);
				} else if (var1 == 'p' && idx1 == 0) { /* parm */
					unsigned char vals[] = {
						0x89, 0xfe /* mov %edi, %esi */
					};
					codea = insere(codea, vals, 2, &tamAtual, funcoes, nFuncs);
				} else {
					error("comando invalido", line);
				}

				/* seta segundo parm da operacao em %edx */
				if (var2 == '$') { /* literal */
					unsigned char *inteiroLido = (unsigned char *) &idx2;
					unsigned char instr[] = {
						0xba /* mov literal para %edx */
					};
					codea = insere(codea, instr, 1, &tamAtual, funcoes,
						nFuncs); /* insere instrucao */
					codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes,
						nFuncs); /* insere literal */
				} else if (var2 == 'v') { /* variavel */
					unsigned char vals[] = {
						0x8b, 0x55, 0xe0 + 4 * idx2 /* seta o valor de vi em %edx */
					};
					codea = insere(codea, vals, 3, &tamAtual, funcoes, nFuncs);
				} else if (var2 == 'p' && idx2 == 0) { /* parm */
					unsigned char vals[] = {
						0x89, 0xfa /* mov %edi, %edx */
					};
					codea = insere(codea, vals, 2, &tamAtual, funcoes, nFuncs);
				}

				/* calcula e insere valor na variavel */
				if (op == '+') {
					unsigned char vals[] = {
						0x01, 0xd6 /* addl %edx, %esi */
					};
					codea = insere(codea, vals, 2, &tamAtual, funcoes, nFuncs);
				} else if (op == '-') {
					unsigned char vals[] = {
						0x29, 0xd6 /* subl %edx, %esi */
					};
					codea = insere(codea, vals, 2, &tamAtual, funcoes, nFuncs);
				} else if (op == '*') {
					unsigned char vals[] = {
						0x0f, 0xaf, 0xf2 /* imull %edx, %esi */
					};
					codea = insere(codea, vals, 3, &tamAtual, funcoes, nFuncs);
				} else {
					error("comando invalido", line);
				}

				unsigned char vals[] = {
					0x89, 0x75, 0xe0 + 4 * idx0 /* mover o valor de %esi para vi */
				};
				codea = insere(codea, vals, 3, &tamAtual, funcoes, nFuncs);

				printf("%c%d = %c%d %c %c%d\n",
					var0, idx0, var1, idx1, op, var2, idx2);
			}
			break;
				  }
		default:
			error("comando desconhecido", line);
		}
		line ++;
		fscanf(f, " ");
	}

	if (funcsFechadas != nFuncs) {
		free(codea);
		error("numero de ends nao corresponde ao numero de functions", -1);
	}

	/* coloca o vetor de codigo no ponteiro
	recebido por referencia */
	*code = codea;

	/* seta a ultima funcao como entrada
	e libera o vetor das funcoes */
	*entry = (funcp) funcoes[nFuncs - 1];
	free(funcoes);

	return;
}

void libera_codigo(void *p)
{
	free(p);
}

