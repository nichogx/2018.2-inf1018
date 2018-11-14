#include <stdio.h>
#include <stdlib.h>

typedef int(*funcp)(int x);

void gera_codigo(FILE *f, void **code, funcp *entry);
void libera_codigo(void *p);

int main(void) {
	void *cd;
	funcp entry;

	FILE *f = fopen("programa", "r");
	if (f == NULL) exit(1);

	gera_codigo(f, &cd, &entry);
	printf("%d\n", (*entry)(10));
	for (int i = 0; ((unsigned char *) entry)[i] != 0xc3; i++) {
		printf("%02x ", ((unsigned char *) entry)[i]);
	} printf("\n");
	libera_codigo(cd);

	return 0;
}
