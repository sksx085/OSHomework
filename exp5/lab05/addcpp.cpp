#include <iostream>
#include <fstream>
#include <cstring>

const int PAGE_TABLE_SIZE = 256;
const int BUFFER_SIZE = 256;
const int PHYS_MEM_SIZE = 256;
const int TLB_SIZE = 16;

struct TLB
{
    unsigned char TLBpage[TLB_SIZE];
    unsigned char TLBframe[TLB_SIZE];
    int ind;
};

int readFromDisk(int pageNum, char *PM, int *OF)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    std::ifstream BS("BACKING_STORE.bin", std::ios::binary);
    if (!BS.is_open())
    {
        std::cerr << "文件打开失败" << std::endl;
        exit(0);
    }
    if (BS.seekg(pageNum * PHYS_MEM_SIZE).fail())
    {
        std::cerr << "fseek错误" << std::endl;
    }

    if (!BS.read(buffer, PHYS_MEM_SIZE))
    {
        std::cerr << "fread错误" << std::endl;
    }

    for (int i = 0; i < PHYS_MEM_SIZE; i++)
    {
        PM[(*OF) * PHYS_MEM_SIZE + i] = buffer[i];
    }

    (*OF)++;

    return (*OF) - 1;
}

int findPage(int logicalAddr, char *PT, TLB *tlb, char *PM, int *OF, int *pageFaults, int *TLBhits, std::ofstream &outputFile)
{
    unsigned char mask = 0xFF;
    unsigned char offset;
    unsigned char pageNum;
    bool TLBhit = false;
    int frame = 0;
    int value;
    int newFrame = 0;

    outputFile << "虚拟地址: " << logicalAddr << "\t";

    pageNum = (logicalAddr >> 8) & mask;
    offset = logicalAddr & mask;

    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb->TLBpage[i] == pageNum)
        {
            frame = tlb->TLBframe[i];
            TLBhit = true;
            (*TLBhits)++;
        }
    }

    if (!TLBhit)
    {
        if (PT[pageNum] == -1)
        {
            newFrame = readFromDisk(pageNum, PM, OF);
            PT[pageNum] = newFrame;
            (*pageFaults)++;
        }
        frame = PT[pageNum];
        tlb->TLBpage[tlb->ind] = pageNum;
        tlb->TLBframe[tlb->ind] = frame;
        tlb->ind = (tlb->ind + 1) % TLB_SIZE;
    }
    int index = ((unsigned char)frame * PHYS_MEM_SIZE) + offset;
    value = *(PM + index);
    outputFile << "物理地址: " << index << "\t数值: " << static_cast<int>(value) << std::endl;
    return 0;
}

int main(int argc, char *argv[])
{
    int val;
    std::ifstream fd(argv[1]);
    std::ofstream outputFile("output.txt");
    int openFrame = 0;

    int pageFaults = 0;
    int TLBhits = 0;
    int inputCount = 0;

    float pageFaultRate;
    float TLBHitRate;

    char PageTable[PAGE_TABLE_SIZE];
    memset(PageTable, -1, sizeof(PageTable));

    TLB tlb;
    memset(tlb.TLBpage, -1, sizeof(tlb.TLBpage));
    memset(tlb.TLBframe, -1, sizeof(tlb.TLBframe));
    tlb.ind = 0;

    char PhyMem[PHYS_MEM_SIZE * PHYS_MEM_SIZE];
    memset(PhyMem, 0, sizeof(PhyMem));

    if (argc < 2)
    {
        std::cerr << "参数不足\n程序退出\n";
        exit(0);
    }

    if (!fd.is_open())
    {
        std::cerr << "文件打开失败\n";
        exit(0);
    }

    while (fd >> val)
    {
        findPage(val, PageTable, &tlb, PhyMem, &openFrame, &pageFaults, &TLBhits, outputFile);
        inputCount++;
    }

    pageFaultRate = static_cast<float>(pageFaults) / static_cast<float>(inputCount);
    TLBHitRate = static_cast<float>(TLBhits) / static_cast<float>(inputCount);
    outputFile << "缺页率 = " << pageFaultRate << "\nTLB命中率 = " << TLBHitRate << std::endl;
    fd.close();
    outputFile.close();

    return 0;
}
