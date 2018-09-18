/* Modulo de Definicao: grava_structs
 * Arquivo: grava_structs.h
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

/* FUNCOES DE ACESSO */

/* FUNCAO: grava_structs
 *         
 * RECEBE: int nstructs  - numero de elementos do array de
 *                         structs a ser escrito em arquivo
 *         void *valores - endereco desse array
 *         char *campos  - descricao dos campos das structs
 *                         que compoem o array. valores validos:
 *                           'c' - char
 *                           's' - short int
 *                           'i' - int
 *                           'l' - long int
 *         char ord      - um caractere que indica se os valores
 *                         inteiros contidos nas estruturas devem
 *                         ser armazenados no arquivo em ordenacao
 *                         little endian ('L') ou big endian ('B')
 *         char *arquivo - o nome do arquivo a ser criado
 *
 * RETORNA: 0 em caso de sucesso, -1 em caso de erro (apenas erros na
 *          abertura ou gravacao de um arquivo sao considerados)
 *
 * NAO HA VALIDACAO DOS PARAMETROS RECEBIDOS!
 */

int grava_structs(int nstructs, void *valores, char *campos, char ord, char *arquivo);


