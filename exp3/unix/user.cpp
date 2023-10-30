#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>

using namespace std;

int main() {
    int clientSocket;
    struct sockaddr_un serverAddress;

    // 创建Unix域套接字
    clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Socket creation failed!" << endl;
        return 1;
    }

    // 设置服务器地址
    memset(&serverAddress, 0, sizeof(struct sockaddr_un));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, "./unix", sizeof(serverAddress.sun_path) - 1);

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un)) == -1) {
        cerr << "Connection failed!" << endl;
        close(clientSocket);
        return 1;
    }

    int lineNumber;
    ssize_t bytesRead = recv(clientSocket, &lineNumber, sizeof(int), 0);

    // 处理从服务器接收到的数据
    if (bytesRead > 0) {
        if (lineNumber == -1) {
            cout << "Word not found in the file." << endl;
        } else {
            cout << "Word found in line number: " << lineNumber << endl;
        }
    } else {
        cerr << "Error receiving data from server!" << endl;
    }

    // 关闭套接字
    close(clientSocket);

    return 0;
}
