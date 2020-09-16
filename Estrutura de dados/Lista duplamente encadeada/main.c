#include "main.h"

int main(void)
{

    struct LDE *lde = lde_init();

    inserir(lde, 3, INICIO);
    inserir(lde, 1, FIM);
    inserir(lde, 2, INICIO);

    print_lde(lde);

    remove_item(lde, 1);
    remove_item(lde, 2);
    remove_item(lde, 3);

    
    print_lde(lde);

    printf("\n\nEsta vazio ? %s\n\n", is_empty(lde) ? "Sim" : "Nao");

    return 0;
}

// gcc -Wall -c main.c lib/LDE.c && gcc main.o LDE.o -o prog && ./prog