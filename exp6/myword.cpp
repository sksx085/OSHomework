#include <iostream>
#include <cstring>

using namespace std;

#define B 512
#define L 64
#define K 3

#define OK 1
#define ERROR -1

#define File_Block_Length (B - 3)
#define File_Name_Length (B - 1)

#define map_row_num 8
#define map_cow_num 8

#define maxDirectoryNumber 49

#define Buffer_Length 64

typedef struct FileDescriptor
{
	int fileLength;
	int file_allocation_blocknumber[File_Block_Length];
	int file_block_length;
	int beginpos;
	int rwpointer;
	char RWBuffer[Buffer_Length];
} FileDescriptor;

typedef struct Directory
{
	int index;
	int count;
	char fileName[File_Name_Length];
	int isFileFlag;
	int isOpenFlag;
	FileDescriptor fileDescriptor;
} Directory;

char ldisk[L][B];
char memory_area[L * (B - K)];
char mem_area[L * (B - K)] = {'\0'};
Directory Directorys[maxDirectoryNumber + 1];
int bitMap[map_row_num][map_cow_num];

void Init();
void directory();
void show_Menu();
void show_ldisk();
void show_bitMap();
int isExist(int index);
int close(int index);
int open(char *filename);
int getSub(char filename[]);
int create(char *filename);
int destroy(char *filename);
int show_File(char *filename);
int lseek(int index, int position);
int read(int index, char memory_area[], int count);
int write(int index, char memory_area[], int count);
int save(int L_pos, int B_pos, int bufLen, int sub);
int load(int step_L, int bufLen, int pos, char memory_area[]);

void show_Menu()
{
	cout << "==========================================\n";
	cout << "\t\t菜单\n";
	cout << "==========================================\n";
	cout << "  1. 创建文件\n";
	cout << "  2. 列出所有文件信息\n";
	cout << "  3. 当前磁盘使用情况\n";
	cout << "  4. 删除文件\n";
	cout << "  5. 打开文件\n";
	cout << "  6. 关闭文件\n";
	cout << "  7. 改变文件读写指针位置\n";
	cout << "  8. 文件读\n";
	cout << "  9. 文件写\n";
	cout << "  10. 查看文件状态\n";
	cout << "  0. 退出\n";
	cout << "==========================================\n";
}

void Init()
{
	memset(ldisk, 0, sizeof(ldisk));
	for (int i = 0; i <= maxDirectoryNumber; i++)
	{
		memset(Directorys[i].fileName, 0, sizeof(Directorys[i].fileName));
		if (i == 0)
		{
			Directorys[i].index = 0;
			Directorys[i].isFileFlag = 0;
			Directorys[i].count = 0;
		}
		else
		{
			Directorys[i].index = -1;
			Directorys[i].count = -1;
			Directorys[i].isFileFlag = 1;
			Directorys[i].isOpenFlag = 0;
			memset(Directorys[i].fileDescriptor.file_allocation_blocknumber, -1, File_Block_Length);
			Directorys[i].fileDescriptor.file_block_length = 0;
			Directorys[i].fileDescriptor.fileLength = 0;
			Directorys[i].fileDescriptor.beginpos = 0;
			memset(Directorys[i].fileDescriptor.RWBuffer, 0, sizeof(Directorys[i].fileDescriptor.RWBuffer));
			Directorys[i].fileDescriptor.rwpointer = 0;
		}
	}

	for (int i = 0; i < map_row_num; i++)
	{
		for (int j = 0; j < map_cow_num; j++)
		{
			bitMap[i][j] = (i * map_cow_num + j < K) ? 1 : 0;
		}
	}
}

int read(int index, char memory_area[], int count)
{
	int sub = isExist(index);
	if (sub == ERROR)
	{
		cout << "索引不正确！\n";
		return ERROR;
	}

	if (!Directorys[sub].isOpenFlag)
	{
		open(Directorys[sub].fileName);
	}

	int step_L = Directorys[sub].fileDescriptor.file_allocation_blocknumber[0];
	int step_ = Directorys[sub].fileDescriptor.file_block_length - 1;
	int pos = 0;

	for (int i = 0; i < step_; i++)
	{
		load(step_L, B, pos, memory_area);
		pos += B;
		step_L++;
	}

	int strLen = Directorys[sub].fileDescriptor.fileLength - (B * step_);
	load(step_L, strLen, pos, memory_area);

	return OK;
}

