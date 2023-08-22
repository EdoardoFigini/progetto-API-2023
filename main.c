#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND 18
#define NIL         -1
#define WHITE       0
#define BLACK       1
#define GRAY        2
#define MAX_CARS    512
#define INFTY       (unsigned int)0xFFFFFFFF

struct heap{
    int a[MAX_CARS];
    int max_heapified;
    size_t size;
};

struct bst_node{
    int key;
    struct heap cars;
    struct bst_node* parent;
    struct bst_node* left;
    struct bst_node* right;
    struct bst_node* next;
    unsigned int steps;
    unsigned short int color;
};

typedef struct{
    struct bst_node* root;
    size_t size;
} bst_t;

typedef struct queue {
    struct bst_node** q;
    int tail;
    int head;
    size_t length;
} queue_t;


// static void debug_tree(bst_t*);

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
    z->cars.max_heapified = 0;
    z->color = WHITE;
    z->steps = INFTY;
    z->steps = INFTY;
    z->next = NULL;

    y = NULL;
    x = t->root;

    while(x!=NULL){
        y = x;
        if(z->key < x->key){
            x = x->left;
        } else if(z->key == x->key){
            free(z);
            return NULL;
        } else {
            x = x->right;
        }
    }
    z->parent = y;

    if(y == NULL){
        t->root = z;
    } else if(z->key < y->key){
        y->left = z;
    } else if(z->key == y->key){
        free(z);
        return NULL;
    } else {
        y->right = z;
    }
    
    z->left = NULL;
    z->right = NULL;

    (t->size)++;
       
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

    if(y != z){
        z->key = y->key;
        memcpy(&(z->cars), &(y->cars), sizeof(struct heap));
    }

    free(y);
}

static struct bst_node* tree_search_util(struct bst_node* x, int n){
    if(x == NULL || x->key == n)
        return x;
    if(n < x->key){ 
        return tree_search_util(x->left, n);
    } else {
        return tree_search_util(x->right, n);
    }
}

static struct bst_node* tree_search(bst_t* t, int n){
    return tree_search_util(t->root, n);
}

// static void tree_destroy_util(struct bst_node* x){
//     if(x!=NULL){
//         tree_destroy_util(x->right);
//         tree_destroy_util(x->left);
//         free(x);
//     }
// }
// 
// static void tree_destroy(bst_t* t){
//     tree_destroy_util(t->root);
//     free(t);
// }

