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
	libera_codigo(cd);

	return 0;
}
