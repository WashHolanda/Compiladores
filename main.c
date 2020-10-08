#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "analyze.h"
#include "cgen.h"
//#include "assembly.h"
//#include "binary.h"

extern int check_return;
int lineno = 1;
FILE * source;
FILE * listing;
FILE * code;

int TraceScan = FALSE; // Se TRUE, imprime tokens do scanner
int TraceParse = FALSE; // Se TRUE, imprime árvore sintática
int TraceAnalyze = TRUE; // Se TRUE, imprime tabela de símbolos
int TraceCode = FALSE; // Se TRUE, imprime como está sendo feito o Código Intermediário
int Error = FALSE;

int main( int argc, char * argv[] ) {
  TreeNode * syntaxTree;
  char pgm[120]; // nome do arquivo do código fonte 
  char path[120]; // nome do diretório do código fonte
  if (argc != 2) {
    fprintf(stderr,"Arquivo não especificado.\n Uso: %s <nome do arquivo>\n",argv[0]);
    exit(1);
  }
  strcpy(path,"codigos/");
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  strcat(path,pgm);
  source = fopen(path,"r");
  if (source==NULL) {
    fprintf(stderr,"Arquivo %s não encontrado.\n",path);
    exit(1);
  }
  listing = stdout;
  fprintf(listing,"\nCOMPILAÇÃO DO ARQUIVO C-: %s\n",pgm);

  syntaxTree = parse();
  if (TraceParse){
    if(Error == TRUE)
      exit(-1);
    fprintf(listing,"\nÁrvore Sintática:\n");
    printTree(syntaxTree);
  }
  buildSymtab(syntaxTree);
  if (TraceAnalyze){
     fprintf(listing,"Compilação Concluida!\n"); 
  } 

  if (!Error){
    char * codefile;
    int fnlen = strcspn(pgm,".");
    codefile = (char *) calloc(12+fnlen+4, sizeof(char));
    strcpy(codefile,"binarios/");
    strncat(codefile,pgm,fnlen);
    strcat(codefile,".inst");
    code = fopen(codefile,"w");
    if (code == NULL) {
      printf("Unable to open %s\n",codefile);
      exit(1);
    }
    fprintf(listing,"Criando Código Intermediário...\n");
    codeGen(syntaxTree,codefile);//GERADOR DE COD. INTERMED.
    fprintf(listing,"Código Intermediário Criado!\n");
    fprintf(listing,"\nGerando Código Assembly...\n");
    generateAssembly(getIntermediate());
    fprintf(listing,"\nCódigo Assembly Gerado!\n");
    /*fprintf(listing,"\nGerando Código Binário...\n");
    generateBinary(getAssembly(), getSize());
    fprintf(listing,"\nCódigo Binário Gerado!\n");*/
    fclose(code);
  } 
  fclose(source);
  return 0;
}
