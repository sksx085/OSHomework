#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

using namespace std;

const char* SOCKET_PATH = "socket.sock";

int main() {
    // 创建Unix域套接字
    int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Socket creation failed!" << endl;
        return 1;
    }

    struct sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(struct sockaddr_un));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);

    // 绑定套接字到服务器地址
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un)) == -1) {
        cerr << "Bind failed!" << endl;
        close(serverSocket);
        return 1;
    }

    // 监听连接请求
    if (listen(serverSocket, 5) == -1) {
        cerr << "Listen failed!" << endl;
        close(serverSocket);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        cerr << "Fork failed!" << endl;
        return 1;
    }

    if (pid == 0) { // 子进程
        // 创建客户端套接字，连接到服务器套接字
        int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cerr << "Client socket creation failed!" << endl;
            return 1;
        }

        struct sockaddr_un clientAddress;
        memset(&clientAddress, 0, sizeof(struct sockaddr_un));
        clientAddress.sun_family = AF_UNIX;
        strncpy(clientAddress.sun_path, SOCKET_PATH, sizeof(clientAddress.sun_path) - 1);

        if (connect(clientSocket, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr_un)) == -1) {
            cerr << "Connection failed!" << endl;
            close(clientSocket);
            return 1;
        }

        ofstream outputFile("output.txt"); // 打开输出文件

        char buffer[100];
        ssize_t bytesRead;

        // 循环读取套接字数据，直到读取结束
        while ((bytesRead = read(clientSocket, buffer, sizeof(buffer))) > 0) {
            outputFile << "Child Process Output: " << buffer << endl;
        }

        // 关闭文件和套接字
        outputFile.close();
        close(clientSocket);

        cout << "Output written to output.txt." << endl;
    } else { // 父进程
        // 等待客户端连接
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            cerr << "Accept failed!" << endl;
            close(serverSocket);
            return 1;
        }

        ifstream inputFile("big.txt"); // 打开文件
        if (!inputFile) {
            cerr << "File not found!" << endl;
            close(clientSocket);
            close(serverSocket);
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
                // 输出单词和行数到套接字，供子进程读取
                write(clientSocket, line.c_str(), line.size());
            }
        }

        // 如果没有找到特定单词，向子进程发送消息
        if (!found) {
            write(clientSocket, "Word not found", sizeof("Word not found"));
        }

        // 关闭套接字和文件
        close(clientSocket);
        close(serverSocket);
        inputFile.close();

        wait(NULL); // 等待子进程结束
    }

    // 删除套接字文件
    unlink(SOCKET_PATH);

    return 0;
}

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

using namespace std;

const char* SOCKET_PATH = "socket.sock";

int main() {
    // 创建Unix域套接字
    int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Socket creation failed!" << endl;
        return 1;
    }

    struct sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(struct sockaddr_un));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);

    // 绑定套接字到服务器地址
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un)) == -1) {
        cerr << "Bind failed!" << endl;
        close(serverSocket);
        return 1;
    }

    // 监听连接请求
    if (listen(serverSocket, 5) == -1) {
        cerr << "Listen failed!" << endl;
        close(serverSocket);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        cerr << "Fork failed!" << endl;
        return 1;
    }

    if (pid == 0) { // 子进程
        // 创建客户端套接字，连接到服务器套接字
        int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cerr << "Client socket creation failed!" << endl;
            return 1;
        }

        struct sockaddr_un clientAddress;
        memset(&clientAddress, 0, sizeof(struct sockaddr_un));
        clientAddress.sun_family = AF_UNIX;
        strncpy(clientAddress.sun_path, SOCKET_PATH, sizeof(clientAddress.sun_path) - 1);

        if (connect(clientSocket, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr_un)) == -1) {
            cerr << "Connection failed!" << endl;
            close(clientSocket);
            return 1;
        }

        ofstream outputFile("output.txt"); // 打开输出文件

        char buffer[100];
        ssize_t bytesRead;

        // 循环读取套接字数据，直到读取结束
        while ((bytesRead = read(clientSocket, buffer, sizeof(buffer))) > 0) {
            outputFile << "Child Process Output: " << buffer << endl;
        }

        // 关闭文件和套接字
        outputFile.close();
        close(clientSocket);

        cout << "Output written to output.txt." << endl;
    } else { // 父进程
        // 等待客户端连接
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            cerr << "Accept failed!" << endl;
            close(serverSocket);
            return 1;
        }

        ifstream inputFile("big.txt"); // 打开文件
        if (!inputFile) {
            cerr << "File not found!" << endl;
            close(clientSocket);
            close(serverSocket);
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
                // 输出单词和行数到套接字，供子进程读取
                write(clientSocket, line.c_str(), line.size());
            }
        }

        // 如果没有找到特定单词，向子进程发送消息
        if (!found) {
            write(clientSocket, "Word not found", sizeof("Word not found"));
        }

        // 关闭套接字和文件
        close(clientSocket);
        close(serverSocket);
        inputFile.close();

        wait(NULL); // 等待子进程结束
    }

    // 删除套接字文件
    unlink(SOCKET_PATH);

    return 0;
}
