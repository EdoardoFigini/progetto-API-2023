#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_M          32
#define HASH_A          6.179952383 
#define NIL             -1
#define DELETED         0
#define MAX_COMMAND     18
#define INT_MAX         0xFFFFFFFF

struct int_list_node {
    unsigned short int key;
    struct int_list_node* next;
};

typedef struct {
    unsigned int dist;          // distanza
    struct int_list_node* cars; // lista di auto
} stazione_t;

struct adj_list {
    struct int_list_node* head;
    stazione_t stazione;
};

typedef struct hash_table_ {
    unsigned int m;
    struct adj_list* table;
} hash_table_t;

typedef struct graph {
    int v;
    hash_table_t array;
} graph_t;

static graph_t* alloc_graph(unsigned int dim){
    graph_t* graph = NULL;
    graph = (graph_t*)malloc(sizeof(graph_t));
    if(graph==NULL) return graph;
    
    graph->v = 0;
    graph->array.m = dim;
    graph->array.table = (struct adj_list*)malloc(dim * sizeof(struct adj_list));
    if(graph->array.table == NULL){
        free(graph);
        return NULL;
    }
    for(int i=0; i<graph->array.m; i++){
        graph->array.table[i].head = NULL;
        graph->array.table[i].stazione.dist = NIL;
        graph->array.table[i].stazione.cars = NULL;
    }
    return graph;
}

static double decimal(double n){
    return n - (int)n;
}

static int hash(int k, int i){
    return (int)(HASH_M * decimal((k+i) * HASH_A));  
}

static int hash_insert(hash_table_t t, int k){
    int j;

    for(int i=0; i<t.m; i++){
        j = hash(k, i);
        if(t.table[j].stazione.dist == NIL || t.table[j].stazione.dist == DELETED){
            t.table[j].stazione.dist = k;
            t.table[j].stazione.cars = NULL;
            return j;
        }
    }
    fprintf(stderr, "No more space in Table\n");
    return -1; // if ret -1 -> alloc new hash
}

static int hash_search(hash_table_t t, int k){
    int j;
    j = hash(k, 0);
    for(int i=0; t.table[j].stazione.dist != NIL && i < t.m; i++){
        if(t.table[j].stazione.dist == k) return j;
        j = hash(k, i);
    }

    return NIL;
}

static struct int_list_node* int_ordered_list_search(struct int_list_node* l, int k){
    struct int_list_node* x;
    if(l == NULL || k > l->key) return NULL;
    for(x = l; x!=NULL && x->key > k; x=x->next);
    if(x != NULL && x->key != k) x = NULL;
    return x;
}

static struct int_list_node* int_list_search(struct int_list_node* l, int k){
    struct int_list_node* x;
    if(l == NULL) return NULL;
    for(x=l; x!=NULL && x->key!=k; x=x->next);
    return x;
}

static void adj_list_insert(struct adj_list* l, int k){
    struct int_list_node* x;
    // if(int_list_search(l->head, k) != NULL) return;
    x = (struct int_list_node*)malloc(sizeof(struct int_list_node));
    if(x == NULL){
        fprintf(stderr, "Malloc error");
        return;
    }
    x->next = l->head;
    x->key = k;
    l->head = x;
    return;
}

static struct int_list_node* inorder_list_insert(struct int_list_node** l, int k){
    struct int_list_node* x;
    struct int_list_node* tmp;
    if(int_ordered_list_search(*l, k) != NULL) return NULL;
    x = (struct int_list_node*)malloc(sizeof(struct int_list_node));
    if(x != NULL){
        x->key = k;
        if(*l != NULL && k < (*l)->key){
            for(tmp = *l; tmp->next != NULL && tmp->next->key > k; tmp = tmp->next);
            x->next = tmp->next;
            tmp->next = x;
        } else {
            x->next = *l;
            *l = x;
        }
    }
    return x;
}

