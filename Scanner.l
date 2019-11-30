%{
#include <Parser.tab.h>
extern YYSTYPE yylval;
void abrirArq();
%}

DIGITO [0-9]
LETRA [a-zA-Z]
OUTRO [^0-9a-zA-Z;/=\-"+""*""("")""{""[""}"">""<""!"",""\n""\t""\]"]
NEWLINE [\n]
TAB [\t]

%%
"/*"                    {flag_coment =1; while (token != FIMCOM && token != FIM) token = yylex();}
"*/"                    {flag_coment =0;return FIMCOM;}
{DIGITO}+		            {return NUM;}
"int"                   {return INT;}
"void"                  {return VOID;}
"return"                {return RET;}
"if"                    {return IF;}
"else"                  {return ELSE;}
"while"                 {return WHI;}
{LETRA}+	              {return ID;}
({LETRA}|{DIGITO})+     {if(!flag_coment) printf("Erro Léxico na linha %d. Lexema: %s\n",num_lines,yytext);return ERR;}
"+"				              {return SOM;}
"-"				              {return SUB;}
"*"				              {return MUL;}
"/"				              {return DIV;}
">="                    {return MAIG;}
"<="                    {return MEIG;}
"=="                    {return IGL;}
"="				              {return ATR;}
">"                     {return MAIO;}
"<"                     {return MENO;}
"!="                    {return DIF;}
","                     {return VIRG;}
";"				              {return PEV;}
"("				              {return APR;}
")"				              {return FPR;}
"{"                     {return ACH;}
"}"                     {return FCH;}
"["                     {return ACO;}
"]"                     {return FCO;}
{TAB}                   {}
{NEWLINE}               {num_lines++;}
[ ]*                    {}
{OUTRO}+		            {if(!flag_coment) printf("Erro Léxico na linha %d. Lexema: %s\n",num_lines,yytext);return ERR;}
<<EOF>>                 {}
%%

void abrirArq()
{
  yyin = fopen("entrada.txt", "r");
}