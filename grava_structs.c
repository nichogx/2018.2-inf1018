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
 * ngx         0.20      2018-09-20 Gravacao do cabecalho dos arquivos
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
	unsigned char segByte  = 0;
	
	char *pCampos = campos;
	
	unsigned char atual = 0;
	int contAtual = 0;
	
	if (arq == NULL) return -1;
	
	for (pCampos = campos; *pCampos; pCampos++, qtCampos++); /* conta campos */

	fwrite(&nstructs, 1, 1, arq); /* escreve no arquivo o byte menos significativo de nstructs */
	if (ord == 'L') {
		segByte = 0x80; /* seta o bit mais significativo para 1 */
	}
	segByte = segByte | qtCampos; /* assume que qtCampos <= 127 && qtCampos >= 0 (argumentos passados corretamente) */
	fwrite(&segByte, 1, 1, arq);
	
	for (pCampos = campos; *pCampos; pCampos++) {
		unsigned char aColocar = 0;
		if (*pCampos == 's') {
			aColocar = 1;
		} else if (*pCampos == 'i') {
			aColocar = 2;
		} else if (*pCampos == 'l') {
			aColocar = 3;
		}
		
		if (contAtual > 3) {
			fwrite(&atual, 1, 1, arq);
			contAtual = 0;
			atual = 0;
		}
		atual = atual << 2;
		atual = aColocar | atual;
		contAtual++;
	} if (contAtual) { /* completa com zeros se ainda faltar */
		atual = atual << ((4 - contAtual) * 2);
		fwrite(&atual, 1, 1, arq);
	}
	
	for (pCampos = campos; *pCampos; pCampos++) {
		/* aqui preencher com os dados das structs */
	}
	
	fclose(arq);
	return 0;
}

/* FUNCOES ENCAPSULADAS NO MODULO */
