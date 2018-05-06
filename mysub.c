#include <stdio.h>
#include <stdlib.h>

#define INITCAP 32


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

    printf("Index %d out of bounds\n", idx);
    return -88;
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


void free_set(Set *set)
{
    free(set->data);
    set->size = set->capacity = 0;
}


typedef struct {
    int size;
    Set list[512];
} Bucket;

void init_bucket_list(Bucket *b)
{
    int i;
    for (i = 0; i < 512; ++i)
        init_set(&b->list[i]);
    b->size = 0;
}

void clear_buckets(Bucket *b)
{
    int i;
    for (i = 511; i >= 0 ; --i)
        free_set(&b->list[i]);
    b->size = 0;
}

int add_array_to_bucket(Bucket *b, int arr[], int n)
{
    int is_full;

    if ((is_full = b->size >= 512) == 1)
        printf("Bucket is full\n");
    else
        array_to_set(&b->list[b->size++], arr, n);
    return (is_full ? 0 : 1);
}


int add_set_to_bucket(Bucket *b, Set *s)
{
    int is_full;

    if ((is_full = b->size >= 512) == 1)
        printf("Bucket is full\n");
    else
        b->list[b->size++] = *s;


    return (is_full ? 0 : 1);
}


void remove_from_bucket(Bucket *b, int idx)
{
    if(idx < b->size && idx >= 0)
        b->list[idx] = b->list[b->size--];

    b->size--;
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


void new_query(int query[], int indices[], int sz, int start)
{
    int i;
    for (i = 0; i < sz; ++i) {
        query[i] = indices[start+i];
    }
}

int map_qcount(int qresult)
{
    return qresult == 0 ? 0 :
           qresult == 2 ? 1 :
           qresult == 4 ? 2 :
           qresult;
}

int mysub(int n)
{
    int i;
    int qsize, qresult, qbin;

    /* indices of marbles */
    int indices[n];
    init_indices(indices, n);

    qsize = 4;                  /* query size is 4 */
    int query[qsize];           /* store current query */

    /* qresult=0 -> B[0] ; qresult=2 -> B[1] ; qresult=4 -> B[2] */
    Bucket B[3];
    for (i = 0; i < 3; ++i)
        init_bucket_list(&B[i]);

    for (i = 0; i < n; i+=4) {
        new_query(query, indices, qsize, i);
        qresult = QCOUNT(1, query);
        qbin = map_qcount(qresult);
        printf("(qresult, qbin) = (%d, %d)\n", qresult, qbin);
        if (qbin >= 0)
            add_array_to_bucket(&B[qbin], query, qsize);
        else
            break;
    }

    for (i = 0; i < 3; ++i) {
        printf("B[%d] size = %d\n", i, B[i].size);
    }


    /*
    i = 0;
    int bin1, bin2;
    while (i < cnt[2]) {
        bin1 = i, bin2 = bin1+1;
        combine_b4_indices(b4, query, bin1, bin2);
        qresult = QCOUNT(1, query);
        if (qresult == 0) {
            update_indices(b0, query, cnt[0]);
            cnt[0] += 2;
            remove_bin1_bin2(b4, cnt[3], bin1, bin2);
            cnt[2] -= 2;
        }
        else if (qresult == 4) {
            i += 2;
        }
    }
    */

    for (i = 2; i >= 0 ; --i)
        clear_buckets(&B[i]);

    return 1;
}
