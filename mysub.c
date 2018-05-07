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
    int qcnt2 = 0;

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

    printf("B[2].size: %d\n", B[2].size);

    Set black, white;
    init_set(&black);
    init_set(&white);

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



    int tmp;
    int k;
    int indices2[100];
    int z = 0;
    if (black.size <= 0 && white.size <= 0) {
        query_set(query, &B[1].list[0]);
        tmp = query[3];
        query[3] = get_index_at(&B[1], 1, 0);

        qresult = QCOUNT(1, query);
        if (qresult == 4) {
            /*array_to_set(&black, query, QSIZE);*/
            /*add_data(&white, tmp);*/

            add_data(&black, query[3]);

            query[0] = get_index_at(&B[1], 1, 1);
            query[1] = get_index_at(&B[1], 1, 2);
            query[2] = get_index_at(&B[1], 1, 3);
            query[3] = get_data(&white, 0);

            qresult = QCOUNT(1, query);
            if (qresult == 4) {
                add_data(&white, query[0]);
                indices2[z++] = query[1];
                indices2[z++] = query[2];
            } else {
                add_array_to_bucket(&B[0], query, QSIZE);
            }
        } else {
            k = 1;
            tmp = qresult;
            query[3] = get_index_at(&B[1], 1, k);
            while ((qresult = QCOUNT(1, query)) == tmp) {
                z++;
                k++;
                query[3] = get_index_at(&B[1], 1, k);
            }
            add_data(&black, get_index_at(&B[1],1,0));
            add_data(&white, query[3]);
        }
    } else if( black.size <= 0) {
        k = 0;
        query_set(query, &white);
        query[3] = get_data(&B[1].list[0], k++);
        while ((qresult = QCOUNT(1, query)) == 4 ) {
            z++;
            query[3] = get_data(&B[1].list[0], k++);
        }
        add_data(&black, query[3]);
    } else if (white.size <= 0) {
        k = 0;
        query_set(query, &black);
        query[3] = get_data(&B[1].list[0], k++);
        while ((qresult = QCOUNT(1, query)) == 4) {
            z++;
            query[3] = get_data(&B[1].list[0], k++);
        }
        add_data(&white, query[3]);
    }

    /* trim set of indices that resulted in a 2 from QCOUNT */
    int bcnt =0, wcnt=0;
    int qcnter = 0;
    int delta_w, delta_b, threshold;
    int b1_sz = B[1].size;
    threshold = (n - B[0].size) / 2;
    for (i = 0; i < b1_sz; ++i) {
        query_set(query, &B[1].list[i]);
        query[3] = get_data(&black, 0);

        delta_b = (black.size + bcnt);
        delta_w = (white.size + wcnt);

        if (delta_b >= threshold || delta_w >= threshold)
            break;

        qresult = QCOUNT(1, query);
        qcnter++;
        if (qresult == 4) {
            bcnt += 3;
            wcnt += 1;
        } else if (qresult == 0) {
            wcnt += 3;
            bcnt += 1;
        } else {
            query[3] = get_data(&white, 0);
            qresult = QCOUNT(1, query);
            qcnter++;

            if (qresult == 4) {
                wcnt += 3;
                bcnt += 1;
            } else {
                bcnt += 3;
                wcnt += 1;
            }
        }
    }


    printf("Size B[0,1,2] :  (%d, %d, %d)\n", B[0].size, B[1].size, B[2].size);

    printf("(black set): size: %d\tvalues: ", black.size);
    print_set(&black);

    printf("(white set): size: %d\tvalues: ", white.size);
    print_set(&white);

    printf("(bcnt, wcnt, qcnter, qcnt2, z) : (%d, %d, %d, %d)\n", bcnt, wcnt, qcnter, qcnt2, z);


    int res;

    if (white.size+wcnt > black.size+bcnt)
        res = get_data(&white, 0);
    else if (black.size+bcnt > white.size+wcnt)
        res = get_data(&black, 0);
    else if (black.size+bcnt == white.size+wcnt)
        res = 0;

    /* clean up memory allocs */
    for (i = n_buckets-1; i >= 0 ; --i)
        free_bucket(&B[i]);

    free_set(&white);
    free_set(&black);

    return res;
}
