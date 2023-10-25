#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 3

typedef struct {
    int l;
    int r;
} datas;

int ori[10010]; // 原始数据数组

void quick_sort(int start, int end) {
    if (start >= end)
        return;
    int key = ori[start];
    int i = start, j = end;
    while (i < j) {
        while (i < j && ori[j] >= key) {
            j--;
        }
        ori[i] = ori[j];
        while (i < j && ori[i] <= key) {
            i++;
        }
        ori[j] = ori[i];
    }
    ori[i] = key;
    quick_sort(start, i - 1);
    quick_sort(i + 1, end);
}

void merge(int *ori, int *sorted_num, int len, int div) {
    int i = 0, j = div, p = div + 1, q = len - 1;
    int m = 0;
    while (i <= j && p <= q) {
        if (ori[i] <= ori[p]) {
            sorted_num[m++] = ori[i++];
        } else {
            sorted_num[m++] = ori[p++];
        }
    }
    while (i <= j) {
        sorted_num[m++] = ori[i++];
    }
    while (p <= q) {
        sorted_num[m++] = ori[p++];
    }
}

void *thread_sort(void *args) {
    datas *sort_data = (datas *)args;
    int start = sort_data->l, end = sort_data->r;
    quick_sort(start, end);
    return NULL;
}

int main() {
    printf("Input ori:\n");
    int len = 0;
    char c;
    while ((c = getchar()) != '\n') {
        ungetc(c, stdin);
        scanf("%d", &ori[len++]);
    }

    int div = len / 2 - 1;

    pthread_t threads[MAX_THREADS];
    datas s1_par = {0, div};
    datas s2_par = {div + 1, len - 1};

    // 创建快速排序线程
    pthread_create(&threads[0], NULL, thread_sort, &s1_par);
    pthread_create(&threads[1], NULL, thread_sort, &s2_par);

    // 等待快速排序线程结束
    for (int i = 0; i < 2; ++i) {
        pthread_join(threads[i], NULL);
    }

    // 归并两个已排序的子数组
    int *sorted_num = (int *)malloc(len * sizeof(int));
    if (sorted_num == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE); // 退出程序，表示内存分配失败   
    }

    merge(ori, sorted_num, len, div);

    // 输出排序后的数组
    printf("Sorted array:\n");
    for (int i = 0; i < len; ++i) {
        printf("%d ", sorted_num[i]);
    }
    printf("\n");

    // 释放动态分配的内存
    free(sorted_num);

    return 0;
}
