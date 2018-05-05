#include <stdio.h>


typedef struct {
    int result;
    int ind[4];
    int size;
} qbucket;


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

void init_buckets(qbucket arr[], int n)
{
    int i, j;
    for (i = 0; i < n; ++i) {
            arr[i].result = -13;
            for (j = 0; j < 4; ++j)
                arr[i].ind[j]= 0;
    }
}

void update_indices(qbucket b[], int query[], int cnt)
{
    int i;
    for (i = 0; i < 4; ++i)
        b[cnt].ind[i]  = query[i];
}


void new_query(int query[], int indices[], int sz, int start)
{
    int i;
    for (i = 0; i < sz; ++i) {
        query[i] = indices[start+i];
    }
}

void combine_b4_indices(qbucket b[], int query[], int bin1, int bin2)
{
    query[0] = qbucket[bin1].ind[0];
    query[1] = qbucket[bin1].ind[1];
    query[2] = qbucket[bin2].ind[2];
    query[3] = qbucket[bin2].ind[3];
}


/* bin1 < bin 2 */
void remove_bin1_bin2(qbucket, b[], int sz, int bin1, int bin2)
{
    if (bin2 < sz-2)
        qbucket[bin2] = qbucket[sz-2];
    if (bin1 < sz-2)
        qbucket[bin1] = qbucket[sz-1];
}

void print_bucket(qbucket b[], int sz)
{
    int i, j;
    for (i = 0; i < sz; ++i) {
        printf("set: %d\n", i);
        printf("\tindices: ");
        for (j = 0; j < 4; ++j) {
            printf("%d, ", b[i].ind[j]);
        }
        printf("\n");
    }

}

int mysub(int n)
{
    int i, j;
    int qsize, qresult, bsize;

    qsize = 4;                  /* query size is 4 */
    bsize = (n / 2) + 1;        /* bin size */

    /* indices of marbles */
    int indices[n];
    init_indices(indices, n);

    int query[qsize];           /* store current query */
    qbucket b4[bsize];          /* store queries that resulted in 4 */
    qbucket b2[bsize];          /* store queries that resulted in 2 */
    qbucket b0[bsize];          /* store queries that resulted in 0 (equal 1's and 0's )*/

    init_buckets(b4, bsize);
    init_buckets(b2, bsize);
    init_buckets(b0, bsize);

    int cnt[3] = {0, 0, 0};

    for (i = 0; i < n; i+=4) {
        new_query(query, indices, qsize, i);
        qresult = QCOUNT(1, query);
        if (qresult == 4) {
            update_indices(b4, query, cnt[2]);
            cnt[2]++;
        }
        else if (qresult == 2) {
            update_indices(b2, query, cnt[1]);
            cnt[1]++;
        }
        else if (qresult == 0) {
            update_indices(b0, query, cnt[0]);
            cnt[0]++;
        }
        else
            break;
    }

    qbucket set[cnt[2]];

    j = 0;
    i = 0;
    for (j = 1; j < cnt[2]; ++j)
    while (i < cnt[2]) {
        bin1 = i, bin2 = bin1+j;
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

    printf("cnt4 = %d\tcnt2 = %d\tctn0 = %d\n", cnt[2], cnt[1], cnt[0]);

    printf("\nbucket 4:\n");
    print_bucket(b4, cnt[2]);

    printf("\nbucket 2:\n");
    print_bucket(b2, cnt[1]);

    printf("\nbucket 0:\n");
    print_bucket(b0, cnt[0]);

    return 1;
}
