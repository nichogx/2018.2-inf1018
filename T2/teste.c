/*#include <stdio.h>
#include <stdlib.h>

typedef int(*funcp)(int x);

unsigned char codigo[] = { 0x83, 0xc7, 0x01, 0x89, 0xf8, 0xc3 };

void gera_codigo2(void **code, funcp *entry);

int main(void) {
	void *cd;
	funcp f = (funcp)codigo;
	funcp entry;
	printf("%d\n", (*f)(10));
	gera_codigo2(&cd, &entry);
	printf("%d\n", (*entry)(10));

	return 0;
}*/
#include <stdio.h>
unsigned char codigo[] = { 0x83, 0xc7, 0x01, 0x89, 0xf8, 0xc3 };

typedef int(*funcp) (int x);

int main(void) {
	funcp f = (funcp)codigo;

	printf("%d\n", (*f)(10));
	return 0;
}
