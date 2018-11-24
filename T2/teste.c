#include <stdio.h>
#include <stdlib.h>

typedef int(*funcp)(int x);

void gera_codigo(FILE *f, void **code, funcp *entry);
void libera_codigo(void *p);

int main(void) {
	void *cd;
	funcp entry;
	
	FILE *f = fopen("programas/programa7", "r");
	if (f == NULL) exit(1);

	gera_codigo(f, &cd, &entry);

	#ifdef _DEBUG
	printf("FUNCAO DE ENTRADA:\n");
	for (int i = 0; ((unsigned char *) entry)[i] != 0xc3 && i < 100; i++) {
		printf("%02x ", ((unsigned char *) entry)[i]);
	} printf("c3 \n\n");
	#endif

	int a = (*entry)(5);
	printf("Retorno: %d\n", a);

	libera_codigo(cd);

	return 0;
}
