#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <chrono>  // 添加chrono头文件

using namespace std;

const int SHM_SIZE = 1024 * 1024 * 4; // 共享内存的大小

int main() {
    auto start = chrono::high_resolution_clock::now();  // 记录开始时间

    // 创建共享内存段
    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        cerr << "Shared memory creation failed!" << endl;
        return 1;
    }

    // 创建子进程
    pid_t pid = fork();

    if (pid == -1) {
        cerr << "Fork failed!" << endl;
        return 1;
    }

    if (pid == 0) { // 子进程
        char* sharedMemory = static_cast<char*>(shmat(shmid, NULL, 0));
        if (sharedMemory == (char*)-1) {
            cerr << "Shared memory attachment failed!" << endl;
            return 1;
        }

        ifstream inputFile("big.txt"); // 打开文件
        if (!inputFile) {
            cerr << "File not found!" << endl;
            shmdt(sharedMemory); // 分离共享内存
            shmctl(shmid, IPC_RMID, NULL); // 删除共享内存段
            return 1;
        }

        // 读取文件内容到共享内存
        inputFile.read(sharedMemory, SHM_SIZE);
        inputFile.close();
        sharedMemory[SHM_SIZE - 1] = '\0'; // 确保共享内存以 null 结尾

        string wordToFind = "call"; // 需要在文件中查找的单词
        string content(sharedMemory);

        // 找到所有包含指定单词的行，并将这些行的内容和行号输出到output.txt文件中
        ofstream outputFile("output.txt");
        size_t pos = content.find(wordToFind, 0);
        size_t lineNumber = 1;
        while (pos != string::npos) {
            // 找到单词所在的行的开始位置
            size_t lineStart = content.rfind('\n', pos) + 1;
            // 找到单词所在的行的结束位置
            size_t lineEnd = content.find('\n', pos);
            if (lineEnd == string::npos) {
                lineEnd = content.size();
            }
            // 提取单词所在的行的内容
            string lineContent = content.substr(lineStart, lineEnd - lineStart);
            // 输出单词所在的行的内容和行号
            outputFile << "Line " << lineNumber << ": " << lineContent << endl;

            // 继续查找下一个单词
            pos = content.find(wordToFind, pos + 1);
            lineNumber++;
        }

        outputFile.close();
        shmdt(sharedMemory); // 分离共享内存
        shmctl(shmid, IPC_RMID, NULL); // 删除共享内存段
    } else { // 父进程
        wait(NULL); // 等待子进程结束
    }
    auto end = chrono::high_resolution_clock::now();  // 记录结束时间
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();  // 计算执行时间，单位为微秒
    cout << "Execution Time: " << duration << " microseconds" << endl;  // 输出执行时间

    return 0;
}
