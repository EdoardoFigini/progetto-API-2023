#include <stdio.h>
#include <stdlib.h>

#define HASH_M          8
#define HASH_A          6.179952383 
#define HASH_TABLE_SIZE 128
#define MAX_COMMAND     18

typedef struct stazione_ {
    int key;
    struct stazione_* next;
} stazione_t;

static stazione_t* alloc_hash(unsigned int dim){
    return (stazione_t*)malloc(dim * sizeof(stazione_t));
}

static double decimal(double n){
    return n - (int)n;
}

static int hash(int k){
    return (int)(HASH_M * decimal(k * HASH_A));  
}

static int hash_insert(stazione_t* a, int k){
    int i = 0;
    while(i<HASH_TABLE_SIZE){
    }

    return 0;
}

static void aggiungi_stazione(stazione_t* autostrada, unsigned short int stazione, unsigned short int autonomia){
    return;
}

static void aggiungi_auto(stazione_t* autostrada, unsigned short int stazione, unsigned short int autonomia){
    return;
}

static void append_car_to_station(stazione_t* stazione, unsigned short int autonomia){
    return;
}

static void rottama_auto(stazione_t* autostrada, unsigned short int stazione, unsigned short int autonomia){
    return;
}

static void demolisci_stazione(stazione_t* autostrada, unsigned short int dist){
    return;
}

static void pianifica_percorso(stazione_t* autostrada, unsigned short int from, unsigned short int to){
    return;
}

static void parse_and_execute(stazione_t* autostrada){
    char command[MAX_COMMAND + 1];
    unsigned short int arg1, arg2;

    while(fscanf(stdin, " %s", command) != EOF){
        switch (command[0])
        {
            case 'd':
                scanf("%hu", &arg1);
                demolisci_stazione(autostrada, arg1);
                break;
            case 'r':
                scanf("%hu %hu", &arg1, &arg2);
                rottama_auto(autostrada, arg1, arg2);
                break;
            case 'p':
                scanf("%hu %hu", &arg1, &arg2);
                pianifica_percorso(autostrada, arg1, arg2);
                break;
            case 'a':
                if (command[9]=='s'){
                    scanf("%hu %hu", &arg1, &arg2);
                    aggiungi_stazione(autostrada, arg1, arg2);

                } else {
                    scanf("%hu %hu", &arg1, &arg2);
                    aggiungi_auto(autostrada, arg1, arg2);
                }
                break;
            default:
                break;
        }
        // fprintf(stderr, "%s\n", command);
        // debug_list(*list, stderr);
        // fprintf(stderr, "-----------------------------------------------------------------------------------\n");
    }
}

int main(int argc, char** argv){
    stazione_t* autostrada;

    autostrada = alloc_hash(HASH_TABLE_SIZE);

    parse_and_execute(autostrada);

    return 0;
}
