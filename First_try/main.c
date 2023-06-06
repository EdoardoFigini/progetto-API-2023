#include <stdio.h>
#include <stdlib.h>

#define MAX_COMMAND 18

typedef struct nodo_{
    unsigned short int key;
    struct nodo_ *next;
} nodo_t;

typedef struct stazione_ {
	unsigned short int distanza;
	nodo_t* list_auto;
	struct stazione_* next;
	struct stazione_* prev;
} stazione_t;

struct list {
    stazione_t *head;
};

static stazione_t* list_insert(struct list*, unsigned short int);
static stazione_t* list_inorder_insert(struct list*, unsigned short int);
static stazione_t* list_search(struct list*, unsigned short int);
static void list_delete(struct list*, stazione_t*);
static void list_print(struct list, FILE*);

static nodo_t* int_list_insert(nodo_t**, unsigned short int);
static nodo_t* int_list_inorder_insert(nodo_t**, unsigned short int);
static nodo_t* int_list_search(nodo_t**, unsigned short int);
static void int_list_delete(nodo_t**, unsigned short int);
static void int_list_print(nodo_t*, FILE*);

static void parse_and_execute(struct list*);
static void aggiungi_stazione(struct list*, unsigned short int, unsigned short int);
static void aggiungi_auto(struct list*, unsigned short int, unsigned short int);
static void rottama_auto(struct list*, unsigned short int, unsigned short int);
static void aggiungi_auto_alla_stazione(stazione_t*, unsigned short int);
static void demolisci_stazione(struct list*, unsigned short int);
static void pianifica_percorso(struct list*, unsigned short int, unsigned short int);
static void trova_percorso(nodo_t**, struct list*, stazione_t*, stazione_t*);

void debug_list(struct list list, FILE* out_file){
    stazione_t* s;
    nodo_t* n;

    fprintf(out_file, "Autostrada: \n");
    for(s=list.head; s != NULL; s = s->next){
        fprintf(out_file, "\t|%2hu| --> ", s->distanza);
        for(n = s->list_auto; n!=NULL; n=n->next){
            fprintf(out_file, "%2hu --> ", n->key);
        }
        fprintf(out_file, "NIL\n");
    }
}

int main(int argc, char** argv){
    struct list autostrada;

    // init
    autostrada.head = NULL;

    // parse and execute
    parse_and_execute(&autostrada);

    return 0;
}

static stazione_t* list_insert(struct list* list, unsigned short int distanza){
    stazione_t* x;

    x = (stazione_t*)malloc(sizeof(stazione_t));
    if(x != NULL){
        x->next = list->head;
        x->distanza = distanza;
        x->list_auto = NULL;
        if(list->head != NULL){
            list->head->prev = x;
        }
        list->head = x;
        x->prev = NULL;
    } else {
        fprintf(stderr, "[-] Errore malloc");
    }
    return x;
}

static stazione_t* list_inorder_insert(struct list* list, unsigned short int distanza){
    stazione_t *x, *tmp;

    x = (stazione_t*)malloc(sizeof(stazione_t));
    if(x != NULL){
        x->distanza = distanza;
        x->list_auto = NULL;
        if(list->head != NULL && distanza > list->head->distanza){
            for(tmp=list->head; tmp->next != NULL && distanza > tmp->next->distanza; tmp = tmp->next);
            x->next = tmp->next;
            tmp->next = x;
            x->prev = tmp;
        } else {
            x->next = list->head;
            list->head = x;
            x->prev = NULL;
        }
    }
    return x;
}

static stazione_t* list_search(struct list* list, unsigned short int k){
    stazione_t* x;
    x = list->head;
    while (x != NULL && x->distanza != k){
        x = x->next;
    }
    return x;
}

static void list_delete(struct list* list, stazione_t* x){
    nodo_t* tmp;

    if (x->prev != NULL){
        x->prev->next = x->next;
    } else {
        list->head = x->next;
    }
    if (x->next != NULL){
        x->next->prev = x->prev;
    }
    while(x->list_auto != NULL){
        tmp = x->list_auto;
        x->list_auto = x->list_auto->next;
        free(tmp);
    }
    free(x);
}

