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
	unsigned char *codea = malloc(tamAtual);
	while ((c = fgetc(f)) != EOF) {
		switch (c) {
		case 'f': { /* function */
			char c0;
			if (fscanf(f, "unction%c", &c0) != 1) {
				error("comando invalido", line);
			}
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

void libera_codigo(void *p) {}
