#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <sstream>

struct MemoryBlock
{
	int start;
	int size;
	bool allocated;

	MemoryBlock(int s, int sz, bool a) : start(s), size(sz), allocated(a) {}
};

class MemoryAllocator
{
private:
	int MAX;
	std::vector<MemoryBlock> memoryBlocks;

public:
	MemoryAllocator(int size) : MAX(size)
	{
		memoryBlocks.emplace_back(0, MAX, false);
	}
	void allocateMemory(int size, char strategy)
	{
		bool allocated = false;
		int indexToInsert = -1;
		MemoryBlock newBlock(0, size, true);

		switch (strategy)
		{
		case 'F': // First Fit
			for (int i = 0; i < memoryBlocks.size(); ++i)
			{
				if (!memoryBlocks[i].allocated && memoryBlocks[i].size >= size)
				{
					newBlock.start = memoryBlocks[i].start;
					indexToInsert = i;
					break;
				}
			}
			break;
		case 'B': // Best Fit
		{
			int bestSize = std::numeric_limits<int>::max();
			for (int i = 0; i < memoryBlocks.size(); ++i)
			{
				if (!memoryBlocks[i].allocated && memoryBlocks[i].size >= size && memoryBlocks[i].size < bestSize)
				{
					bestSize = memoryBlocks[i].size;
					newBlock.start = memoryBlocks[i].start;
					indexToInsert = i;
				}
			}
		}
		break;
		case 'W': // Worst Fit
		{
			int worstSize = -1;
			for (int i = 0; i < memoryBlocks.size(); ++i)
			{
				if (!memoryBlocks[i].allocated && memoryBlocks[i].size > worstSize)
				{
					worstSize = memoryBlocks[i].size;
					newBlock.start = memoryBlocks[i].start;
					indexToInsert = i;
				}
			}
		}
		break;
		default:
			std::cout << "未知策略。请使用 'F', 'B', 或 'W'。\n";
			return;
		}

		// 如果找到合适的内存块，则分配它
		if (indexToInsert != -1)
		{
			memoryBlocks[indexToInsert].start += size;
			memoryBlocks[indexToInsert].size -= size;
			if (memoryBlocks[indexToInsert].size == 0)
			{
				memoryBlocks.erase(memoryBlocks.begin() + indexToInsert);
			}
			memoryBlocks.insert(memoryBlocks.begin() + indexToInsert, newBlock);
			allocated = true;
		}

		if (!allocated)
		{
			std::cout << "内存分配失败。没有足够的空间。\n";
		}
	}

	// 内存释放方法
	void releaseMemory(int start)
	{
		for (int i = 0; i < memoryBlocks.size(); ++i)
		{
			if (memoryBlocks[i].start == start && memoryBlocks[i].allocated)
			{
				// Mark the block as unallocated
				memoryBlocks[i].allocated = false;

				// Try to merge with the previous block if it's unallocated
				if (i > 0 && !memoryBlocks[i - 1].allocated)
				{
					memoryBlocks[i - 1].size += memoryBlocks[i].size;
					memoryBlocks.erase(memoryBlocks.begin() + i);
					i--; // Adjust the index after erasing
				}

				// Try to merge with the next block if it's unallocated
				if (i < memoryBlocks.size() - 1 && !memoryBlocks[i + 1].allocated)
				{
					memoryBlocks[i].size += memoryBlocks[i + 1].size;
					memoryBlocks.erase(memoryBlocks.begin() + i + 1);
				}

				return; // Memory released and merged if necessary, exit the function
			}
		}

		std::cout << "未找到起始地址为 " << start << " 的已分配内存块。\n";
	}

	// 内存压缩方法
	void compactMemory()
	{
		if (memoryBlocks.empty())
			return; // If no memory blocks, nothing to compact.

		// First, we sort the blocks by starting address to ensure adjacent blocks are next to each other.
		std::sort(memoryBlocks.begin(), memoryBlocks.end(), [](const MemoryBlock &a, const MemoryBlock &b)
				  { return a.start < b.start; });

		for (int i = 0; i < memoryBlocks.size() - 1; ++i)
		{
			// If the current block is free and the next block is also free, merge them.
			if (!memoryBlocks[i].allocated && !memoryBlocks[i + 1].allocated)
			{
				memoryBlocks[i].size += memoryBlocks[i + 1].size; // Increase the size of the current block.
				memoryBlocks.erase(memoryBlocks.begin() + i + 1); // Remove the next block.
				--i;											  // Decrement i since we removed an element from the vector.
			}
		}
	}

	// 打印内存状态
	void printMemoryStatus()
	{
		for (const MemoryBlock &block : memoryBlocks)
		{
			std::cout << "[" << block.start << " : " << block.start + block.size - 1 << "] "
					  << (block.allocated ? "Allocated" : "Free") << std::endl;
		}
	}
};

int main()
{
	int initialMemorySize = 1200; 
	MemoryAllocator allocator(initialMemorySize);

	std::string line;
	while (true)
	{
		std::cout << "输入命令（R 申请，L 释放，C 压缩，S 状态，X 退出）: ";
		std::getline(std::cin, line); // 读取整行输入
		std::istringstream iss(line); // 使用istringstream来解析输入
		char command;
		iss >> command;

		if (command == 'R')
		{
			int size;
			char strategy;
			if (!(iss >> size >> strategy))
			{ // 检查是否成功从流中提取数据
				std::cout << "输入格式错误，请按照 '大小 策略' 的格式输入。\n";
				continue;
			}
			allocator.allocateMemory(size, strategy);
		}
		else if (command == 'L')
		{
			int start;
			if (!(iss >> start))
			{ // 检查是否成功从流中提取数据
				std::cout << "输入格式错误，请输入正确的起始地址。\n";
				continue;
			}
			allocator.releaseMemory(start);
		}
		else if (command == 'C')
		{
			allocator.compactMemory();
		}
		else if (command == 'S')
		{
			allocator.printMemoryStatus();
		}
		else if (command == 'X')
		{
			break;
		}
		else
		{
			std::cout << "无效命令，请重试。\n";
		}
	}

	return 0;
}