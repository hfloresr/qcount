#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define INITCAP     256
#define BUCKET_MAX  512
#define QSIZE       4


typedef struct {
    int size;
    int capacity;
    int *data;
} Set;

void init_set(Set *set)
{
    set->size = 0;
    set->capacity = INITCAP;
    set->data = malloc(sizeof(int) * set->capacity);
}

void grow_set(Set *set)
{
    set->capacity *= 2;
    set->data = realloc(set->data, sizeof(int) * set->capacity);
}

void add_data(Set *set, int value)
{
    if (set->size >= set->capacity)
        grow_set(set);

    set->data[set->size++] = value;
}


int get_data(Set *set, int idx)
{
    if (idx < set->size || idx >= 0)
        return set->data[idx];

    printf("Index %d out of bounds. Size is: %d\n", idx, set->size);
    return -88;
}

void copy_data(Set *a, Set *b)
{
    int i;
    if (a->size == b->size)
        for (i = 0; i < a->size; ++i)
            a->data[i] = b->data[i];
}

void array_to_set(Set *set, int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        add_data(set, arr[i]);
}

void join_sets(Set *a, Set *b)
{
    int i, x;
    for (i = 0; i < b->size; ++i) {
        x = get_data(b, i);
        add_data(a, x);
    }
}

void query_set(int query[], Set *set)
{
    int i;
    for (i = 0; i < QSIZE; ++i)
        query[i] = set->data[i];
}

void free_set(Set *set)
{
    free(set->data);
    set->size = set->capacity = 0;
}

void print_set(Set *set)
{
    int i;
    printf("{ ");
    for (i = 0; i < set->size; ++i)
        printf("%d, ", get_data(set, i));
    printf("}\n");
}


typedef struct {
    int size;
    Set list[BUCKET_MAX];
} Bucket;

void init_bucket_list(Bucket *b)
{
    int i;
    for (i = 0; i < BUCKET_MAX; ++i)
        init_set(&b->list[i]);
    b->size = 0;
}

void free_bucket(Bucket *b)
{
    int i;
    for (i = BUCKET_MAX-1; i >= 0 ; --i)
        free_set(&b->list[i]);
    b->size = 0;
}

int get_index_at(Bucket *b, int bucket_idx, int set_idx)
{
    return get_data(&b->list[bucket_idx], set_idx);
}

int add_array_to_bucket(Bucket *b, int arr[], int n)
{
    int is_full;

    if ((is_full = b->size >= BUCKET_MAX) == 1)
        printf("Bucket is full\n");
    else
        array_to_set(&b->list[b->size++], arr, n);
    return (is_full ? 0 : 1);
}

int add_set_to_bucket(Bucket *b, Set *s)
{
    int is_full;

    if ((is_full = b->size >= BUCKET_MAX) == 1)
        printf("Bucket is full\n");
    else
        b->list[b->size++] = *s;
    return (is_full ? 0 : 1);
}

void remove_from_bucket(Bucket *b, int idx)
{
    if (idx >= b->size || idx < 0) {
        printf("Index %d is out of bounds. Size is: %d\n", idx, b->size);
        return;
    }

    b->size--;
    if (idx < b->size && idx >= 0)
        copy_data(&b->list[idx], &b->list[b->size]);
}




/* Boyer-Moor majority vote algorithm */
int majority_element(int arr[], int sz)
{
    int i, cnt, maj;

    maj = 0;   /* majority index */
    cnt = 1;   /* counter */
    for (i = 1; i < sz; ++i) {
        if (arr[maj] == arr[i])
            ++cnt;
        else
            --i;
        if (cnt == 0) {
            maj = i;
            cnt = 1;
        }
    }
    return maj;
}


void init_indices(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        arr[i] = i+1;
}


void new_query(int query[], int indices[], int start)
{
    int i;
    for (i = 0; i < QSIZE; ++i)
        query[i] = indices[start+i];
}

int map_qcount(int qresult)
{
    return qresult == 0 ? 0 :
           qresult == 2 ? 1 :
           qresult == 4 ? 2 :
           qresult;
}

void print_bucket_sizes(Bucket B[], int sz)
{
    int i;
    printf("Size B(0,1,2) = (");
    for (i = 0; i < sz; ++i) {
        printf("%d, ", B[i].size);
    }
    printf(")\n");
}


