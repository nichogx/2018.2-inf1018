/* Nicholas de Godoy Xavier 1710878 3WA */
/* Alexandre Augusto Poggio Heine 1711905 3WA */

/* Modulo de Implementacao: grava_structs
 * Arquivo: grava_structs.c
 * 
 * Autores:
 * ngx - Nicholas Godoy
 * alx - Alexandre Augusto
 *
 * Versionamento:
 * Autor(es)   Versao    Data       Descricao
 * ngx         0.10      2018-09-18 Inicial
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "grava_structs.h"

/* CABECALHO DAS FUNCOES ENCAPSULADAS NO MODULO */

/* FUNCOES EXPORTADAS PELO MODULO */
int grava_structs(int nstructs, void *valores, char *campos, char ord, char *arquivo)
{
	FILE *arq = fopen(arquivo, "wb");
	unsigned char qtCampos = 0;
	char *pCampos = campos;

	for (; *pCampos; pCampos++, qtCampos++); /* conta campos */
	pCampos = campos; /* reseta pCampos */
	
	if (arq == NULL) {
		return -1;
	}

	fputc(nstructs, arq); /* escreve no arquivo o byte menos significativo de nstructs */
	if (ord == 'L') {
		unsigned char segByte = 0x80; /* seta o bit mais significativo para 1 */
		segByte = segByte | qtCampos;
		fputc(segByte, arq);
	}
	
	for (; *pCampos; pCampos++) {
		if (*pCampos == 'c');
	}
	
	fclose(arq);
	return 0;
}

/* FUNCOES ENCAPSULADAS NO MODULO */
