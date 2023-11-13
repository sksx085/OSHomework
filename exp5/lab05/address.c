#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int PAGE_TABLE_SIZE = 256;
const int BUFFER_SIZE = 256;
const int PHYS_MEM_SIZE = 256;
const int TLB_SIZE = 16;

struct TLB {
    unsigned char TLBpage[16];
    unsigned char TLBframe[16];
    int ind;
};

int readFromDisk(int pageNum, char *PM, int *OF) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    FILE *BS = fopen("BACKING_STORE.bin", "rb");
    if (BS == NULL) {
        printf("文件打开失败\n");
        exit(0);
    }

    if (fseek(BS, pageNum * PHYS_MEM_SIZE, SEEK_SET) != 0)
        printf("fseek 错误\n");

    if (fread(buffer, sizeof(char), PHYS_MEM_SIZE, BS) == 0)
        printf("fread 错误\n");

    for (int i = 0; i < PHYS_MEM_SIZE; i++) {
        *((PM + (*OF) * PHYS_MEM_SIZE) + i) = buffer[i];
    }

    (*OF)++;

    return (*OF) - 1;
}

int findPage(int logicalAddr, char *PT, struct TLB *tlb, char *PM, int *OF, int *pageFaults, int *TLBhits) {
    unsigned char mask = 0xFF;
    unsigned char offset;
    unsigned char pageNum;
    int frame = 0;
    int value;
    int newFrame = 0;

    printf("虚拟地址：%d\t", logicalAddr);

    pageNum = (logicalAddr >> 8) & mask;
    offset = logicalAddr & mask;

    // 检查是否在TLB中
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        if (tlb->TLBpage[i] == pageNum) {
            frame = tlb->TLBframe[i];
            (*TLBhits)++;
            break;
        }
    }

    // 检查是否在页表中
    if (i == TLB_SIZE) {
        if (PT[pageNum] != -1) {
            // printf("页命中\t\t");
        } else {
            // printf("页错误\t");
            newFrame = readFromDisk(pageNum, PM, OF);
            PT[pageNum] = newFrame;
            (*pageFaults)++;
        }
        frame = PT[pageNum];

        tlb->TLBpage[tlb->ind] = pageNum;
        tlb->TLBframe[tlb->ind] = PT[pageNum];
        tlb->ind = (tlb->ind + 1) % TLB_SIZE;
    }

    int index = ((unsigned char)frame * PHYS_MEM_SIZE) + offset;
    value = *(PM + index);
    printf("物理地址：%d\t 数值：%d\n", index, value);

    return 0;
}

int main(int argc, char *argv[]) {
    int val;
    FILE *fd;
    int openFrame = 0;

    int pageFaults = 0;
    int TLBhits = 0;
    int inputCount = 0;

    float pageFaultRate;
    float TLBHitRate;

    char PageTable[PAGE_TABLE_SIZE];
    memset(PageTable, -1, sizeof(PageTable));

    struct TLB tlb;
    memset(tlb.TLBpage, -1, sizeof(tlb.TLBpage));
    memset(tlb.TLBframe, -1, sizeof(tlb.TLBframe));
    tlb.ind = 0;

    char PhyMem[PHYS_MEM_SIZE][PHYS_MEM_SIZE];

    if (argc < 2) {
        printf("参数不足\n程序退出\n");
        exit(0);
    }

    fd = fopen(argv[1], "r");
    if (fd == NULL) {
        printf("文件打开失败\n");
        exit(0);
    }

    while (fscanf(fd, "%d", &val) == 1) {
        findPage(val, PageTable, &tlb, (char *)PhyMem, &openFrame, &pageFaults, &TLBhits);
        inputCount++;
    }

    pageFaultRate = (float)pageFaults / (float)inputCount;
    TLBHitRate = (float)TLBhits / (float)inputCount;
    printf("缺页率 = %.4f\nTLB 命中率= %.4f\n", pageFaultRate, TLBHitRate);
    fclose(fd);

    return 0;
}
