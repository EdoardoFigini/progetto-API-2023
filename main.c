#include <stdio.h>
#include <stdlib.h>

#define HASH_M 8
#define HASH_A 6.179952383  //1/sezione aurea
//(double)(1.14e20 / powf(2f, sizeof(unsigned long)))
#define HAS_TABLE_SIZE 512

double decimal(double n){
    return n - (int)n;
}

int hash(int k){
    return (int)(HASH_M * decimal(k * HASH_A));  
}

int hash_insert(int* a, int k){
    int i = 0;
    while(i<HAS_TABLE_SIZE){
        puts("s");
        i++;
    }

    return 0;
}

int main(int argc, char** argv){
    fprintf(stderr, "%d", hash(38));
    puts("asacdsv");

    return 0;
}
