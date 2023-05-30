#include <stdio.h>
#include <stdlib.h>


typedef struct stazione {
	unsigned short int distanza;
	unsigned short int max_auto;
	struct stazione* next;
	struct stazione* prev;
} stazione_t;

struct list {
    stazione_t *head;
};

static void list_insert(struct list*, int, int);
static unsigned short int parse_command();

int main(int argc, char** argv){
    struct list autostrada;

    autostrada.head = NULL;

    list_insert(&autostrada, 10, 50);

    // fprintf(stdout, "%p", autostrada);
    fprintf(stdout, "%d\n%d\n", autostrada.head->distanza, autostrada.head->max_auto);

    return 0;
}

static void list_insert(struct list* list, int distanza, int max_auto){
    stazione_t* x;

    x = (stazione_t*)malloc(sizeof(stazione_t));
    if(x != NULL){
        x->next = list->head;
        x->distanza = distanza;
        x->max_auto = max_auto;
        if(list->head != NULL){
            list->head->prev = x;
        }
        list->head = x;
        x->prev = NULL;
    } else {
        fprintf(stderr, "Errore malloc");
    }
}

static unsigned short int parse_command(){

}