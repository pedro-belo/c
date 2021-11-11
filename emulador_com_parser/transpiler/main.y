%{

    #include <stdio.h>
    #include "./defs.h"
    #include "./lex.yy.c"

    unsigned short make_instruction(unsigned char c, unsigned short s);

    FILE *output;

%}

%token MNEMONIC INTEGER TKERR EIN HLT DW
%type <integer> MNEMONIC INTEGER HLT

%%

S:
    INSTRUCTIONS EIN {
        return 1;
    };

INSTRUCTIONS:
    ASR_INSTRUCTIONS INSTRUCTIONS { } | ASY_INSTRUCTIONS INSTRUCTIONS { } | { };


ASR_INSTRUCTIONS:
    DW INTEGER {
        fprintf(output, "%d\n", $2);
    };


ASY_INSTRUCTIONS:
    MNEMONIC INTEGER {
        fprintf(output, "%d\n", make_instruction($1, $2));
    } |

    HLT {
        fprintf(output, "%d\n", make_instruction($1, 0));
    };

%union {
    uint32_t integer;
};

%%

int main(int argc, char **argv) {

    if(argc != 2){
        fprintf(stderr, "Enter input file\n");
        return 1;
    }

    yyin = fopen(argv[1], "r");

    if(!yyin){
        printf("Cannot open '%s' !\n", argv[1]);
        return 1;
    }

    output = stdout;

    if(!yyparse())
        printf("parse fail\n");

    fclose(yyin);
    return 0;
}

int yyerror(char* s) {
	fprintf(stderr, "\nInvalid command on the line %d.\n%s (%s)\n\n",yylineno, s, yytext);
}

unsigned short make_instruction(unsigned char c, unsigned short s){
   return ( ( c << 8) & 0xF00) | (s & 0xFF);
}