static void list_print(struct list list, FILE* outfile){
    stazione_t* x;

    fprintf(outfile, "List: Head --> ");
    for(x = list.head; x != NULL; x = x->next){
        fprintf(outfile, "%hu <-> ", x->distanza);
    }
    fprintf(outfile, "NIL");
}

static nodo_t* int_list_insert(nodo_t** list_head, unsigned short int key){
    nodo_t* x;
    x = (nodo_t*)malloc(sizeof(nodo_t));
    if(x != NULL){
        x->key = key;
        x->next = *list_head;
        *list_head = x;
    }
}

static nodo_t* int_list_inorder_insert(nodo_t** list_head, unsigned short int key){
    nodo_t *x, *tmp;
    x = (nodo_t*)malloc(sizeof(nodo_t));
    if(x != NULL){
        x->key = key;
        if(*list_head != NULL && key < (*list_head)->key){
            for(tmp = *list_head; tmp->next != NULL && tmp->next->key > key; tmp = tmp->next);
            x->next = tmp->next;
            tmp->next = x;
        } else {
            x -> next = *list_head;
            *list_head = x;
        }
    }
    return x;
}

static nodo_t* int_list_search(nodo_t** list_head, unsigned short int k){
    nodo_t* x;
    x = *list_head;
    while (x != NULL && x->key != k){
        x = x->next;
    }
    return x;
}

static void int_list_delete(nodo_t** list_head, unsigned short int key){
    nodo_t *curr, *prec;
    curr = *list_head;
    prec = NULL;
    while(curr && curr->key != key){
        prec = curr;
        curr = curr->next;    
    }
    if(curr != NULL){
        if(prec != NULL){
            prec->next = curr->next;
        }else{
            *list_head = curr->next;
        }
        free(curr);    
    }
}

static void int_list_print(nodo_t* list, FILE* outfile){
    nodo_t* x;

    // fprintf(outfile, "List: Head --> ");
    for(x=list; x!=NULL; x = x->next){
        fprintf(outfile, "%hu ", x->key);
    }
    // fprintf(outfile, "NIL\n");
    fprintf(outfile, "\n");
}