static void max_heapify(struct heap* h, int i){
    int l = 2*i;
    int r = 2*i + 1;
    int max = NIL;
    int tmp = NIL;

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

static void build_max_heap(struct heap* h){
    for(int i=h->size / 2; i>=0; i--){
        max_heapify(h, i);
    }

    h->max_heapified = 1;
}

static void heap_insert(struct heap* h, int k){
    h->a[(h->size)++] = k;
    h->max_heapified = 0;
    // build_max_heap(h);
}

static int heap_delete(struct heap* h, int k){
    int i;

    for(i=0; i<h->size && h->a[i] != k; i++);
    if(i >= h->size)
        return -1;

    h->a[i] = h->a[h->size - 1];
    h->a[h->size - 1] = NIL;
    (h->size)--;
    h->max_heapified = 0;

    // build_max_heap(h);
    
    return 0;
} 

static void enqueue(queue_t* queue, struct bst_node* x){
    if((queue->tail == queue->length && queue->head == 0) || queue->head == queue->tail + 1){
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
    
    x = queue->q[queue->head];
    queue->q[queue->head] = NULL;
    if(queue->head == queue->length)
        queue->head = 0;
    else
        queue->head = queue->head + 1;
    return x;
}

static int get_max_car(struct bst_node* x){
    return *(x->cars.a);
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
        if(fscanf(stdin, " %d", &n)){};
        s->cars.a[(s->cars.size)++] = n;
    }

    // build_max_heap(h);

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

static void reset(struct bst_node* x){
    if(x != NULL){
        x->color = WHITE;
        x->steps = INFTY;
        x->next = NULL;
        reset(x->right);
        reset(x->left);
    }
}

static void find_adjacents(struct bst_node* x, struct bst_node* origin, queue_t* q, int dir){
    if(x != origin && (x->key > origin->key) == dir && get_max_car(origin) >= abs(x->key - origin->key)){
        if((unsigned int)x->steps > (unsigned int)(origin->steps)){
            x->steps = origin->steps + 1;
            if(x->color == WHITE){
                enqueue(q, x);
            }
            if(x->next==NULL || (x->next!=NULL && origin->key < x->next->key)){
                x->next = origin;
            }
            x->color = GRAY;
        }
    }
    if(x->left!=NULL && (dir || (!dir && get_max_car(origin) > origin->key - x->key)) && (!dir || (dir && origin->key < x->key))) 
        find_adjacents(x->left, origin, q, dir);
    if(x->right!=NULL && (!dir || (dir && get_max_car(origin) > x->key - origin->key)) && (dir || (!dir && origin->key > x->key)))
        find_adjacents(x->right, origin, q, dir);
}

static void print_path(struct bst_node* x){
    if(x != NULL){
        print_path(x->next);
        fprintf(stdout, x->color == 3 ? "%u\n" : "%u ", x->key);
    }
}

static int find_best_path(bst_t* t, struct bst_node* from, struct bst_node* to, queue_t* q){
    struct bst_node* u;
    int break_loop = 0;
    
    u = NULL;

    reset(t->root);

    from->color = GRAY;
    from->steps = 0;

    q->head = 0;
    q->tail = 0;
    if(q->length < t->size){
        if(q->q != NULL) free(q->q);
        q->length = t->size;
        q->q = malloc(sizeof(struct bst_node) * q->length);
    }
    memset(q->q, 0x0, sizeof(struct bst_node*) * q->length);

    enqueue(q, from);
    while(!break_loop && q->head != q->tail){
        u = dequeue(q);
        if(u == to){
            break_loop = 1;
        } else {
            if(!u->cars.max_heapified){
                build_max_heap(&(u->cars));
            }
            find_adjacents(t->root, u, q, to->key > from->key);
        }
        u->color = BLACK;
    }

    if(to->steps == INFTY){
        return -1;
    }
    
    to->color = 3;
    print_path(to);


    return 0;
}

static void pianifica_percorso(bst_t* autostrada, int from, int to, queue_t* q){
    struct bst_node* start;
    struct bst_node* end;
    
    start = tree_search(autostrada, from);
    end = tree_search(autostrada, to);
    if(start == NULL || end == NULL){
        fprintf(stdout, "nessun percorso\n");
        return;
    }
    
   
    if(find_best_path(autostrada, start, end, q) != 0){
       fprintf(stdout, "nessun percorso\n");
       return;
    }

}

static void parse_and_execute(bst_t* autostrada){
    char command[MAX_COMMAND + 1];
    int arg1, arg2;
    queue_t q;
    
    q.length = 0;
    q.q = NULL;

    while(fscanf(stdin, " %s", command) != EOF){
        switch (command[0]){
            case 'd':                
                if(fscanf(stdin, "%d", &arg1)){}
                demolisci_stazione(autostrada, arg1);
                break;
            case 'r':
                if(fscanf(stdin, "%d %d", &arg1, &arg2)){}
                rottama_auto(autostrada, arg1, arg2);
                break;
            case 'p':
                if(fscanf(stdin, "%d %d", &arg1, &arg2)){}
                pianifica_percorso(autostrada, arg1, arg2, &q);
                break;
            case 'a':
                if (command[9]=='s'){
                    if(fscanf(stdin, "%d %d", &arg1, &arg2)){}
                    aggiungi_stazione(autostrada, arg1, arg2);

                } else {
                    if(fscanf(stdin, "%d %d", &arg1, &arg2)){}
                    aggiungi_auto(autostrada, arg1, arg2);
                }
                break;
            default:
                break;
        }
    }

    // if(q.q != NULL)
    //     free(q.q);

}

// static void debug_tree_util(bst_t* t, struct bst_node* node, int space){
//     if(node == NULL) return;
// 
//     space += 5;
// 
//     debug_tree_util(t, node->right, space);
// 
//     fprintf(stderr, "\n");
//     for(int i=5; i<space; i++)
//         fprintf(stderr, " ");
//     fprintf(stderr, node->color == WHITE ? "%d\n" : "\033[31m%d\033[0m\n", node->key);
// 
//     debug_tree_util(t, node->left, space);
// }
// 
// static void debug_tree(bst_t* t){
//     fprintf(stderr, "size: %lu\n", t->size);
//     debug_tree_util(t, t->root, 0);
//     fprintf(stderr, "-----------------------------------------------------------------------\n");
// }

int main(int argc, char** argv) {
    bst_t* autostrada;

    autostrada = init_tree();

    parse_and_execute(autostrada);
    
    // debug_tree(autostrada);
    
    // tree_destroy(autostrada);
    
    return 0;
}
