#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND 18
#define NIL         -1
#define RED         0
#define BLACK       1
#define MAX_CARS    512

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
    int color;
};

typedef struct{
    struct bst_node* root;
    struct bst_node* nil;
} bst_t;

struct node{
    int key;
    struct node* next;
};  

typedef struct {
    struct node* head;
} list_t;

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
    t->nil = malloc(sizeof(struct bst_node));
    t->root = t->nil;
    t->nil->parent = t->root;
    t->nil->key = NIL;
    memset(t->nil->cars.a, 0, sizeof(int) * MAX_CARS);
    t->nil->cars.size = 0;
    t->nil->left = NULL;
    t->nil->right = NULL;
    t->nil->color = BLACK;

    return t;
}

static void left_rotate(bst_t* t, struct bst_node* x){
    struct bst_node* y;

    y = x->right;
    x->right = y->left;
    if(y->left != t->nil)
        y->left->parent = x;
    y->parent =x->parent;
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
    y->parent =x->parent;
    if(x->parent == t->nil)
        t->root = y;
    else if(x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

static struct bst_node* tree_minimum(bst_t* t, struct bst_node* x){
    while (x->left != t->nil)
        x = x->left;

    return x;
}

// static struct bst_node* tree_maximum(bst_t* t, struct bst_node* x){
//     while (x->right != t->nil)
//         x = x->right;
// 
//     return x;
// }

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

// static struct bst_node* tree_predecessor(bst_t* t, struct bst_node* x){
//     struct bst_node* y;
// 
//     if(x->left != t->nil)
//         return tree_maximum(t, x->left);
//     y = x->parent;
//     while(y!=t->nil && x==y->left){
//         x = y;
//         y = y->parent;
//     }
// 
//     return y;
// }

static int rb_insert_fixup(bst_t* t, struct bst_node* z){
    struct bst_node *x;
    struct bst_node *y;

    if(z == t->root)
        t->root->color = BLACK;
    else{
        x = z->parent;
        if(x->color == RED){
            if(x == x->parent->left){
                y = x->parent->right;
                if(y->color == RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rb_insert_fixup(t, x->parent);
                } else {
                    if(z == x->right) {
                        z = x;
                        left_rotate(t, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    right_rotate(t, x->parent);
                }
            } else {
                y = x->parent->left;
                if(y->color == RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rb_insert_fixup(t, x->parent);
                } else {
                    if(z == x->left) {
                        z = x;
                        right_rotate(t, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    left_rotate(t, x->parent);
                }
            }
        }
    }

    return 0;
}

static void rb_delete_fixup(bst_t* t, struct bst_node* x){
    struct bst_node* w;

    if(x->color == RED || x->parent == t->nil)
        x->color = BLACK;
    else if (x == x->parent->left){
        w = x->parent->right;
        if(w->color == RED){
            w->color = BLACK;
            x->parent->color = RED;
            left_rotate(t, x->parent);
            w = x->parent->right;
        }
        if(w->left->color == BLACK && w->right->color == BLACK){
            w->color = RED;
            rb_delete_fixup(t, x->parent);
        }
        else {
            if(w->right->color == BLACK){
                w->left->color = BLACK;
                w->color = RED;
                right_rotate(t, w);
                w = x->parent->right;
            }
            w->color = x->parent->color;
            x->parent->color = BLACK;
            x->right->color = BLACK;
            left_rotate(t, x->parent);
        }
    } else {
        w = x->parent->left;
        if(w->color == RED){
            w->color = BLACK;
            x->parent->color = RED;
            right_rotate(t, x->parent);
            w = x->parent->left;
        }
        if(w->right->color == BLACK && w->left->color == BLACK){
            w->color = RED;
            rb_delete_fixup(t, x->parent);
        }
        else {
            if(w->left->color == BLACK){
                w->right->color = BLACK;
                w->color = RED;
                left_rotate(t, w);
                w = x->parent->left;
            }
            w->color = x->parent->color;
            x->parent->color = BLACK;
            x->left->color = BLACK;
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

    y = t->nil;
    x = t->root;

    while(x!=t->nil){
        y = x;
        (z->key < x->key) ? (x = x->left) : (x = x->right);
    }
    z->parent = y;

    if(y == t->nil){
        t->root = z;
    } else if(z->key < y->key){
        y->left = z;
    } else if(z->key == y->key){
        return t->nil;
    } else {
        y->right = z;
    }

    z->left = t->nil;
    z->right = t->nil;
    z->color = RED;
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

    (y->left != t->nil) ? (x = y->left) : (x = y->right);
    
    x->parent = y->parent;
    if(y->parent == t->nil)
        t->root = x;
    else if(y == y->parent->left)
        y->parent->left = x;
    else 
        y->parent->right = x;

    if(y != z)
        z->key = y->key;

    if(y->color == BLACK)
        rb_delete_fixup(t, x);

    free(z);
    // return y;
}

static struct bst_node* tree_search_util(bst_t* t, struct bst_node* x, int n){
    if(x == t->nil || x->key == n)
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

static void aggiungi_stazione(bst_t* autostrada, int dist, int num){
    struct bst_node* s;
    int n;

    s = tree_insert(autostrada, dist);
    if(s == autostrada->nil){
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
    
    // fprintf(stderr, "%d\n", stazione);
    // for(int i=0; i<s->cars.size; i++)
    //     fprintf(stderr, "%d", s->cars.a[i]);

    if(heap_delete(&(s->cars), autonomia) != 0){
        fprintf(stdout, "non rottamata\n");
        return;
    }

    fprintf(stdout, "rottamata\n");
}

static struct bst_node* find_backwards(bst_t* t, struct bst_node* from, struct bst_node* to, list_t* l){
    struct bst_node* x;
    struct bst_node* y;

    x = to;

    while(abs(from->key - x->key) > from->cars.a[0] && x != t->nil){
        fprintf(stderr, "%d -> %d: dist=%d, autonomia=%d\n", from->key, x->key, abs(from->key - x->key), from->cars.a[0]);
        x = tree_successor(t, x);
        if(x->key == from->key)
            x = t->nil;
    }
    fprintf(stderr, "%d -> %d: dist=%d, autonomia=%d\n", from->key, x->key, abs(from->key - x->key), from->cars.a[0]);
    
    if(x != from && x != t->nil){
        // fprintf(stdout, "%d ", from->key);
        y = find_backwards(t, x, to, l);
        if(y == t->nil)
            return y;
        list_insert(l, from->key);
    }

    return x;
}

// static struct bst_node* find_backwards(bst_t* t, struct bst_node* from, struct bst_node* to, list_t* l){
//     struct bst_node* x;
//     struct bst_node* y;
// 
//     x = tree_successor(t, to);
// 
//     while(abs(to->key - x->key) > x->cars.a[0] && x != t->nil){
//         fprintf(stderr, "%5d -> %5d:\tdist=%5d,\tautonomia=%5d\n", from->key, x->key, abs(from->key - x->key), from->cars.a[0]);
//         x = tree_successor(t, x);
//         if(x == from)
//             x = t->nil;
//     }
//     fprintf(stderr, "%5d -> %5d:\tdist=%5d,\tautonomia=%5d\n", from->key, x->key, abs(from->key - x->key), from->cars.a[0]);
//     
//     fprintf(stderr, "x=%5d\n", x->key);
//     if(x != from && x != t->nil){
//         y = find_backwards(t, from, x, l);
//         if(y == t->nil)
//             return y;
//         fprintf(stdout, "%d ", y->key);
//         // list_insert(l, y->key);
//     }
// 
//     return x;
// }

static struct bst_node* find_forward(bst_t* t, struct bst_node* from, struct bst_node* to){
    struct bst_node* x;
    struct bst_node* y;
    
    x = from;
    
    while(abs(to->key - x->key) > x->cars.a[0] && x != t->nil){
        fprintf(stderr, "%5d -> %5d:\tdist=%5d,\tautonomia=%5d\n", x->key, to->key, abs(to->key - x->key), x->cars.a[0]);
        x = tree_successor(t, x);
        if(x == to)
            x = t->nil;
    }
    fprintf(stderr, "%5d -> %5d:\tdist=%5d,\tautonomia=%5d\n", x->key, to->key, abs(to->key - x->key), x->cars.a[0]);
    
    fprintf(stderr, "x=%5d\n", x->key);
    if(x != from && x != t->nil){
        y = find_forward(t, from, x);
        if(y == t->nil)
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
    if(start == autostrada->nil || end == autostrada->nil){
        fprintf(stdout, "nessun percorso\n");
        return;
    }
   
    x = autostrada->nil;
    if(to > from){
        x = find_forward(autostrada, start, end);
        if(x==autostrada->nil){
            fprintf(stdout, "nessun percorso\n");
            return;
        }
        fprintf(stdout, "%d %d\n", x->key, to);
    } else {
        list_t l;
        l.head = NULL;
        x = find_backwards(autostrada, start, end, &l);
        if(x==autostrada->nil){
            fprintf(stdout, "nessun percorso\n");
            return;
        }
        for(struct node* tmp=l.head; tmp!=NULL; tmp=tmp->next)
            fprintf(stdout, "%d ", tmp->key);
        fprintf(stdout, "%d\n", to);
        list_destroy(&l);
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
    if(node == t->nil) return;

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

    debug_tree(autostrada);
    
    return 0;
}
