#include <stdio.h>
#include <stdlib.h>
#include<string.h>

typedef int (*funcp)(int x);

static void error(const char *msg, int line) {
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
                             int qtd, int *tamAtual, funcp *funcoes, int nFuncs) {

	unsigned char *tmp = NULL;

	(*tamAtual) += qtd;
	tmp = realloc(dest, *tamAtual);
	if (tmp == NULL) {
		free(dest);
		error("memoria insuficiente", -1);
	}

	for (int i = 0; i < qtd; i++) {
		tmp[*tamAtual - (qtd - i)] = valores[i];
	}

	for (int i = 0; i < nFuncs; i++) {
		funcoes[i] = funcoes[i] + (tmp - dest);
	}

	return tmp;
}

void gera_codigo(FILE *f, void **code, funcp *entry)
{
	int line = 1;
	int c;

	int nFuncs = 0;
	int funcsFechadas = 0;
	funcp *funcoes = NULL;

	int tamAtual = 0;
	unsigned char *codea = NULL;
	unsigned char *tmp = NULL;
	while ((c = fgetc(f)) != EOF) {
		switch (c) {
		case 'f': { /* function */
			char c0;
			if (fscanf(f, "unction%c", &c0) != 1) {
				error("comando invalido", line);
			}

			/* preenche vetor de codigo */
			unsigned char vals[] = {
				0x55, /* push %rbp */
				0x48, 0x89, 0xe5, /* mov %rsp, %rbp */
				0x48, 0x83, 0xec, 0x20 /* sub $32, %rsp */
			};
			codea = insere(codea, vals, 8, &tamAtual, funcoes, nFuncs);

			/* realoca vetor de enderecos das funcoes */
			nFuncs++;
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
				codea = insere(codea, instr, 1, &tamAtual, funcoes, nFuncs); /* insere instrucao */
				codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes, nFuncs); /* insere literal */
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
			} else error("comando invalido", line);

			/* reestabelece a pilha e retorna */
			unsigned char vals[] = {
				0x48, 0x89, 0xec, /* mov %rbp, %rsp */
				0x5d, /* pop %rbp */
				0xc3  /* ret */
			};
			codea = insere(codea, vals, 5, &tamAtual, funcoes, nFuncs);
				
			printf("ret %c%d\n", var0, idx0);
			break;
		}
		case 'z': {  /* retorno condicional */
			int idx0, idx1;
			char var0, var1;
			if (fscanf(f, "ret %c%d %c%d", &var0, &idx0, &var1, &idx1) != 4) {
				error("comando invalido", line);
			}
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
					codea = insere(codea, instr, 1, &tamAtual, funcoes, nFuncs); /* insere instrucao */
					codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes, nFuncs); /* insere literal */
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
				} else error("comando invalido", line);

				/* seta segundo parm da operacao em %edx */
				if (var2 == '$') { /* literal */
					unsigned char *inteiroLido = (unsigned char *) &idx2;
					unsigned char instr[] = {
						0xba /* mov literal para %edx */
					};
					codea = insere(codea, instr, 1, &tamAtual, funcoes, nFuncs); /* insere instrucao */
					codea = insere(codea, inteiroLido, 4, &tamAtual, funcoes, nFuncs); /* insere literal */
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
				} else error("comando invalido", line);

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

	*code = codea;
	*entry = (funcp) funcoes[nFuncs - 1];
	
	return;
}

void libera_codigo(void *p) {
	free(p);
}

