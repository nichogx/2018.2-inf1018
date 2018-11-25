#include <stdio.h>
#include <stdlib.h>

typedef int(*funcp)(int x);

void gera_codigo(FILE *f, void **code, funcp *entry);
void libera_codigo(void *p);

int main(void) {
	void *cd;
	funcp entry;
	int numTestes;
	char padrao[9] = "programa";
	printf("Digite o numero de testes que deseja fazer: ");
	scanf("%d",&numTestes);
	while ( numTestes > 0 )
	{
		char * novo;
		if(numTestes < 10)
		{
			novo = (char *) malloc(10*sizeof(char));
			snprintf(novo,10,"%s%d",padrao,numTestes);
			printf("%s\n",novo);
		}
		else
		{
			novo = (char *) malloc(11*sizeof(char));
			snprintf(novo,11,"%s%d",padrao,numTestes);
			printf("%s\n",novo);
		}
		FILE *f = fopen(novo, "r");
		if (f == NULL) exit(1);

		gera_codigo(f, &cd, &entry);

		#ifdef _DEBUG
		printf("FUNCAO DE ENTRADA:\n");
		for (int i = 0; ((unsigned char *) entry)[i] != 0xc3 && i < 100; i++) {
			printf("%02x ", ((unsigned char *) entry)[i]);
		} printf("c3 \n\n");
		#endif

		int a = (*entry)(6);
		printf("Retorno: %d\n", a);

		libera_codigo(cd);
		numTestes--;
	}
	return 0;
}