int write(int index, char memory_area[], int count)
{
	int sub = isExist(index);
	if (sub == ERROR)
	{
		cout << "索引不正确！\n";
		return ERROR;
	}

	if (!Directorys[sub].isOpenFlag)
	{
		open(Directorys[sub].fileName);
	}

	int i = 0;
	int step_ = 0;
	int num = 0;
	int step_L;
	int step_B;

	while (count)
	{
		Directorys[sub].fileDescriptor.RWBuffer[i] = memory_area[count - 1];
		count--;
		i++;

		if (i == Buffer_Length)
		{
			step_L = Directorys[sub].fileDescriptor.file_allocation_blocknumber[step_];
			step_B = Buffer_Length * num;

			save(step_L, step_B, Buffer_Length, sub);
			num++;

			if (num == B / Buffer_Length)
			{
				num = 0;
				step_++;
			}

			i = 0;
		}

		if (count == 0)
		{
			step_L = Directorys[sub].fileDescriptor.file_allocation_blocknumber[step_];
			step_B = Buffer_Length * num;

			save(step_L, step_B, i, sub);
			break;
		}
	}

	memset(Directorys[sub].fileDescriptor.RWBuffer, '\0', Buffer_Length);

	return OK;
}

int load(int step_L, int bufLen, int pos, char memory_area[])
{
	for (int i = 0; i < bufLen; i++)
	{
		memory_area[pos + i] = ldisk[step_L][i];
	}
	return OK;
}

int save(int L_pos, int B_pos, int bufLen, int sub)
{
	for (int i = 0; i < bufLen; i++)
	{
		ldisk[L_pos][B_pos + i] = Directorys[sub].fileDescriptor.RWBuffer[i];
	}
	return OK;
}

int isExist(int index)
{
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (Directorys[i].index == index)
		{
			return i;
		}
	}
	return ERROR;
}

int getSub(char filename[])
{
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (strcmp(Directorys[i].fileName, filename) == 0)
		{
			return i;
		}
	}
	return ERROR;
}

int create(char *filename)
{
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (strcmp(Directorys[i].fileName, filename) == 0)
		{
			cout << "该文件已经存在，无需创建！\n";
			return ERROR;
		}
	}

	int sub;
	for (sub = 1; sub <= maxDirectoryNumber; sub++)
	{
		if (Directorys[sub].index == -1)
		{
			break;
		}
		else if (sub == maxDirectoryNumber)
		{
			cout << "磁盘已满，无法创建文件！";
			return ERROR;
		}
	}

	strcpy(Directorys[sub].fileName, filename);
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (isExist(i) == -1)
		{
			Directorys[sub].index = i;
			break;
		}
	}

	cout << "请输入内存大小 (提示：最大为61*512 Byte) ";
	cin >> Directorys[sub].fileDescriptor.fileLength;

	int L_Counter = (Directorys[sub].fileDescriptor.fileLength % B != 0) ? (Directorys[sub].fileDescriptor.fileLength / B + 1) : (Directorys[sub].fileDescriptor.fileLength / B);
	int i = K;
	for (; i < map_row_num * map_cow_num - L_Counter; i++)
	{
		int outflag = 0;
		for (int j = 0; j < L_Counter; j++)
		{
			int maprow = (i + j) / map_cow_num;
			int mapcow = (i + j) % map_cow_num;
			if (bitMap[maprow][mapcow])
			{
				break;
			}
			else
			{
				if (j == L_Counter - 1)
				{
					outflag = 1;
				}
			}
		}
		if (outflag == 1)
		{
			Directorys[sub].fileDescriptor.file_block_length = L_Counter;
			Directorys[sub].fileDescriptor.beginpos = i;
			for (int j = 0; j < L_Counter; j++)
			{
				Directorys[sub].fileDescriptor.file_allocation_blocknumber[j] = i + j;
			}
			Directorys[sub].isOpenFlag = 0;
			Directorys[sub].fileDescriptor.rwpointer = 0;
			memset(Directorys[sub].fileDescriptor.RWBuffer, '\0', Buffer_Length);
			break;
		}
		else if (L_Counter + i == map_row_num * map_cow_num - 1 - K)
		{
			cout << "内存不足，无法分配！\n";
			Directorys[sub].index = -1;
			return ERROR;
		}
	}

	int map_ = i;
	cout << "文件 " << filename << " 创建成功!\n";

	for (int j = 0; j < Directorys[sub].fileDescriptor.file_block_length; j++)
	{
		int maprow = (map_ + j) / map_cow_num;
		int mapcow = (map_ + j) % map_cow_num;
		bitMap[maprow][mapcow] = 1;
	}
	Directorys[0].count++;

	return OK;
}

