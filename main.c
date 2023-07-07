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
    size_t heapsize;
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

static void debug_tree(bst_t*);

static bst_t* init_tree(){
    bst_t* t;

    t=(bst_t*)malloc(sizeof(bst_t));
    t->nil = malloc(sizeof(struct bst_node));
    t->root = t->nil;
    t->nil->parent = t->root;
    t->nil->key = NIL;
    memset(t->nil->cars.a, 0, sizeof(int) * MAX_CARS);
    t->nil->cars.heapsize = 0;
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

static struct bst_node* tree_insert(bst_t* t, int n){
    struct bst_node* x;
    struct bst_node* y;
    struct bst_node* z;

    z = (struct bst_node*)malloc(sizeof(struct bst_node));
    z->key = n;
    memset(z->cars.a, 0, sizeof(int) * MAX_CARS);
    z->cars.heapsize = 0;

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

static struct bst_node* tree_search(bst_t* t, struct bst_node* x, int n){
    if(x == t->nil || x->key == n)
        return x;
    if(n < x->key) 
        return tree_search(t, x->left, n);
    else
        return tree_search(t, x->right, n);
}

static void max_heapify(struct heap* h, int i){
    int l = 2*i;
    int r = l + 1;
    int max;
    int tmp;

    (l <= h->heapsize && h->a[l] > h->a[i]) ? (max = l) : (max = i);
    if(r <= h->heapsize && h->a[r] > h->a[max])
        max = r;
    if(max != i){
        tmp = h->a[i];
        h->a[i] = h->a[max];
        h->a[max] = tmp;
        max_heapify(h, max);
    }
}

static void heap_insert(struct heap* h, int k){
    h->a[(h->heapsize)++] = k;
    for(int i = (h->heapsize +1)/ 2; i>=0; i--)
        max_heapify(h, i);
}; 

static void aggiungi_stazione(bst_t* autostrada, int dist, int num){
    struct bst_node* s;
    int n;

    s = tree_insert(autostrada, dist);
    if(s == autostrada->nil)
        fprintf(stdout, "non aggiunta\n");

    for(int i=0; i<num; i++){
        scanf(" %d", &n);
        heap_insert(&(s->cars), n);
    }


    fprintf(stdout, "aggiunta\n");
}

static void aggiungi_auto(bst_t* autostrada, int stazione, int autonomia){
    struct bst_node* s;
    s = tree_search(autostrada, autostrada->root, stazione);
    if(s == autostrada->nil){
        fprintf(stdout, "non aggiunta\n");
        return;
    }

    heap_insert(&(s->cars), autonomia);

    fprintf(stdout, "aggiunta\n");
}


static void demolisci_stazione(bst_t* autostrada, int stazione){
    fprintf(stderr, "Not implemented\n");
}

static void rottama_auto(bst_t* autostrada, int stazione, int autonomia){
    fprintf(stderr, "Not implemented\n");
}

static void pianifica_percorso(bst_t* autostrada, int from, int to){
    fprintf(stderr, "Not implemented\n");
}

static void parse_and_execute(bst_t* autostrada){
    char command[MAX_COMMAND + 1];
    int arg1, arg2;

    while(fscanf(stdin, " %s", command) != EOF){
        switch (command[0])
        {
            case 'd':                
                scanf("%d", &arg1);
                demolisci_stazione(autostrada, arg1);
                break;
            case 'r':
                scanf("%d %d", &arg1, &arg2);
                rottama_auto(autostrada, arg1, arg2);
                break;
            case 'p':
                scanf("%d %d", &arg1, &arg2);
                pianifica_percorso(autostrada, arg1, arg2);
                break;
            case 'a':
                if (command[9]=='s'){
                    scanf("%d %d", &arg1, &arg2);
                    aggiungi_stazione(autostrada, arg1, arg2);

                } else {
                    scanf("%d %d", &arg1, &arg2);
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
    
    // fprintf(stderr, "%d\n", autostrada->root->cars.heapsize);
    // for(int i=0; i<autostrada->root->cars.heapsize; i++)
    //     fprintf(stderr, "%d ", autostrada->root->cars.a[i]);
    // return 0;

    debug_tree(autostrada);
    
    return 0;
}
