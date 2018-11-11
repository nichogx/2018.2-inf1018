/* Modulo de Definicao: grava_structs
 * Arquivo: grava_structs.h
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

/* FUNCAO: dump_structs
 * 
 * EXIBE: - uma linha indicando a orientacao do arquivo ('L' ou 'B')
 *        - uma linha indicando o numero de structs armazenadas 
 *          no arquio (decimal)
 *        - dump dos valores armazenados, em hexadecimal, com um 
 *          campo por linha. cada byte deve ser exibido em hexa,
 *          com dois digitos. deve haver um unico espaco entre
 *          dois bytes
 *        - uma linha separadora no inicio de cada struct (caractere '*')          
 *
 * RECEBE: char *arquivo - o nome do arquivo a ser lido
 *
 * RETORNA: void
 *
 * NAO HA VALIDACAO DOS PARAMETROS RECEBIDOS!
 */

void dump_structs(char *arquivo);

