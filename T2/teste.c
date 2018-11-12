#include <stdio.h>
#include <stdlib.h>

typedef int(*funcp)(int x);

void gera_codigo2(void **code, funcp *entry);

int main(void) {
	funcp entry;
	printf("%d\n", entry(0));

	return 0;
}