static void parse_and_execute(struct list* list){
    char command[MAX_COMMAND + 1];
    unsigned short int arg1, arg2, argn;

    while(fscanf(stdin, " %s", command) != EOF){
        switch (command[0])
        {
            case 'd':
                scanf("%hu %hu", &arg1);
                demolisci_stazione(list, arg1);
                break;
            case 'r':
                scanf("%hu %hu", &arg1, &arg2);
                rottama_auto(list, arg1, arg2);
                break;
            case 'p':
                scanf("%hu %hu", &arg1, &arg2);
                pianifica_percorso(list, arg1, arg2);
                break;
            case 'a':
                if (command[9]=='s'){
                    scanf("%hu %hu", &arg1, &arg2);
                    aggiungi_stazione(list, arg1, arg2);

                } else {
                    scanf("%hu %hu", &arg1, &arg2);
                    aggiungi_auto(list, arg1, arg2);
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

static void aggiungi_stazione(struct list* list, unsigned short int dist, unsigned short int num_auto){
    short unsigned int i, n, a;
    stazione_t* s;

    s = list_inorder_insert(list, dist);
    // fprintf(stdout, "[+] Aggiunta stazione %d con %d auto\n", dist, num_auto);
    if(s==NULL){
        fprintf(stdout, "non aggiunta\n");
        return;
    }
    for(i=0; i<num_auto; i++){
        fscanf(stdin, " %hu", &a);
        aggiungi_auto_alla_stazione(s, a);
    }
    fprintf(stdout, "aggiunta\n");
}

static void aggiungi_auto(struct list* list, unsigned short int stazione, unsigned short int autonomia){
    stazione_t* s;

    s = list_search(list, stazione);
    if(s == NULL){
        fprintf(stdout, "non aggiunta\n");
        return;
    }
    aggiungi_auto_alla_stazione(s, autonomia);

    fprintf(stdout, "aggiunta\n");
}

static void aggiungi_auto_alla_stazione(stazione_t* stazione, unsigned short int autonomia){
    nodo_t* s;
    s = int_list_inorder_insert(&(stazione->list_auto), autonomia);
    
    // fprintf(stdout, "[+] Auto con autonomia %hu aggiunta a %hu\n", s->key, stazione->distanza);
    return;
}

static void rottama_auto(struct list* list, unsigned short int stazione, unsigned short int autonomia){
    stazione_t* s;
    nodo_t* a;

    s = list_search(list, stazione);
    if(s == NULL){
        // fprintf(stderr, "[-] Stazione %d non trovata\n", stazione);
        fprintf(stdout, "non rottamata\n");
        return;
    }

    a = int_list_search(&(s->list_auto), autonomia);
    if(a == NULL){
        // fprintf(stderr, "[-] Stazione %d non trovata\n", stazione);
        fprintf(stdout, "non rottamata\n");
        return;
    }
    int_list_delete(&(s->list_auto), autonomia);

    // fprintf(stdout, "[+] Auto %d rottamata\n", autonomia);
    fprintf(stdout, "rottamata\n");
}

static void demolisci_stazione(struct list* list, unsigned short int dist){
    stazione_t* s;
    nodo_t *tmp;

    s = list_search(list, dist);
    if(s == NULL){
        // fprintf(stderr, "[-] Stazione %d non trovata\n", dist);
        fprintf(stdout, "non demolita\n");
        return;
    }

    list_delete(list, s);
    // fprintf(stdout, "[+] Stazione %d demolita\n", dist);
    fprintf(stdout, "demolita\n");
}

static void pianifica_percorso(struct list* list, unsigned short int start, unsigned short int end){
    stazione_t *partenza, *arrivo;
    int distance;
    nodo_t* percorso;

    percorso = NULL;

    distance = abs(end-start);

    partenza = list_search(list, start);
    arrivo = list_search(list, end);
    if(partenza == NULL || arrivo == NULL){
        fprintf(stdout, "nessun percorso");
        return;
    }
    if(distance < partenza->list_auto->key){
        fprintf(stdout, "%d %d\n", start, end);
    } else {
        int_list_insert(&percorso, end);
        trova_percorso(&percorso, list, partenza, arrivo);
        int_list_print(percorso, stdout);
        int_list_print(percorso, stderr);
        fprintf(stderr, "----------------------------------------------------------------------------------\n");
    }
}

static void trova_percorso(nodo_t** percorso, struct list* list, stazione_t* start, stazione_t* end){
    unsigned short int autonomia;
    stazione_t* tmp;
    fprintf(stderr, "%d -> %d\n", start->distanza, end->distanza);

    autonomia = start->list_auto->key;
    if(autonomia < abs(end->distanza - start->distanza)){
        // for(tmp = list; tmp != NULL && abs(tmp->distanza - start->distanza) <= autonomia; tmp = tmp->next);
        //     fprintf(stderr, "abs(%2d - %2d) = %2d < %2d -> %2d\n", tmp->distanza, start->distanza, abs(tmp->distanza - start->distanza), autonomia, abs(tmp->distanza - start->distanza) < autonomia);
        // }
        // fprintf(stderr, "abs(%2d - %2d) = %2d < %2d -> %2d\n", tmp->distanza, start->distanza, abs(tmp->distanza - start->distanza), autonomia, abs(tmp->distanza - start->distanza) < autonomia);

        if(end->distanza > start->distanza){
            for(tmp = start->next; tmp != NULL && tmp != end && abs(tmp->distanza - start->distanza) <= autonomia; tmp = tmp->next);
            trova_percorso(percorso, list, tmp->prev, end);
        } else {
            for(tmp = end->next; tmp != NULL && tmp != start && abs(tmp->distanza - start->distanza) <= autonomia; tmp = tmp->next){
                fprintf(stderr, "a: abs(%2d - %2d) = %2d < %2d -> %2d\n", tmp->distanza, start->distanza, abs(tmp->distanza - start->distanza), autonomia, abs(tmp->distanza - start->distanza) < autonomia);
            }
            fprintf(stderr, "b: abs(%2d - %2d) = %2d < %2d -> %2d\n", tmp->distanza, start->distanza, abs(tmp->distanza - start->distanza), autonomia, abs(tmp->distanza - start->distanza) < autonomia);

            trova_percorso(percorso, list, tmp->next, end);
        }
    }
    int_list_insert(percorso, start->distanza);
}