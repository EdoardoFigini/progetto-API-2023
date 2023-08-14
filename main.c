#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND 18
#define NIL         -1
#define WHITE       0
#define BLACK       1
#define GRAY        2
#define MAX_CARS    512
#define INFTY       (int)0xFFFFFFFF

struct heap{
    int a[MAX_CARS];
    size_t size;
};

struct bst_node{
    int key;
    struct heap cars;
    struct bst_node* parent;
    struct bst_node* left;
    struct bst_node* right;
    struct bst_node* prev;
    int dist;
    int color;
};

typedef struct{
    struct bst_node* root;
    // struct bst_node* nil;
    size_t size;
} bst_t;

struct node{
    int key;
    struct node* next;
};  

typedef struct {
    struct node* head;
} list_t;

struct bfs_node {
    int key;
    int color;
    int dist;
};

typedef struct queue {
    struct bst_node** q;
    int tail;
    int head;
    size_t length;
} queue_t;


static void debug_tree(bst_t*);

static void list_insert(list_t* l, int n){
    struct node* x;

    x = (struct node*)malloc(sizeof(struct node));
    x->key = n;
    x->next = l->head;
    l->head = x;
}

static void list_destroy(list_t* l){
    struct node* x;

    while(l->head!=NULL){
        x = l->head;
        l->head = l->head->next;
        free(x);
    }
}

static bst_t* init_tree(){
    bst_t* t;

    t=(bst_t*)malloc(sizeof(bst_t));
    t->root = NULL;

    return t;
}

static struct bst_node* tree_minimum(struct bst_node* x){
    while (x->left != NULL)
        x = x->left;

    return x;
}

// static struct bst_node* tree_maximum(bst_t* t, struct bst_node* x){
//     while (x->right != NULL)
//         x = x->right;
// 
//     return x;
// }

static struct bst_node* tree_successor(struct bst_node* x){
    struct bst_node* y;

    if(x->right != NULL)
        return tree_minimum(x->right);
    y = x->parent;
    while(y!=NULL && x==y->right){
        x = y;
        y = y->parent;
    }

    return y;
}

static struct bst_node* tree_insert(bst_t* t, int n){
    struct bst_node* x;
    struct bst_node* y;
    struct bst_node* z;

    z = (struct bst_node*)malloc(sizeof(struct bst_node));
    z->key = n;
    memset(z->cars.a, NIL, sizeof(int) * MAX_CARS);
    z->cars.size = 0;

    y = NULL;
    x = t->root;

    while(x!=NULL){
        y = x;
        (z->key < x->key) ? (x = x->left) : (x = x->right);
    }
    z->parent = y;

    if(y == NULL){
        t->root = z;
    } else if(z->key < y->key){
        y->left = z;
    } else if(z->key == y->key){
        return NULL;
    } else {
        y->right = z;
    }
    
    z->left = NULL;
    z->right = NULL;
    z->color = WHITE;
    z->dist = INFTY;
    z->prev = NULL;

    t->size += 1;
       
    return z;
}

static void tree_delete(bst_t* t, struct bst_node* z){
    struct bst_node* y;
    struct bst_node* x;

    if(z->left == NULL || z->right == NULL)
        y = z;
    else
        y = tree_successor(z);

    if(y->left != NULL){
        x = y->left;
    } else {
        x = y->right;
    }
   
    if(x != NULL)
        x->parent = y->parent;
    
    if(y->parent == NULL){
        t->root = x;
    } else if(y == y->parent->left){
        y->parent->left = x;
    } else { 
        y->parent->right = x;
    }

    if(y != z)
        z->key = y->key;

    // if(y->color == BLACK)
    //     rb_delete_fixup(t, x);

    // struct bst_node* s = z->parent;
    // fprintf(stderr, "%d: %p\n", s->key, s);
    // fprintf(stderr, "\t%d: %p\n", s->left->key, s->left);
    // fprintf(stderr, "\t%d: %p\n", s->right->key, s->right);
    // debug_tree(t);

    free(z);
}

static struct bst_node* tree_search_util(bst_t* t, struct bst_node* x, int n){
    if(x == NULL || x->key == n)
        return x;
    if(n < x->key) 
        return tree_search_util(t, x->left, n);
    else
        return tree_search_util(t, x->right, n);
}

static struct bst_node* tree_search(bst_t* t, int n){
    return tree_search_util(t, t->root, n);
}

static void max_heapify(struct heap* h, int i){
    int l = 2*i;
    int r = l + 1;
    int max;
    int tmp;

    (l <= h->size && h->a[l] > h->a[i]) ? (max = l) : (max = i);
    if(r <= h->size && h->a[r] > h->a[max])
        max = r;
    if(max != i){
        tmp = h->a[i];
        h->a[i] = h->a[max];
        h->a[max] = tmp;
        max_heapify(h, max);
    }
}

