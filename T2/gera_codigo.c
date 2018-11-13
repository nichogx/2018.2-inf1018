#include <stdio.h>
#include <stdlib.h>
#include<string.h>

typedef int (*funcp)(int x);

static void error(const char *msg, int line) {
	fprintf(stderr, "erro %s na linha %d\n", msg, line);
	exit(EXIT_FAILURE);
}

void gera_codigo(FILE *f, void **code, funcp *entry)
{
	int line = 1;
	int  c;

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
			tamAtual += 4;
			tmp = realloc(codea, tamAtual);
			if (tmp == NULL) {
				free(codea);
				printf("erro de memoria\n");
				exit(EXIT_FAILURE);
			}
			codea = tmp;
			codea[tamAtual - 4] = 0x55; /* push %rbp */
			codea[tamAtual - 3] = 0x48; /* mov %rsp, %rbp */
			codea[tamAtual - 2] = 0x89;
			codea[tamAtual - 1] = 0xe5;
			printf("function\n");
			break;
		}
		case 'e': { /* end */
			char c0;
			if (fscanf(f, "nd%c", &c0) != 1) {
				error("comando invalido", line);
			}
			printf("end\n");
			break;
		}
		case 'r': {  /* retorno incondicional */
			int idx0, idx1;
			char var0, var1;
			if (fscanf(f, "et %c%d", &var0, &idx0) != 2) {
				error("comando invalido", line);
			}
			if (var0 == '$') { /* literal */
				unsigned char *inteiroLido = (unsigned char *) &idx0;
				tamAtual += 10;
				codea = realloc(codea, tamAtual);
				if (tmp == NULL) {
					free(codea);
					printf("erro de memoria\n");
					exit(EXIT_FAILURE);
				}
				codea = tmp;
				codea[tamAtual - 10] = 0xb8; /* mov literal para %eax */
				codea[tamAtual - 9]  = inteiroLido[0];
				codea[tamAtual - 8]  = inteiroLido[1];
				codea[tamAtual - 7]  = inteiroLido[2];
				codea[tamAtual - 6]  = inteiroLido[3];

				/* reestabelece a pilha e retorna */
				codea[tamAtual - 5] = 0x48; /* mov %rbp, %rsp */
				codea[tamAtual - 4] = 0x89;
				codea[tamAtual - 3] = 0xec;
				codea[tamAtual - 2] = 0x5d; /* pop %rbp */
				codea[tamAtual - 1] = 0xc3; /* ret */

			} else if (var0 == 'v') { /* variavel */
				
			}
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
				int f, idx1;
				char var1;
				if (fscanf(f, "all %d %c%d\n", &f, &var1, &idx1) != 3) {
					error("comando invalido", line);
				}
				printf("%c%d = call %d %c%d\n", var0, idx0, f, var1, idx1);
			} else { /* operaÃ§Ã£o aritmÃ©tica */
				int idx1, idx2;
				char var1 = c0, var2, op;
				if (fscanf(f, "%d %c %c%d", &idx1, &op, &var2, &idx2) != 4) {
					error("comando invalido", line);
				}
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
	return;
}

void gera_codigo2(void **code, funcp *entry) {
	unsigned char *codea = malloc(14);
	*entry = (funcp) codea;
	codea[0] = 0x55;
	codea[1] = 0x48;
	codea[2] = 0x89;
	codea[3] = 0xe5;
	codea[4] = 0xb8;
	codea[5] = 0x64;
	codea[6] = 0x00;
	codea[7] = 0x00;
	codea[8] = 0x00;
	codea[9] = 0x48;
	codea[10] = 0x89;
	codea[11] = 0xec;
	codea[12] = 0x5d;
	codea[13] = 0xc3;

	/* passa o array usado para o parametro recebido,
	para ser liberado fora */
	*code = codea;
}

void libera_codigo(void *p) {
	free(p);
}
