#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <chrono>  // 添加chrono头文件

using namespace std;

//const int SHM_SIZE = 1024 * 1024 * 4; // 共享内存的大小
ofstream outputFile("output.txt");
int main() {
    auto start = chrono::high_resolution_clock::now();  // 记录开始时间

    int pipefd[2];
    pid_t pid;

    // 创建管道
    if (pipe(pipefd) == -1) {
        cerr << "Pipe creation failed!" << endl;
        return 1;
    }

    // 创建子进程
    pid = fork();

    if (pid == -1) {
        cerr << "Fork failed!" << endl;
        return 1;
    }

    if (pid == 0) { // 子进程
        close(pipefd[1]); // 子进程关闭写入端

        ofstream outputFile("output.txt"); // 打开输出文件

        char buffer[100];
        ssize_t bytesRead;

        // 循环读取管道数据，直到读取结束
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            outputFile << "Child Process Output: " << buffer << endl;
        }

        // 关闭文件和管道
        outputFile.close();
        close(pipefd[0]);

        cout << "Output written to output.txt." << endl;
    } else { // 父进程
        close(pipefd[0]); // 父进程关闭读取端

        ifstream inputFile("big.txt"); // 打开文件
        if (!inputFile) {
            cerr << "File not found!" << endl;
            close(pipefd[1]);
            return 1;
        }

        string wordToFind = "call"; // 需要在文件中查找的单词
        string line;
        int lineNumber = 0;
        bool found = false;

        // 逐行读取文件内容，查找特定单词
        while (getline(inputFile, line)) {
            lineNumber++;
            if (line.find(wordToFind) != string::npos) {
                found = true;
                // 输出单词和行数
                cout << "Word found: " << wordToFind << " at line number: " << lineNumber << endl;
                // 将结果写入管道，供子进程读取
                write(pipefd[1], line.c_str(), line.size());
            }
        }

        // 如果没有找到特定单词，向子进程发送消息
        if (!found) {
            write(pipefd[1], "Word not found", sizeof("Word not found"));
        }

        // 关闭写入端和文件
        close(pipefd[1]);
        inputFile.close();

        wait(NULL); // 等待子进程结束
    }


    auto end = chrono::high_resolution_clock::now();  // 记录结束时间
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();  // 计算执行时间，单位为微秒
    cout << "Execution Time: " << duration << " microseconds" << endl;  // 输出执行时间

    return 0;
}
