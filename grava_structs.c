/* Nicholas de Godoy Xavier 1710878 3WA */
/* Alexandre Augusto Poggio Heine 1711905 3WA */

/* Modulo de Implementacao: grava_structs
 * Arquivo: grava_structs.c
 * 
 * Autores:
 * alx - Alexandre Augusto
 * ngx - Nicholas Godoy
 *
 * Versionamento:
 * Autor(es)   Versao    Data       Descricao
 * alx         0.40      2018-09-24 Continuacao do desenvolvimento - dump_structs
 *                                  imprime o tipo endian, o tamanho do array de
 *                                  strucs e os campos das structs como esperado.
 * alx, ngx    0.30      2018-09-23 Continuacao do desenvolvimento - grava_structs
 *                                  funciona como esperado para LEndian e BEndian.
 * ngx         0.20      2018-09-20 Gravacao do cabecalho dos arquivos
 * ngx         0.10      2018-09-18 Inicial
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "grava_structs.h"

/* CABECALHO DAS FUNCOES ENCAPSULADAS NO MODULO */

int leCampo(char c);
int power(int bas, int exp);

/* FUNCOES EXPORTADAS PELO MODULO */
int grava_structs(int nstructs, void *valores, char *campos, char ord, char *arquivo)
{
	FILE *arq = fopen(arquivo, "wb");
	
	unsigned char qtCampos = 0; /* quantidade de campos da struct */
	unsigned char segByte  = 0; /* segundo byte, inicializado como sendo big endian */
	unsigned char maiorCampo = leCampo(*campos);
	
	char *pCampos = campos; /* ponteiro para a string de campos */
	char *pVals = valores; /* ponteiro para o array de structs */
	
	unsigned char atual = 0; /*  */
	int contAtual = 0;
	
	int i;
	
	if (arq == NULL) return -1;
	
	for (pCampos = campos; *pCampos; pCampos++, qtCampos++) { /* conta campos e acha o maior */
		unsigned char campAt = leCampo(*pCampos);
		if (campAt > maiorCampo) {
			maiorCampo = campAt;
		}
	}

	fwrite(&nstructs, 1, 1, arq); /* escreve no arquivo o byte menos significativo de nstructs */
	                              /* assume que nstructs <= 255 (argumentos passados corretamente) */
	if (ord == 'L') {
		segByte = 0x80; /* seta o bit mais significativo para 1 */
	}
	segByte = segByte | qtCampos; /* assume que qtCampos <= 127 && qtCampos >= 0 (argumentos passados corretamente) */
	fwrite(&segByte, 1, 1, arq);
	
	for (pCampos = campos; *pCampos; pCampos++) {
		if (contAtual > 3) {
			fwrite(&atual, 1, 1, arq);
			contAtual = 0;
			atual = 0;
		}
		atual = atual << 2;
		atual = leCampo(*pCampos) | atual;
		contAtual++;
	} if (contAtual) { /* completa com zeros se ainda faltar */
		atual = atual << ((4 - contAtual) * 2);
		fwrite(&atual, 1, 1, arq);
	}
	
	for (i = 0; i < nstructs; i++) {
		unsigned char valAtual = 0;
		for (pCampos = campos; *pCampos; pCampos++) {
			unsigned char campAtual = leCampo(*pCampos);
			unsigned char tam = power(2, campAtual);
			
			while (valAtual % tam) { /* anda pelo padding */
				pVals++;
				valAtual++;
			}
			
			if (0x80 & segByte) { /* eh little endian */
				fwrite(pVals, tam, 1, arq);
			} else { /* big endian */
				int j = tam;
				for (; j > 0; j--)
					fwrite(pVals + j - 1, 1, 1, arq);
			}
			
			pVals += tam;
			valAtual += tam;
		}
		while (valAtual % power(2, maiorCampo)) { /* anda pelo padding */
			pVals++;
			valAtual++;
		}
	}
	
	fclose(arq);
	return 0;
}

void dump_structs(char *arquivo)
{

	FILE *arq = fopen(arquivo,"rb");

	int i = 0; /* índice para o vetor de campos */
	
	unsigned char numStruct; /* número de structs da array do arquivo */
	unsigned char endianType; /* little ou big endian */
	unsigned char qtCampos; /* quantidade de campos da struct original */
	
	unsigned char *vetCampo; /* vetor com os campos da struct */
	
	fread(&numStruct, sizeof(char), 1, arq); /* leu número de structs */
	fread(&qtCampos, sizeof(char), 1, arq); /* leu byte do tipo endian & número de campos */
	
	endianType = qtCampos >> 7;
	qtCampos = qtCampos & 0x7F; /* retirou o bit indicador de tipo endian */
	
	vetCampo = (unsigned char *) malloc((qtCampos+1)*sizeof(char)); /* define quantidade de campos para o array */
	if(vetCampo == NULL) /* Quero conversar com o professor sobre essas linhas (128-133) */
	{
		printf("Erro de memoria.");
		return;
	}
	
	if(endianType) /* print para tipo endian */
		printf("L\n");
	else
		printf("B\n");
	
	printf("%d\n", numStruct); /* print para tamanho do array */
	
	while(i != qtCampos) {/* grava em tpCampo os campos da struct */
		int j;
		unsigned char campos = 0; /* byte com campos */
		
		fread(&campos, sizeof(char), 1, arq);
		
		for(j = 3; (j >= 0) && (i != qtCampos); j--) {/* grava os 4 campos armazenados em um byte até a quantidade de campos ter terminado ou o byte terminar */
			
			switch(campos >> (2*j) & 0x03) { /* identifica o campo do conjunto de 2 bits verificado */
			case 0:	vetCampo[i] = 'c'; break;
			case 1:	vetCampo[i] = 's'; break;
			case 2:	vetCampo[i] = 'i'; break;
			case 3: vetCampo[i] = 'l'; break;
			}
			i++;
		}
	}
	vetCampo[i] = '\0';
	
	/* Considerando que a parte anterior funcione */
	
	while(numStruct) /* Para cada struct, printa os campos das structs desconsiderando paddings */
	{
		printf("*\n");
		for(i = 0; i < qtCampos; i++) /* Para cada campo, verifica o seu tipo */
		{
			int j;
			
			for(j = power(2,leCampo(vetCampo[i])); j != 0; j--) /* printa todos os bytes de um campo */
			{
				unsigned char byte;
				fread(&byte, sizeof(char), 1, arq);
				printf("%02x", byte);
				
				if(j != 1)
					printf(" ");
			}
			printf("\n");
		}
		numStruct--;
	}
	
	free(vetCampo);
	fclose(arq);
}

/* FUNCOES ENCAPSULADAS NO MODULO */

int leCampo(char c) {
	if (c == 's')
		return 1;
	
	if (c == 'i')
		return 2;
	
	if (c == 'l')
		return 3;
	
	return 0;
}

int power(int bas, int exp) {
	int i, res = 1;
	
	for (i = 0; i < exp; i++)
		res *= bas;
	
	return res;
}