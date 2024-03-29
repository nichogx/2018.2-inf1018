#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*funcp)(int x);

static void error(const char *msg, int line)
{
	fprintf(stderr, "erro %s na linha %d\n", msg, line);
	exit(EXIT_FAILURE);
}

/* essa funcao gerencia as insercoes no vetor de codigo.
	recebe:
		dest: vetor antigo de codigo
		valores: vetor de codigo a inserir no final
		qtd: tamanho do vetor valores
		tamAtual: endereco da variavel que guarda o tamanho de dest
 */
static void insere(unsigned char *dest, unsigned char *valores,
	int qtd, int *tamAtual)
{
	/* atualiza tamanho do vetor */
	(*tamAtual) += qtd;

	/* adiciona no vetor novo */
	for (int i = 0; i < qtd; i++) {
		dest[*tamAtual - (qtd - i)] = valores[i];
	}
}

void gera_codigo(FILE *f, void **code, funcp *entry)
{
	int line = 1;
	int c;

	int nFuncs = 0;
	int funcsFechadas = 0;
	funcp funcoes[20];
	/* vetor que segura o endereco inicial de todas as
	funcoes */

	int nCalls = 0;
	int calls[20];
	/* vetor que segura a posicao dos calls */

	int tamAtual = 0;
	unsigned char *codea = (unsigned char *) malloc(1000 * sizeof(codea));

	#ifdef _DEBUG
	printf("CODIGO LIDO: \n");
	#endif

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
			insere(codea, vals, 8, &tamAtual);

			/* atualiza tamanho do vetor de enderecos das funcoes */
			nFuncs++;

			/* insere endereco da atual no final do vetor de funcoes */
			funcoes[nFuncs - 1] = (funcp) &codea[tamAtual - 8];

			#ifdef _DEBUG
			printf("function\n");
			#endif
			break;
				  }
		case 'e': { /* end */
			char c0;
			if (fscanf(f, "nd%c", &c0) != 1) {
				error("comando invalido", line);
			}
			funcsFechadas++;

			#ifdef _DEBUG
			printf("end\n");
			#endif
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
				insere(codea, instr, 1, &tamAtual); /* insere instrucao */
				insere(codea, inteiroLido, 4, &tamAtual); /* insere literal */
			} else if (var0 == 'v') { /* variavel */
				unsigned char vals[] = {
					0x8b, 0x45, 0xe0 + 4 * idx0 /* seta o valor de vi em %edx */
				};
				insere(codea, vals, 3, &tamAtual);
			} else if (var0 == 'p' && idx0 == 0) { /* parm */
				unsigned char vals[] = {
					0x89, 0xf8 /* mov %edi, %eax */
				};
				insere(codea, vals, 2, &tamAtual);
			} else {
				error("comando invalido", line);
			}

			/* reestabelece a pilha e retorna */
			unsigned char vals[] = {
				0x48, 0x89, 0xec, /* mov %rbp, %rsp */
				0x5d,             /* pop %rbp */
				0xc3              /* ret */
			};
			insere(codea, vals, 5, &tamAtual);

			#ifdef _DEBUG
			printf("ret %c%d\n", var0, idx0);
			#endif
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
				insere(codea, instr, 2, &tamAtual); /* insere instrucao */
				insere(codea, inteiroLido, 4, &tamAtual); /* insere literal */
				insere(codea, compara, 4, &tamAtual);
			} else if (var0 == 'v') { /* variavel */
				unsigned char vals[] = {
					0x81, 0x7d, 0xe0 + 4 * idx0, /* cmpl $0, valor de vi */
					0x0, 0x0, 0x0, 0x0
				};

				insere(codea, vals, 7, &tamAtual);
			} else if (var0 == 'p' && idx0 == 0) { /* parm */
				unsigned char vals[] = {
					0x83, 0xff, 0x0 /* cmpl $0, %edi */
				};
				insere(codea, vals, 3, &tamAtual);
			} else {
				error("comando invalido", line);
			}

			if (var1 == '$') { /* literal */
				unsigned char *inteiroLido = (unsigned char *) &idx1;
				unsigned char instr[] = {
					0x75, 0x07,				/* jne endif */
					0xb8					/* mov literal para %eax */
				};
				insere(codea, instr, 3, &tamAtual); /* insere instrucao */
				insere(codea, inteiroLido, 4, &tamAtual); /* insere literal */
			} else if (var1 == 'v') { /* variavel */
				unsigned char vals[] = {
					0x75, 0x05,					/* jne endif */
					0x8b, 0x45, 0xe0 + 4 * idx1 /* seta o valor de vi em %eax */
				};
				insere(codea, vals, 5, &tamAtual);
			} else if (var1 == 'p' && idx1 == 0) { /* parm */
				unsigned char vals[] = {
					0x75, 0x04,				/* jne endif */
					0x89, 0xf8				/* mov %edi, %eax */
				};
				insere(codea, vals, 4, &tamAtual);
			} else {
				error("comando invalido", line);
			}
			insere(codea, fim, 2, &tamAtual); /* insere leave e ret */

			#ifdef _DEBUG
			printf("zret %c%d %c%d\n", var0, idx0, var1, idx1);
			#endif
			break;
				  }
		case 'v': {  /* atribuicao */
			int idx0;
			char c0;

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
					insere(codea, salvaParmPilha, 3, &tamAtual); /* insere salvamento de parametro na pilha */
					if (var1 == '$')
					{
						unsigned char *inteiroLido = (unsigned char *) &idx1;
						unsigned char instr[] = {
							0xbf					/* mov literal para %edi */
						};
						
						insere(codea, instr, 1, &tamAtual); /* insere instrucao */
						insere(codea, inteiroLido, 4, &tamAtual); /* insere literal */
					} else if (var1 == 'v')
					{
						unsigned char vals[] = {
							0x8b, 0x7d, 0xe0 + 4 * idx1 /* seta o valor de vi em %edi */
						};
						insere(codea, vals, 3, &tamAtual); /* insere instrucao */
					} else if (var1 != 'p')
					{
						error("comando invalido", line);
					}
					insere(codea, caller, 1, &tamAtual); /* insere chamada (0xE8) */

					/* no final eh editado esse numero */
					/* assume que tamAtual caiba em 3 bytes */
					unsigned char diff[] = {
						fCall, 
						0x00, 0x00, 0x00
					};

					/* atualiza vetor de posicao de calls */
					nCalls++;
					calls[nCalls - 1] = tamAtual - 1;

					insere(codea, diff, 4, &tamAtual); /* insere funcao */
					insere(codea, retiraParmPilha, 3, &tamAtual); /* insere retirada de parametro da pilha */

					/* move de eax para a variavel atribuida */
					unsigned char vals[] = {
						0x89, 0x45, 0xe0 + 4 * idx0 /* mover o valor de %eax para vi */
					};
					insere(codea, vals, 3, &tamAtual);
				} else {
					error("comando invalido", line);
				}
				#ifdef _DEBUG
				printf("%c%d = call %d %c%d\n", c, idx0, fCall, var1, idx1);
				#endif
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
					insere(codea, instr, 1, &tamAtual); /* insere instrucao */
					insere(codea, inteiroLido, 4, &tamAtual); /* insere literal */
				} else if (var1 == 'v') { /* variavel */
					unsigned char vals[] = {
						0x8b, 0x75, 0xe0 + 4 * idx1 /* seta o valor de vi em %esi */
					};
					insere(codea, vals, 3, &tamAtual);
				} else if (var1 == 'p' && idx1 == 0) { /* parm */
					unsigned char vals[] = {
						0x89, 0xfe /* mov %edi, %esi */
					};
					insere(codea, vals, 2, &tamAtual);
				} else {
					error("comando invalido", line);
				}

				/* seta segundo parm da operacao em %edx */
				if (var2 == '$') { /* literal */
					unsigned char *inteiroLido = (unsigned char *) &idx2;
					unsigned char instr[] = {
						0xba /* mov literal para %edx */
					};
					insere(codea, instr, 1, &tamAtual); /* insere instrucao */
					insere(codea, inteiroLido, 4, &tamAtual); /* insere literal */
				} else if (var2 == 'v') { /* variavel */
					unsigned char vals[] = {
						0x8b, 0x55, 0xe0 + 4 * idx2 /* seta o valor de vi em %edx */
					};
					insere(codea, vals, 3, &tamAtual);
				} else if (var2 == 'p' && idx2 == 0) { /* parm */
					unsigned char vals[] = {
						0x89, 0xfa /* mov %edi, %edx */
					};
					insere(codea, vals, 2, &tamAtual);
				}

				/* calcula e insere valor na variavel */
				if (op == '+') {
					unsigned char vals[] = {
						0x01, 0xd6 /* addl %edx, %esi */
					};
					insere(codea, vals, 2, &tamAtual);
				} else if (op == '-') {
					unsigned char vals[] = {
						0x29, 0xd6 /* subl %edx, %esi */
					};
					insere(codea, vals, 2, &tamAtual);
				} else if (op == '*') {
					unsigned char vals[] = {
						0x0f, 0xaf, 0xf2 /* imull %edx, %esi */
					};
					insere(codea, vals, 3, &tamAtual);
				} else {
					error("comando invalido", line);
				}

				unsigned char vals[] = {
					0x89, 0x75, 0xe0 + 4 * idx0 /* mover o valor de %esi para vi */
				};
				insere(codea, vals, 3, &tamAtual);
				#ifdef _DEBUG
				printf("%c%d = %c%d %c %c%d\n",
					c, idx0, var1, idx1, op, var2, idx2);
				#endif
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

	/* linka calls */
	#ifdef _DEBUG
	printf("\nCODIGO ANTES DA LINKAGEM:\n");
	for (int i = 0; i < tamAtual; i++) {
		printf("%02x ", codea[i]);
	} printf("\n");
	#endif

	for (int i = 0; i < nCalls; i++) {
		int cl = calls[i];
		unsigned int fAChamar = codea[cl + 1]; /* numero da funcao a ser chamada */
		int diff = (unsigned char *)funcoes[fAChamar] - (unsigned char *)(&codea[cl + 1]) - 4;
		codea[cl + 1] = *((unsigned char *) (&diff));
		codea[cl + 2] = *((unsigned char *) (&diff) + 1);
		codea[cl + 3] = *((unsigned char *) (&diff) + 2);
		codea[cl + 4] = *((unsigned char *) (&diff) + 3);
	}

	#ifdef _DEBUG
	printf("\nCODIGO DEPOIS DA LINKAGEM:\n");
	for (int i = 0; i < tamAtual; i++) {
		printf("%02x ", codea[i]);
	} printf("\n\n");
	#endif

	codea = (unsigned char *) realloc(codea, tamAtual);

	/* coloca o vetor de codigo no ponteiro
	recebido por referencia */
	*code = codea;

	/* seta a ultima funcao como entrada
	e libera o vetor das funcoes */
	*entry = (funcp) funcoes[nFuncs - 1];

	return;
}

void libera_codigo(void *p)
{
	free(p);
}