static void int_list_remove(struct int_list_node** l, int x){
    struct int_list_node *curr, *prec;
    curr = *l;
    prec = NULL;
    while(curr && curr->key != x){
        prec = curr;
        curr = curr->next;    
    }
    if(curr != NULL){
        if(prec != NULL){
            prec->next = curr->next;
        }else{
            *l = curr->next;
        }
        free(curr);    
    }
}

static void adj_list_remove(struct adj_list* l, int k){
    int_list_remove(&(l->head), k);
    return;
}

static void int_list_destroy(struct int_list_node* l){
    struct int_list_node *x;
    while(l!=NULL){
        x = l;
        l = l->next;
        free(x);
    }
    free(l);
}

static unsigned int* bellman_ford(graph_t* graph, int s){
    unsigned int* p;
    unsigned int* d;
    int index;
    struct int_list_node* x;
    
    p = (unsigned int*)malloc(graph->array.m * sizeof(unsigned int));
    d = (unsigned int*)malloc(graph->array.m * sizeof(unsigned int));
    memset(p, NIL,      graph->array.m * sizeof(unsigned int));
    memset(d, INT_MAX,  graph->array.m * sizeof(unsigned int));
    
    d[hash_search(graph->array, s)] = 0;
    
    for(int i=0; i<graph->array.m; i++){
        if(graph->array.table[i].stazione.dist != NIL){
            for(x=graph->array.table[i].head; x!=NULL; x=x->next){
                index = hash_search(graph->array, x->key);
                if(d[index] > d[i] + 1){
                    d[index] = d[i] + 1;
                    p[index] = i;
                }
            }
        }
    }

    return p;
}

static void aggiungi_stazione(graph_t* autostrada, unsigned short int dist, unsigned short int num){
    int index;
    unsigned short int argn;
    struct adj_list* table;
    int diff;

    if(hash_search(autostrada->array, dist) != NIL){
        fprintf(stdout, "non aggiunta\n");
        return;
    }

    index = hash_insert(autostrada->array, dist);
    // se index == NIL, va allocata nuova tabella
    if(index==NIL) return;
    table = autostrada->array.table;
    (autostrada->v)++;
    
    fprintf(stdout, "aggiunta\n");
    
    for(int i=0; i<num; i++){
        scanf(" %hu", &argn);
        inorder_list_insert(&(table[index].stazione.cars), argn);
    }
    if(autostrada->v == 1) return;
    for(int i=0; i<autostrada->array.m; i++){
        if(table[i].stazione.dist != NIL && table[i].stazione.dist != DELETED && i != index){
            diff = abs(dist - table[i].stazione.dist);
            if(table[i].stazione.cars != NULL && table[i].stazione.cars->key >= diff){
                adj_list_insert(table+i, table[index].stazione.dist);
            }
            if(table[index].stazione.cars != NULL && table[index].stazione.cars->key >= diff){
                adj_list_insert(table+index, table[i].stazione.dist);
            }
        }
    }

    return;
}

static void aggiungi_auto(graph_t* autostrada, unsigned short int stazione, unsigned short int autonomia){
    int index, diff;
    struct adj_list* table;

    index = hash_search(autostrada->array, stazione);
    if(index == NIL) {
        fprintf(stdout, "non aggiunta\n");
        return;
    }
    if(inorder_list_insert(&(autostrada->array.table[index].stazione.cars), autonomia) == NULL){
        fprintf(stdout, "non aggiunta\n");
        return;
    }
    
    fprintf(stdout, "aggiunta\n");
    
    table = autostrada->array.table;
    if(autostrada->v == 1 || autostrada->array.table[index].stazione.cars->key != autonomia) return;
    for(int i=0; i<autostrada->array.m; i++){
        if(table[i].stazione.dist != NIL && table[i].stazione.dist != DELETED && i != index){
            diff = abs(stazione - table[i].stazione.dist);
            if(table[index].stazione.cars != NULL && table[index].stazione.cars->key >= diff){
                adj_list_insert(table+index, table[i].stazione.dist);
            }
        }
    }
    
    return;
}