void print_query(int query[])
{
    int i;
    printf("query: [ ");
    for (i = 0; i < QSIZE; ++i)
        printf("%d, ", query[i]);
    printf("]\n");
}


/* TODO: modifiy query for when n doesn't evenly divide into 4 queries */

int mysub(int n)
{
    int i,j, r1, r2, r3, r4, n_buckets;
    int qresult, qbin;

    /* indices of marbles */
    int indices[n];
    init_indices(indices, n);

    int query[QSIZE];           /* store current query */

    /* QCOUNT = 0, 2, 4 -> B[0], B[1], B[2] */
    n_buckets = 3;
    Bucket B[n_buckets];
    for (i = 0; i < n_buckets; ++i)
        init_bucket_list(&B[i]);

    for (i = 0; i < n; i+=4) {
        new_query(query, indices, i);
        qresult = QCOUNT(1, query);
        qbin = map_qcount(qresult);
        /* printf("qresult = %d\n", qresult); */
        if (qbin >= 0)
            add_array_to_bucket(&B[qbin], query, QSIZE);
        else {
            printf("i,n = %d,%d\t", i, n);
            print_query(query);
            break;
        }
    }

    Set black, white, even;
    init_set(&black);
    init_set(&white);
    init_set(&even);

    /* Seperate sets into black & white marbles */
    if (B[2].size >= 1) {
        join_sets(&black, &B[2].list[0]);
        remove_from_bucket(&B[2], 0);
    }

    /* flatten set of indices to two different sets (black and white) */
    while ((i = B[2].size-1) >= 0) {
        query_set(query, &black);
        query[3] = get_index_at(&B[2], i, 0);

        qresult = QCOUNT(1, query);
        if (qresult == 4) {
            join_sets(&black, &B[2].list[i]);
            remove_from_bucket(&B[2], i);
        } else if (qresult == 2) {
            join_sets(&white, &B[2].list[i]);
            remove_from_bucket(&B[2], i);
        }
    }

    /* trim set of indices that resulted in a 2 from QCOUNT */
    int b1_sz = B[1].size;
    for (i = 1; i < b1_sz; i+=2) {
        query_set(query, &B[1].list[i-1]);
        query[3] = get_index_at(&B[1], i, 0);

        qresult = QCOUNT(1, query);
        printf("qresult: %d\n", qresult);
        if (qresult == 4) {
            query[3] = get_data(&black, 0);
            qresult = QCOUNT(1, query);

            query[3] = get_index_at(&B[1], i, 0);
            if (qresult == 4)
                array_to_set(&black, query, QSIZE);
            else 
                array_to_set(&white, query, QSIZE);
        } else if (qresult == 0) {
            array_to_set(&even, query, QSIZE);
        } else {
            break;
        }
    }

    printf("Size B[0,1,2] :  (%d, %d, %d)\n", B[0].size, B[1].size, B[2].size);

    printf("(black set): size: %d\tvalues: ", black.size);
    print_set(&black);

    printf("(white set): size: %d\tvalues: ", white.size);
    print_set(&white);

    printf("(even set): size: %d\tvalues: ", even.size);
    print_set(&even);







    /*
    srand(time(0));

    int bsz, wsz, esz;
    Set blo[256], wlo[256], elo[256];
    for (i = 0; i < 256; ++i) {
        init_set(&blo[i]);
        init_set(&wlo[i]);
        init_set(&elo[i]);
    }
    bsz = wsz = esz = 0;

    printf("Initial B[1] size: %d\n", B[1].size);

    int tmp[3];
    int k;
    while ((i = B[1].size-1) > 1) {
        query_set(query, &black);

        r1 = rand() % B[1].size;
        r2 = rand() % QSIZE;
        query[2] = get_index_at(&B[1], r1, r2);

        while((r3 = rand() % (B[1].size - 1)) == r1)
            ;
        r4 = rand() % QSIZE;
        query[3] = get_index_at(&B[1], r3, r4);

        qresult = QCOUNT(1, query);
        if (qresult == 4) {
            add_data(&black, query[2]);
            add_data(&black, query[3]);

            k = 0;
            for (j = 0; j < 4; ++j) {
                if (j != r2) {
                    tmp[k++] = get_index_at(&B[1], r1, j);
                }
            }
            array_to_set(&blo[bsz++], tmp, 3);

            k = 0;
            for (j = 0; j < 4; ++j) {
                if (j != r4) {
                    tmp[k++] = get_index_at(&B[1], r3, j);
                }
            }
            array_to_set(&blo[bsz++], tmp, 3);

        } else if (qresult == 2) {
            add_data(&even_pairs, query[2]);
            add_data(&even_pairs, query[3]);

            k = 0;
            for (j = 0; j < 4; ++j) {
                if (j != r2) {
                    tmp[k++] = get_index_at(&B[1], r1, j);
                }
            }
            array_to_set(&elo[esz++], tmp, 3);

            k = 0;
            for (j = 0; j < 4; ++j) {
                if (j != r4) {
                    tmp[k++] = get_index_at(&B[1], r3, j);
                }
            }
            array_to_set(&elo[esz++], tmp, 3);
        } else if (qresult == 0) {
            add_data(&white, query[2]);
            add_data(&white, query[3]);

            k = 0;
            for (j = 0; j < 4; ++j) {
                if (j != r2) {
                    tmp[k++] = get_index_at(&B[1], r1, j);
                }
            }
            array_to_set(&wlo[wsz++], tmp, 3);

            k = 0;
            for (j = 0; j < 4; ++j) {
                if (j != r4) {
                    tmp[k++] = get_index_at(&B[1], r3, j);
                }
            }
            array_to_set(&wlo[wsz++], tmp, 3);
        } else {
            printf("i = %d\t", i);
            print_query(query);
            break;
        }
        remove_from_bucket(&B[1], r1);
        remove_from_bucket(&B[1], r3);
    }

    int bcnt=0, wcnt = 0;


    printf("wsz, bsz, esz: (%d, %d, %d)\n", wsz, bsz, esz);
    int qres3;

    for (i = 0; i < esz; ++i) {
        for (j = 0; j < 3; ++j) {
            query[j] = get_data(&elo[i], j);
        }

        query[3] = get_data(&black, 0);
        qresult = QCOUNT(1, query);

        if (qresult == 4) {
            bcnt += 3;
        } else {
            query[3] = get_data(&white, 0);
            qresult = QCOUNT(1, query);
            if (qresult == 4)
                wcnt += 3;
        }

        if (qresult == 0) {
            bcnt += 2;
            wcnt += 1;
        } else if (qresult == 2) {
            wcnt += 2;
            bcnt += 1;
        }
    }


    for (i = 1; i < bsz; i+=2) {
        for (j = 0; j < 3; ++j) {
            query[j] = get_data(&blo[i-1], j);
        }
        query[3] = get_data(&blo[i], 0);
        
        qresult = QCOUNT(1, query);
        if (qresult == 2) {
            query[3] = get_data(&white, 0);
            qresult = QCOUNT(1, query);
            if (qresult == 4) {
                wcnt += 4;
                bcnt += 2;
            }
            else {
                bcnt += 4;
                wcnt += 2;
            }

        } else if (qresult == 0) {
            query[3] = get_data(&blo[i], 1);
            qresult = QCOUNT(1, query);
            if (qresult == 0) {
                wcnt += 4;
                bcnt += 2;
            } else if (qresult == 2) {
                bcnt += 4;
                wcnt += 2;
            }
        } else if (qresult == 4) {
            for (j = 0; j < 3; ++j)
                query[j] = get_data(&blo[i], j);
            query[3] = get_data(&white, 0);
            qresult = QCOUNT(1, query);
            if (qresult == 4) {
                wcnt += 6;
            } else {
                wcnt += 4;
                bcnt += 2;
            }
        }
    }
    */

    int res;

    if (white.size > black.size)
        res = get_data(&white, 4);
    else if (black.size > white.size)
        res = get_data(&black, 4);
    else
        res = 0;

    /* clean up memory allocs */
    for (i = n_buckets-1; i >= 0 ; --i)
        free_bucket(&B[i]);

    free_set(&even);
    free_set(&white);
    free_set(&black);

    return res;
}
