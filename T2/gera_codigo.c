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
	FILE *myfp;

	if ((myfp = fopen("programa", "r")) == NULL) {
		perror("nao conseguiu abrir arquivo!");
		exit(1);
	}

	while ((c = fgetc(myfp)) != EOF) {
		switch (c) {
		case 'f': { /* function */
			char c0;
			if (fscanf(myfp, "unction%c", &c0) != 1) {
				error("comando invalido", line);
			}
			printf("function\n");
			break;
		}
		case 'e': { /* end */
			char c0;
			if (fscanf(myfp, "nd%c", &c0) != 1) {
				error("comando invalido", line);
			}
			printf("end\n");
			break;
		}
		case 'r': {  /* retorno incondicional */
			int idx0, idx1;
			char var0, var1;
			if (fscanf(myfp, "et %c%d", &var0, &idx0) != 2) {
				error("comando invalido", line);
			}
			printf("ret %c%d\n", var0, idx0);
			break;
		}
		case 'z': {  /* retorno condicional */
			int idx0, idx1;
			char var0, var1;
			if (fscanf(myfp, "ret %c%d %c%d", &var0, &idx0, &var1, &idx1) != 4) {
				error("comando invalido", line);
			}
			printf("zret %c%d %c%d\n", var0, idx0, var1, idx1);
			break;
		}
		case 'v': {  /* atribuicao */
			int idx0;
			char var0 = c, c0;
			if (fscanf(myfp, "%d = %c", &idx0, &c0) != 2) {
				error("comando invalido", line);
			}

			if (c0 == 'c') { /* call */
				int f, idx1;
				char var1;
				if (fscanf(myfp, "all %d %c%d\n", &f, &var1, &idx1) != 3) {
					error("comando invalido", line);
				}
				printf("%c%d = call %d %c%d\n", var0, idx0, f, var1, idx1);
			} else { /* operaÃ§Ã£o aritmÃ©tica */
				int idx1, idx2;
				char var1 = c0, var2, op;
				if (fscanf(myfp, "%d %c %c%d", &idx1, &op, &var2, &idx2) != 4) {
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
		fscanf(myfp, " ");
	}
	return;
}

void gera_codigo2(void **codigo, funcp *entry) {
	unsigned char code[] = { 0x83, 0xc7, 0x01, 0x89, 0xf8, 0xc3 };
	//unsigned char *code = malloc(6);
	*entry = (funcp) code;/*
	code[0] = 0x55;
	code[1] = 0x48;
	code[2] = 0x89;
	code[3] = 0xe5;
	code[4] = 0xb8;
	code[5] = 0x64;
	code[6] = 0x00;
	code[7] = 0x00;
	code[8] = 0x00;
	code[9] = 0x48;
	code[10] = 0x89;
	code[11] = 0xec;
	code[12] = 0x5d;
	code[13] = 0xc3;*//*
	code[0] = 0x83;
	code[1] = 0xc7;
	code[2] = 0x01;
	code[3] = 0x89;
	code[4] = 0xf8;
	code[5] = 0xc3;*/
}

void libera_codigo(void *p) {}