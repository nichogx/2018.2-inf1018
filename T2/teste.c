#include <stdio.h>
#include <stdlib.h>

typedef int(*funcp)(int x);

void gera_codigo(FILE *f, void **code, funcp *entry);
void gera_codigo2(void **code, funcp *entry);
void libera_codigo(void *p);

int main(void) {
	void *cd;
	funcp entry;

	gera_codigo2(&cd, &entry);
	printf("%d\n", (*entry)(10));
	libera_codigo(cd);

	FILE *f = fopen("programa", "r");
	if (f == NULL) exit(1);

	gera_codigo(f, &cd, &entry);
	printf("%d\n", (*entry)(10));
	libera_codigo(cd);

	return 0;
}