int destroy(char *filename)
{
	int sub;
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (strcmp(Directorys[i].fileName, filename) == 0)
		{
			sub = i;
			break; 
		}
		else if (i == maxDirectoryNumber)
		{
			cout << "该文件不存在！\n";
			return ERROR;
		}
	}

	if (Directorys[sub].isOpenFlag)
	{
		cout << "文件打开，无法删除！\n";
		return ERROR;
	}

	int position = Directorys[sub].fileDescriptor.file_allocation_blocknumber[0];
	for (int i = 0; i < Directorys[sub].fileDescriptor.file_block_length; i++)
	{
		int d_row = (position + i) / map_row_num;
		int d_cow = (position + i) % map_row_num;
		bitMap[d_row][d_cow] = 0;
	}

	memset(Directorys[sub].fileName, 0, File_Name_Length);
	Directorys[sub].index = -1;
	memset(Directorys[sub].fileDescriptor.file_allocation_blocknumber, -1, File_Block_Length);
	Directorys[sub].fileDescriptor.file_block_length = 0;
	Directorys[sub].fileDescriptor.fileLength = 0;
	Directorys[sub].fileDescriptor.beginpos = 0;
	memset(Directorys[sub].fileDescriptor.RWBuffer, '\0', Buffer_Length);
	Directorys[sub].fileDescriptor.rwpointer = 0;

	cout << "文件 " << filename << " 删除成功！\n";
	Directorys[0].count--;
	return OK;
}

int open(char *filename)
{
	int sub;
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (strcmp(Directorys[i].fileName, filename) == 0)
		{
			sub = i;
			break;
		}
		else if (i == maxDirectoryNumber)
		{
			return ERROR; 
		}
	}
	Directorys[sub].isOpenFlag = 1;
	return OK;
}

int lseek(int index, int position)
{
	int sub;
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (Directorys[i].index == index)
		{
			sub = i;
			break; 
		}
		else if (i == maxDirectoryNumber)
		{
			cout << "index 数据有错误，找不到该索引\n";
			return ERROR;
		}
	}
	Directorys[sub].fileDescriptor.rwpointer = position;
	return OK;
}

int close(int index)
{
	int sub;
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (Directorys[i].index == index)
		{
			sub = i;
			if (!Directorys[i].isOpenFlag)
			{
				cout << "该文件已经为关闭状态！\n";
				return ERROR;
			}
			break; 
		}
		else if (i == maxDirectoryNumber)
		{
			cout << "文件不存在，打开失败\n";
			return ERROR;
		}
	}

	int pos = Directorys[sub].fileDescriptor.file_allocation_blocknumber[0];
	for (int i = 0; i < Directorys[sub].fileDescriptor.fileLength; i++)
	{
		int L_Pos = i / B;
		int B_Pos = i % B;
		ldisk[pos + L_Pos][B_Pos] = Directorys[sub].fileDescriptor.RWBuffer[i];
	}

	Directorys[sub].isOpenFlag = 0;
	Directorys[sub].fileDescriptor.rwpointer = 0;
	return OK;
}

void directory()
{
	if (Directorys[0].count == 0)
	{
		cout << "目前没有文件\n";
	}
	for (int i = 1; i <= Directorys[0].count; i++)
	{
		if (Directorys[i].index != -1)
		{
			cout << "第 " << i << " 个文件为：" << Directorys[i].fileName << endl;
			cout << "文件长度为：" << Directorys[i].fileDescriptor.fileLength << " Byte\n";
		}
	}
}

void show_ldisk()
{
	for (int i = 0; i < L; i++)
	{
		cout << i << ": " << ldisk[i] << endl;
	}
	cout << endl;
}

