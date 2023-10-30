#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>

using namespace std;

int main() {
    int serverSocket, clientSocket;
    socklen_t clientLength;
    struct sockaddr_un serverAddress, clientAddress;

    // 创建Unix域套接字
    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Socket creation failed!" << endl;
        return 1;
    }

    // 设置服务器地址
    memset(&serverAddress, 0, sizeof(struct sockaddr_un));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, "./unix", sizeof(serverAddress.sun_path) - 1);

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

    // 接受客户端连接
    clientLength = sizeof(struct sockaddr_un);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);
    if (clientSocket == -1) {
        cerr << "Accept failed!" << endl;
        close(serverSocket);
        return 1;
    }

    // 打开输入文件
    ifstream inputFile("big.txt");
    if (!inputFile) {
        cerr << "File not found!" << endl;
        close(clientSocket);
        close(serverSocket);
        return 1;
    }

    string wordToFind = "specific_word"; // 需要在文件中查找的单词
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
            // 将包含单词的行数发送给客户端
            send(clientSocket, &lineNumber, sizeof(int), 0);
        }
    }

    // 如果没有找到特定单词，向客户端发送消息
    if (!found) {
        int notFound = -1;
        send(clientSocket, &notFound, sizeof(int), 0);
    }

    // 关闭文件和套接字
    inputFile.close();
    close(clientSocket);
    close(serverSocket);

    return 0;
}
