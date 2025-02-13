#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND 18
#define NIL         -1
#define RED         0
#define WHITE       0
#define BLACK       1
#define GRAY        2
#define MAX_CARS    512
#define INFTY       (unsigned int)0xFFFFFFFF
#define MAX_QUEUE   64000

struct heap{
    int a[MAX_CARS];
    unsigned char max_heapified;
    size_t size;
};

struct bst_node{
    unsigned int key;
    struct heap cars;
    struct bst_node* parent;
    struct bst_node* left;
    struct bst_node* right;
    struct bst_node* prev;
    unsigned int steps;
    unsigned char color;
    unsigned char rb;
};

typedef struct{
    struct bst_node* root;
    struct bst_node* nil;
    size_t size;
} bst_t;

typedef struct queue {
    struct bst_node* q[MAX_QUEUE];
    unsigned short int tail;
    unsigned short int head;
    size_t length;
} queue_t;

static void left_rotate(bst_t* t, struct bst_node* x){
    struct bst_node* y;

    y = x->right;
    x->right = y->left;
    if(y->left != t->nil)
        y->left->parent = x;
    y->parent = x->parent;
    if(x->parent == t->nil)
        t->root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

static void right_rotate(bst_t* t, struct bst_node* x){
    struct bst_node* y;

    y = x->left;
    x->left = y->right;
    if(y->right != t->nil)
        y->right->parent = x;
    y->parent = x->parent;
    if(x->parent == t->nil)
        t->root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->right = x;
    x->parent = y;
}

static struct bst_node* tree_minimum(bst_t* t, struct bst_node* x){
    while (x->left != t->nil)
        x = x->left;

    return x;
}

static struct bst_node* tree_successor(bst_t* t, struct bst_node* x){
    struct bst_node* y;

    if(x->right != t->nil)
        return tree_minimum(t, x->right);
    y = x->parent;
    while(y!=t->nil && x==y->right){
        x = y;
        y = y->parent;
    }

    return y;
}

static void rb_insert_fixup(bst_t* t, struct bst_node* z){
    struct bst_node *x;
    struct bst_node *y;

    if(z == t->root)
        t->root->rb = BLACK;
    else{
        x = z->parent;
        if(x->rb == RED){
            if(x == x->parent->left){
                y = x->parent->right;
                if(y->rb == RED){
                    x->rb = BLACK;
                    y->rb = BLACK;
                    x->parent->rb = RED;
                    rb_insert_fixup(t, x->parent);
                } else {
                    if(z == x->right) {
                        z = x;
                        left_rotate(t, z);
                        x = z->parent;
                    }
                    x->rb = BLACK;
                    x->parent->rb = RED;
                    right_rotate(t, x->parent);
                }
            } else {
                y = x->parent->left;
                if(y->rb == RED){
                    x->rb = BLACK;
                    y->rb = BLACK;
                    x->parent->rb = RED;
                    rb_insert_fixup(t, x->parent);
                } else {
                    if(z == x->left) {
                        z = x;
                        right_rotate(t, z);
                        x = z->parent;
                    }
                    x->rb = BLACK;
                    x->parent->rb = RED;
                    left_rotate(t, x->parent);
                }
            }
        }
    }
}

static void rb_delete_fixup(bst_t* t, struct bst_node* x){
    struct bst_node* w;
    
    if(x->rb == RED || x->parent == t->nil){
        x->rb = BLACK;
    } else if (x == x->parent->left){
        w = x->parent->right;
        if(w->rb == RED){
            w->rb = BLACK;
            x->parent->rb = RED;
            left_rotate(t, x->parent);
            w = x->parent->right;
        }
        if(w->left->rb == BLACK && w->right->rb == BLACK){
            w->rb = RED;
            rb_delete_fixup(t, x->parent);
        }
        else {
            if(w->right->rb == BLACK){
                w->left->rb = BLACK;
                w->rb = RED;
                right_rotate(t, w);
                w = x->parent->right;
            }
            w->rb = x->parent->rb;
            x->parent->rb = BLACK;
            w->right->rb = BLACK;
            left_rotate(t, x->parent);
        }
    } else {
        w = x->parent->left;
        if(w->rb == RED){
            w->rb = BLACK;
            x->parent->rb = RED;
            right_rotate(t, x->parent);
            w = x->parent->left;
        }
        if(w->right->rb == BLACK && w->left->rb == BLACK){
            w->rb = RED;
            rb_delete_fixup(t, x->parent);
        }
        else {
            if(w->left->rb == BLACK){
                w->right->rb = BLACK;
                w->rb = RED;
                left_rotate(t, w);
                w = x->parent->left;
            }
            w->rb = x->parent->rb;
            x->parent->rb = BLACK;
            w->left->rb = BLACK;
            right_rotate(t, x->parent);
        }
    }
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
    z->prev = NULL;

    y = t->nil;
    x = t->root;

    while(x!=t->nil){
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

    if(y == t->nil){
        t->root = z;
    } else if(z->key < y->key){
        y->left = z;
    } else if(z->key == y->key){
        free(z);
        return NULL;
    } else {
        y->right = z;
    }
    
    z->left = t->nil;
    z->right = t->nil;
    z->rb = RED;

    (t->size)++;

    rb_insert_fixup(t, z);
       
    return z;
}

static void tree_delete(bst_t* t, struct bst_node* z){
    struct bst_node* y;
    struct bst_node* x;

    if(z->left == t->nil || z->right == t->nil)
        y = z;
    else
        y = tree_successor(t, z);

    if(y->left != t->nil){
        x = y->left;
    } else {
        x = y->right;
    }
   
    x->parent = y->parent;
    
    if(y->parent == t->nil){
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

    if(y->rb == BLACK){
        rb_delete_fixup(t, x);
    }


    free(y);
}

static struct bst_node* tree_search_util(struct bst_node* x, int n, struct bst_node* nil){
    if(x == nil || x->key == n){
        return x;
    }

    return tree_search_util((struct bst_node*)((unsigned long int)(x->right) * (n >= x->key) + (unsigned long int)(x->left) * (n < x->key)), n, nil);
}

static struct bst_node* tree_search(bst_t* t, int n){
    return tree_search_util(t->root, n, t->nil);
}

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
    
    return 0;
} 

static void enqueue(queue_t* queue, struct bst_node* x){
    if((queue->tail == queue->length && queue->head == 0) || queue->head == queue->tail + 1){
        return;
    }

    queue->q[queue->tail] = x;

    queue->tail = (queue->tail != queue->length) * (queue->tail + 1);
}

static struct bst_node* dequeue(queue_t* queue){
    struct bst_node* x;
    
    if(queue->head == queue->tail)
        return NULL;
    
    x = queue->q[queue->head];
    queue->q[queue->head] = NULL;

    queue->head = (queue->head != queue->length) * (queue->head + 1);
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
        heap_insert(&(s->cars), n);
    }
    
    fprintf(stdout, "aggiunta\n");
}

static void aggiungi_auto(bst_t* autostrada, int stazione, int autonomia){
    struct bst_node* s;
    s = tree_search(autostrada, stazione);
    if(s == autostrada->nil){
        fprintf(stdout, "non aggiunta\n");
        return;
    }

    heap_insert(&(s->cars), autonomia);

    fprintf(stdout, "aggiunta\n");
}


static void demolisci_stazione(bst_t* autostrada, int stazione){
    struct bst_node* s;
    s = tree_search(autostrada, stazione);
    if(s == autostrada->nil){
        fprintf(stdout, "non demolita\n");
        return;
    }
    
    tree_delete(autostrada, s);
    
    fprintf(stdout, "demolita\n");
}

static void rottama_auto(bst_t* autostrada, int stazione, int autonomia){
    struct bst_node* s;

    s = tree_search(autostrada, stazione);
    if(s == autostrada->nil){
        fprintf(stdout, "non rottamata\n");
        return;
    }

    if(heap_delete(&(s->cars), autonomia) != 0){
        fprintf(stdout, "non rottamata\n");
        return;
    }
        
    fprintf(stdout, "rottamata\n");
}

static void reset(struct bst_node* x, struct bst_node* nil){
    if(x == nil) return;
    x->color = WHITE;
    x->steps = INFTY;
    x->prev = NULL;
    reset(x->right, nil);
    reset(x->left,  nil);
}

static void find_adjacents_f(struct bst_node* x, struct bst_node* origin, queue_t* q, struct bst_node* nil){
    int dist = x->key - origin->key;
    if((x != origin && (dist > 0) && get_max_car(origin) >= dist) && ((unsigned int)x->steps > (unsigned int)(origin->steps))){
        x->steps = origin->steps + 1;
        if(x->color == WHITE){
            enqueue(q, x);
        }
        if(x->prev==NULL || (x->prev!=NULL && origin->key < x->prev->key)){
            x->prev = origin;
        }
        x->color = GRAY;
    }
    if(x->right!=nil && get_max_car(origin) > dist)
        find_adjacents_f(x->right, origin, q, nil);
    if(x->left!=nil && dist > 0) 
        find_adjacents_f(x->left, origin, q, nil);
}

static void find_adjacents_b(struct bst_node* x, struct bst_node* origin, queue_t* q, struct bst_node* nil){
    int dist = origin->key - x->key;
    if((x != origin && (dist > 0) && get_max_car(origin) >= dist) && ((unsigned int)x->steps > (unsigned int)(origin->steps))){
        x->steps = origin->steps + 1;
        if(x->color == WHITE){
            enqueue(q, x);
        }
        if(x->prev==NULL || (x->prev!=NULL && origin->key < x->prev->key)){
            x->prev = origin;
        }
        x->color = GRAY;
    }
    if(x->left!=nil && get_max_car(origin) > dist)
        find_adjacents_b(x->left, origin, q, nil);
    if(x->right!=nil && dist > 0)
        find_adjacents_b(x->right, origin, q, nil);
}

static void print_path(struct bst_node* x){
    if(x == NULL) return;
    print_path(x->prev);
    fprintf(stdout, x->color == 3 ? "%u\n" : "%u ", x->key);
}

static int find_best_path(bst_t* t, struct bst_node* from, struct bst_node* to, queue_t* q){
    struct bst_node* u;
    int break_loop = 0;
    
    u = NULL;
    
    reset(t->root, t->nil);

    from->color = GRAY;
    from->steps = 0;

    q->head = 0;
    q->tail = 0;
    q->length = t->size;

    enqueue(q, from);
    while(!break_loop && q->head != q->tail){
        u = dequeue(q);
        if(u == to){
            break_loop++;
        } else {
            if(!u->cars.max_heapified){
                build_max_heap(&(u->cars));
            }
            to->key > from->key ? find_adjacents_f(t->root, u, q, t->nil) : find_adjacents_b(t->root, u, q, t->nil);
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
    // q.q = NULL;

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

int main(int argc, char** argv) {
    bst_t autostrada;
    struct bst_node nil;

    autostrada.nil = &nil;
    autostrada.root = &nil;
    autostrada.nil->parent = autostrada.root;
    autostrada.nil->key = NIL;
    autostrada.nil->cars.size = 0;
    autostrada.nil->left = NULL;
    autostrada.nil->right = NULL;
    autostrada.nil->rb = BLACK;

    autostrada.size = 0;

    parse_and_execute(&autostrada);
    
    // tree_destroy(&autostrada);
    
    return 0;
}
