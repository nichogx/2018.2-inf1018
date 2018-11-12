#include <stdlib.h>

typedef int (*funcp)(int x);

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
	return 0;
}

void gera_codigo2(void **code, funcp *entry) {
	*code = malloc(14);
	entry = code + 1;
}

void libera_codigo(void *p) {}