static void heap_insert(struct heap* h, int k){
    h->a[(h->size)++] = k;
    for(int i = (h->size +1)/ 2; i>=0; i--)
        max_heapify(h, i);
}

static int heap_delete(struct heap* h, int k){
    int i;
    for(i=0; i<h->size && h->a[i] != k; i++);
    if(i == h->size)
        return 1;

    h->a[i] = NIL;
    (h->size)--;

    for(i = (h->size +1)/ 2; i>=0; i--)
        max_heapify(h, i);

    return 0;
} 

static void enqueue(queue_t* queue, struct bst_node* x){
    if(queue->tail == queue->length && queue->head == 0 || queue->head == queue->tail + 1){
        return;
    }

    queue->q[queue->tail] = x;
    if(queue->tail == queue->length)
        queue->tail = 0;
    else
        queue->tail = queue->tail + 1;
}

static struct bst_node* dequeue(queue_t* queue){
    struct bst_node* x;
    
    if(queue->head == queue->tail)
        return NULL;
    
    // for(int i=0; i<queue->length; i++)
    //     queue->q[i] == NULL ? fprintf(stderr, "%2d: NULL\n", i): fprintf(stderr, "%2d: %d\n", i, queue->q[i]->key);
    x = queue->q[queue->head];
    if(queue->head == queue->length)
        queue->head = 0;
    else
        queue->head = queue->head + 1;
    return x;
}

static int get_max_car(struct bst_node* x){
    return x->cars.a[0];
}

static void aggiungi_stazione(bst_t* autostrada, int dist, int num){
    struct bst_node* s;
    int n;

    s = tree_insert(autostrada, dist);
    if(s == NULL){
        fprintf(stdout, "non aggiunta\n");
        return;
    }

    for(int i=0; i<num; i++){
        if(scanf(" %d", &n)){};
        // heap_insert(&(s->cars), n);
        s->cars.a[(s->cars.size)++] = n;
    }
    for(int i = (s->cars.size + 1) / 2; i >= 0; i--)
        max_heapify(&(s->cars), i);


    fprintf(stdout, "aggiunta\n");
}

static void aggiungi_auto(bst_t* autostrada, int stazione, int autonomia){
    struct bst_node* s;
    s = tree_search(autostrada, stazione);
    if(s == NULL){
        fprintf(stdout, "non aggiunta\n");
        return;
    }

    heap_insert(&(s->cars), autonomia);

    fprintf(stdout, "aggiunta\n");
}


static void demolisci_stazione(bst_t* autostrada, int stazione){
    struct bst_node* s;
    s = tree_search(autostrada, stazione);
    if(s == NULL){
        fprintf(stdout, "non demolita\n");
        return;
    }
    
    // fprintf(stderr, "%d: %p\n", s->key, s);
    // fprintf(stderr, "\t%d: %p\n", s->left->key, s->left);
    // fprintf(stderr, "\t%d: %p\n", s->right->key, s->right);
    tree_delete(autostrada, s);

    fprintf(stdout, "demolita\n");
}

static void rottama_auto(bst_t* autostrada, int stazione, int autonomia){
    struct bst_node* s;

    s = tree_search(autostrada, stazione);
    if(s == NULL){
        fprintf(stdout, "non rottamata\n");
        return;
    }

    if(heap_delete(&(s->cars), autonomia) != 0){
        fprintf(stdout, "non rottamata\n");
        return;
    }

    fprintf(stdout, "rottamata\n");
}

static void set_white(struct bst_node* x){
    if(x != NULL){
        x->color = WHITE;
        set_white(x->right);
        set_white(x->left);
    }
}

static void find_dist(struct bst_node* x, struct bst_node* origin, queue_t* q, unsigned int* p){
    if(x != NULL){
        if(origin->key > x->key && get_max_car(origin) >= abs(x->key - origin->key)){
            fprintf(stderr, "%d -> %d,\tautonomia: %u,\tdistanza: %u\tcolor: %u\n", origin->key, x->key, get_max_car(origin), abs(x->key - origin->key), x->color);
            if(x->color == WHITE){
                x->color = GRAY;
                x->dist = origin->dist + 1;
                enqueue(q, x);
                // if(x->key < p[x->dist]){
                if(x->prev == NULL || (x->prev!=NULL && x->prev->key > origin->key)){
                    x->prev = origin;
                //     p[x->dist] = x->key;
                //     p[x->dist+1] = x->key;
                }
            }
        }
        find_dist(x->right, origin, q, p);
        find_dist(x->left, origin, q, p);
    }
}

static void stampa(struct bst_node* x){
    if(x->prev != NULL){
        stampa(x->prev);
    }
    fprintf(stdout, "%d ", x->key);
    fprintf(stderr, "%d ", x->key);
}