static void rottama_auto(graph_t* autostrada, unsigned short int stazione, unsigned short int autonomia){
    int index;
    index = hash_search(autostrada->array, stazione);
    if(index == NIL){
        fprintf(stdout, "non rottamata\n");
        return;
    }
    if(int_ordered_list_search(autostrada->array.table[index].stazione.cars, autonomia) == NULL){
        fprintf(stdout, "non rottamata\n");
        return;
    }
    int_list_remove(&(autostrada->array.table[index].stazione.cars), autonomia);
    fprintf(stdout, "rottamata\n");
    // TODO: se l'auto rottamata era la prima della lista, bisogna aggiornare la lista di adiacenza
    if(autostrada->v == 1 || autostrada->array.table[index].stazione.cars->key != autonomia) return;
    return;
}

static void demolisci_stazione(graph_t* autostrada, unsigned short int dist){
    int index;
    index = hash_search(autostrada->array, dist);
    if(index == NIL){
        fprintf(stdout, "non demolita\n");
        return;
    }
    autostrada->array.table[index].stazione.dist = DELETED;
    int_list_destroy(autostrada->array.table[index].stazione.cars);
    autostrada->array.table[index].stazione.cars = NULL;
    int_list_destroy(autostrada->array.table[index].head);
    autostrada->array.table[index].head = NULL;
    for(int i=0; i<autostrada->array.m; i++){
        adj_list_remove(autostrada->array.table+index, autostrada->array.table[i].stazione.dist);
    }
    fprintf(stdout, "demolita\n");
    return;
}

static unsigned int stampa_percorso(graph_t* autostrada, unsigned int* p, unsigned int src, unsigned int k){
    if(k == src) return src;
    fprintf(stderr, "%d %d\n", src, k);
    // fprintf(stderr, "%d ", autostrada->array.table[p[stampa_percorso(autostrada, p, src, p[k])]].stazione.dist);
    fprintf(stdout, "%u ", stampa_percorso(autostrada, p, src, autostrada->array.table[p[hash_search(autostrada->array, k)]].stazione.dist));
}

static void pianifica_percorso(graph_t* autostrada, unsigned short int from, unsigned short int to){
    unsigned int* p;
    p = bellman_ford(autostrada, from);
    fprintf(stderr, "%d %d\n", from, to);
    // for(int i=0; i<autostrada->array.m; i++){
    //     fprintf(stderr, "%u\n", p[i]);
    // }
    stampa_percorso(autostrada, p, from, to);
    return;
}

static void parse_and_execute(graph_t* autostrada){
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
    }
}

static void debug_graph(graph_t* autostrada, FILE* fp){
    struct int_list_node* x;
    
    fprintf(fp, "Graph Vertices: %d,\nHash table size: %d\n", autostrada->v, autostrada->array.m);
    for(int i=0; i<autostrada->array.m; i++){
        fprintf(fp, "%3i: %4d -> ", i, autostrada->array.table[i].stazione.dist);
        if(autostrada->array.table[i].head == NULL)
            fprintf(fp, "%p\n", autostrada->array.table[i].head);
        else{ 
            for(x = autostrada->array.table[i].head; x != NULL; x = x->next)
                fprintf(fp, "%d -> ", x->key);
            fprintf(fp, "%p\n", x);   
        }
    }
}

int main(int argc, char** argv){
    graph_t* autostrada;
    int index;

    autostrada = alloc_graph(HASH_M);

    parse_and_execute(autostrada);
    
    debug_graph(autostrada, stderr);

    // for(int i=0; i<autostrada->array.m; i++){
    //     if(autostrada->array.table[i].stazione.dist != NIL){
    //         index = hash_search(autostrada->array, autostrada->array.table[i].stazione.dist);
    //         fprintf(stderr, "t[%d] = %d\n", index, autostrada->array.table[index].stazione.dist);
    //     }
    //}

    return 0;
}
