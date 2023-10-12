#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // 创建服务器套接字
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create server socket" << std::endl;
        return 1;
    }
    //绑定服务器地址和端口
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);
    if (bind(serverSocket, (struct sockaddr *) &serverAddress,
             sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind server address" << std::endl;
        return 1;
    }
    // 监听连接请求
    if (listen(serverSocket, 1) < 0) {
        std::cerr << "Failed to listen on server socket" << std::endl;
        return 1;
    }
    std::cout << "Server started. Listening on port 8080..." << std::endl;
    while (true) { // 接受客户端连接
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress,
                                  &clientAddressLength);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client connection " << std::endl;
            continue;
        }
        // 从客户端读取数据
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        if (bytesRead < 0) {
            std::cerr << "Failed to read data from client" << std::endl;
        } else if (bytesRead == 0) {
            std::cout << "Client disconnected" << std::endl;
        } else {
            std::cout << "Received data from client: " << buffer << std::endl;
            // 判断是否为GET请求
            if (strncmp(buffer, "GET", 3) == 0) {
                // 发送响应给客户端 const
                char *response =
                        "HTTP/1.1 200 OK\r\nContent-Length:12\r\n\r\nHello,World !";
                ssize_t bytesSent = send(clientSocket, response, strlen(response), 0);
                if (bytesSent < 0) {
                    std::cerr << "Failed to send response to client " << std::endl;
                }
            }
        }
        // 关闭客户端套接字
        close(clientSocket);
    }
    // 关闭服务器套接字
    close(serverSocket);
    return 0;
}
