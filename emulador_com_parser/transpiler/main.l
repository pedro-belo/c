%option noyywrap
%option yylineno

%{
    #include <stdio.h>
    #include "./main.tab.h"
    #include "./defs.h"

    extern YYSTYPE yylval;
%}

space ("\ "|"\t")+
blank ({space}|"\n")

dw "DW"
hlt "HLT"
lda "LDA"
str "STR"
add "ADD"
sub "SUB"
jz "JZ"
jp "JP"
jn "JN"
jmp "JMP"

number [0-9]+
comment ";"[^\n]*
mnemonic ({lda}|{str}|{add}|{sub}|{jz}|{jp}|{jn}|{jmp})

%%

{dw}  {                     return DW;       }
{hlt} { yylval.integer = 0; return HLT;      }
{lda} { yylval.integer = 1; return MNEMONIC; }
{str} { yylval.integer = 2; return MNEMONIC; }
{add} { yylval.integer = 3; return MNEMONIC; }
{sub} { yylval.integer = 4; return MNEMONIC; }
{jz}  { yylval.integer = 5; return MNEMONIC; }
{jp}  { yylval.integer = 6; return MNEMONIC; }
{jn}  { yylval.integer = 7; return MNEMONIC; }
{jmp} { yylval.integer = 8; return MNEMONIC; }
{number} { yylval.integer = atoi(yytext); return INTEGER; }
{comment} { }
{blank} { }
<<EOF>> { return EIN; }

. { fprintf(stderr, ">> The token '%s' is not defined.", yytext); return TKERR; }

%%