static int find_backwards(bst_t* t, struct bst_node* from, struct bst_node* to){
    struct bst_node* u;
    queue_t* q;
    unsigned int* percorso;
    int break_loop = 0;

    percorso = malloc(t->size * sizeof(int));
    memset(percorso, INFTY, sizeof(unsigned int) * t->size);
    percorso[0] = from->key;
    percorso[1] = from->key;
    
    u = NULL;

    set_white(t->root);

    from->color = GRAY;
    from->dist = 0;

    q = malloc(sizeof(queue_t));
    q->head = 0;
    q->tail = 0;
    q->length = t->size;
    q->q = malloc(sizeof(struct bst_node) * q->length);

    enqueue(q, from);
    while(!break_loop && q->head != q->tail){
        u = dequeue(q);
        fprintf(stderr, "%d\n", u->key);
        if(u == to){
            break_loop = 1;
        }
        find_dist(t->root, u, q, percorso);
        u->color = BLACK;
    }
    

    for(int i=0; i<=t->size; i++){
        fprintf(stderr, i!=t->size ? "%d " : "%d\n", percorso[i]);
    }
    
    if(to->dist == INFTY)
        return -1;
    
    stampa(to);
    
    // for(int i=0; i<=to->dist; i++){
    //     fprintf(stdout, i!=to->dist ? "%d " : "%d\n", percorso[i]);
    // }

    free(q->q);
    free(q);
    free(percorso);

    return 0;
}

static struct bst_node* find_forward(bst_t* t, struct bst_node* from, struct bst_node* to){
    struct bst_node* x;
    struct bst_node* y;
    
    x = from;
    
    while(x != NULL && abs(to->key - x->key) > get_max_car(x)){
        // fprintf(stderr, "%5d -> %5d:\tdist=%5d,\tautonomia=%5d\n", x->key, to->key, abs(to->key - x->key), get_max_car(x));
        x = tree_successor(x);
        if(x->key == to->key){
            x = NULL;
        }
    }
    // fprintf(stderr, "%5d -> %5d:\tdist=%5d,\tautonomia=%5d\n", x->key, to->key, abs(to->key - x->key), get_max_car(x));
    
    // fprintf(stderr, "x=%5d\n", x->key);
    if(x != from && x != NULL){
        y = find_forward(t, from, x);
        if(y == NULL)
            return y;
        fprintf(stdout, "%d ", y->key);
    }
    
    return x;
}

static void pianifica_percorso(bst_t* autostrada, int from, int to){
    struct bst_node* start;
    struct bst_node* end;
    struct bst_node* x;
    
    start = tree_search(autostrada, from);
    end = tree_search(autostrada, to);
    if(start == NULL || end == NULL){
        fprintf(stdout, "nessun percorso\n");
        return;
    }
   
    x = NULL;
    if(to > from){
        x = find_forward(autostrada, start, end);
        if(x==NULL){
            fprintf(stdout, "nessun percorso\n");
            return;
        }
        fprintf(stdout, "%d %d\n", x->key, to);
    } else {
        int res = find_backwards(autostrada, start, end);
        if(res!=0){
            fprintf(stdout, "nessun percorso\n");
            return;
        }
    }

}

static void parse_and_execute(bst_t* autostrada){
    char command[MAX_COMMAND + 1];
    int arg1, arg2;

    while(fscanf(stdin, " %s", command) != EOF){
        switch (command[0])
        {
            case 'd':                
                if(scanf("%d", &arg1)){}
                demolisci_stazione(autostrada, arg1);
                break;
            case 'r':
                if(scanf("%d %d", &arg1, &arg2)){}
                rottama_auto(autostrada, arg1, arg2);
                break;
            case 'p':
                if(scanf("%d %d", &arg1, &arg2)){}
                pianifica_percorso(autostrada, arg1, arg2);
                break;
            case 'a':
                if (command[9]=='s'){
                    if(scanf("%d %d", &arg1, &arg2)){}
                    aggiungi_stazione(autostrada, arg1, arg2);

                } else {
                    if(scanf("%d %d", &arg1, &arg2)){}
                    aggiungi_auto(autostrada, arg1, arg2);
                }
                break;
            default:
                break;
        }
    }
}

static void debug_tree_util(bst_t* t, struct bst_node* node, int space){
    if(node == NULL) return;

    space += 5;

    debug_tree_util(t, node->right, space);

    fprintf(stderr, "\n");
    for(int i=5; i<space; i++)
        fprintf(stderr, " ");
    fprintf(stderr, node->color == BLACK ? "%d\n" : "\033[31m%d\033[0m\n", node->key);

    debug_tree_util(t, node->left, space);
}

static void debug_tree(bst_t* t){
    debug_tree_util(t, t->root, 0);
}

int main(int argc, char** argv) {
    bst_t* autostrada;

    autostrada = init_tree();

    parse_and_execute(autostrada);
    
    // fprintf(stderr, "%d\n", autostrada->root->cars.size);
    // for(int i=0; i<autostrada->root->cars.size; i++)
    //     fprintf(stderr, "%d ", autostrada->root->cars.a[i]);
    // return 0;

    // debug_tree(autostrada);
    
    
    return 0;
}
