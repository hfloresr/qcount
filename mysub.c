#include <stdio.h>


typedef struct {
    int result;
    int ind[4];
} qbucket;

void init_indices(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        arr[i] = i+1;
}

void init_buckets(qbucket arr[], int n)
{
    int i, j;
    for (i = 0; i < n; ++i) {
            arr[i].result = -13;
            for (j = 0; j < 4; ++j)
                arr[i].ind[j]= 0;
    }
}

void update_indices(qbucket b[], int query[], int *cnt)
{
    int i;
    for (i = 0; i < 4; ++i)
        b[(*cnt)++].ind[i]  = query[i];
}

/*
void store_result(qbucket b, int query[], int res, int sz)
{
        if (res == 4)
            update_indices()
}
*/

void new_query(int query[], int indices[], int sz, int start)
{
    int i;
    /* initialize query */
    for (i = 0; i < sz; ++i) {
        query[i] = indices[start+i];
    }
}

int mysub(int n)
{
    int qsize = 4;
    int nb = (n + 1) / 4;
    int indices[n];


    int query[qsize];
    int qresult;

    init_indices(indices, n);

    /* buckets */
    int cnt4 = 0, cnt2 = 0, cnt0 = 0;   /* counter */
    qbucket b4[nb];
    qbucket b2[nb];
    qbucket b0[nb];

    init_buckets(b4, nb);
    init_buckets(b2, nb);
    init_buckets(b0, nb);

    int i, j;
    for (i = 0; i < 16; i+=4) {
        new_query(query, indices, qsize, i);
        qresult = QCOUNT(1, query);
        if (qresult == 4)
            update_indices(b4, query, &cnt4);
        else if (qresult == 2)
            update_indices(b2, query, &cnt2);
        else if (qresult == 0)
            update_indices(b0, query, &cnt0);
        else
            return -1;
        printf("query results: %d\n", qresult);
    }
    printf("cnt4 = %d\tcnt2 = %d\tctn0 = %d\n", cnt4, cnt2, cnt0);

    return 0;
}