int show_File(char *filename)
{
	int sub;
	for (int i = 1; i <= maxDirectoryNumber; i++)
	{
		if (strcmp(Directorys[i].fileName, filename) == 0)
		{
			sub = i;
			break; 
		}
		else if (i == maxDirectoryNumber)
		{
			cout << "未找到文件!!" << endl;
			return ERROR; 
		}
	}
	cout << "文件名：" << Directorys[sub].fileName << endl;
	cout << "文件打开状态（1为打开，0为关闭）：" << Directorys[sub].isOpenFlag << endl;
	cout << "文件的 index 索引值: " << Directorys[sub].index << endl;
	cout << "文件的长度：" << Directorys[sub].fileDescriptor.fileLength << " Byte\n";
	return OK;
}

void handleFileCreation()
{
	char newFile[20];
	cout << "请输入文件名：  ";
	scanf(" %s", newFile);
	create(newFile);
}

void handleFileDirectoryDisplay()
{
	directory();
}

void handleDiskUsageDisplay()
{
	show_ldisk();
}

void handleFileDeletion()
{
	printf("请输入要删除的文件名:");
	char destroyfile[20];
	scanf(" %s", destroyfile);
	destroy(destroyfile);
}

void handleFileOpening()
{
	printf("请输入要打开的文件名:");
	char openfile[20];
	scanf(" %s", openfile);
	if (open(openfile) == ERROR)
		printf("不存在文件 %s ，打开失败\n", openfile);
	else
		printf("文件 %s 打开成功！\n", openfile);
}

void handleFileClosing()
{
	printf("请输入要关闭的文件名:");
	char closefile[20];
	scanf(" %s", closefile);
	int sub = getSub(closefile);
	if (sub == 0)
		printf("不存在文件 %s ，关闭失败\n", closefile);
	else
	{
		if (close(Directorys[sub].index) != ERROR)
			printf("文件 %s 关闭成功！\n", closefile);
	}
}

void handleChangePointerPosition()
{
	printf("请输入要操作的文件名：");
	char movefile[20];
	int move_pos;
	scanf(" %s", movefile);
	printf("请输入要移动到的位置：");
	scanf("%d", &move_pos);
	int sub = getSub(movefile);
	if (sub != 0)
	{
		if (lseek(Directorys[sub].index, move_pos))
		{
			printf("文件 %s 指针移动成功！\n", movefile);
		}
		else
			printf("文件 %s 移动失败..\n", movefile);
	}
}

void handleFileReading()
{
	printf("请输入要读的文件名：");
	char readfile[20];
	scanf(" %s", readfile);
	int sub = getSub(readfile);
	if (sub != 0)
	{
		read(Directorys[sub].index, memory_area, Directorys[sub].fileDescriptor.fileLength);
		printf("%s\n", memory_area);
	}
	else
		printf("文件名输入错误..\n");
}

void handleFileWriting()
{
	printf("请输入要写入数据的文件名：");
	char writefile[20];
	scanf(" %s", writefile);
	int sub = getSub(writefile);
	if (sub != 0)
	{
		char writebuf[L * (B - K)];
		memset(writebuf, '\0', L * (B - K));
		printf("请输入要写入的数据：");
		scanf(" %s", writebuf);
		int len = 0;
		for (int i = 0; i < Directorys[sub].fileDescriptor.fileLength; i++)
		{
			if (writebuf[i] == '\0')
			{
				len = i;
				break;
			}
		}
		write(Directorys[sub].index, writebuf, len);
	}
	else
		printf("文件名输入错误..\n");
}

void handleFileQuerying()
{
	printf("请输入要查询的文件名：\n");
	char seefile[20];
	scanf(" %s", seefile);
	if (getSub(seefile))
		show_File(seefile);
	else
		printf("文件名不存在...\n");
}

int main()
{
	int scanner;
	Init(); 
	do
	{
		show_Menu(); 
		printf("请选择： ");
		scanf("%d", &scanner);

		switch (scanner)
		{
		case 1:
			handleFileCreation();
			break;
		case 2:
			handleFileDirectoryDisplay();
			break;
		case 3:
			handleDiskUsageDisplay();
			break;
		case 4:
			handleFileDeletion();
			break;
		case 5:
			handleFileOpening();
			break;
		case 6:
			handleFileClosing();
			break;
		case 7:
			handleChangePointerPosition();
			break;
		case 8:
			handleFileReading();
			break;
		case 9:
			handleFileWriting();
			break;
		case 10:
			handleFileQuerying();
			break;
		default:
			if (scanner != 0)
			{
				printf("选择无效，请重新选择： ");
			}
			break;
		}
		printf("\n");
	} while (scanner != 0);

	return 0;
}
