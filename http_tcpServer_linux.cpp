#include "http_tcpServer_linux.h"
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace {
    void log(const std::string &message) {
        std::cout << message << std::endl;
    }

    void exitWithError(const std::string &errorMsg){
        log("Error: " + errorMsg);
        exit(0);
    }
}

namespace http {
    const int BUFFER_SIZE = 30720;
    std::string TcpServer::buildResponse() {
        // Create a simple HTTP response or any server message you want to send
        std::stringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/plain\r\n";
        response << "Content-Length: 13\r\n";
        response << "\r\n";
        response << "Hello, World!";
        return response.str();
    }

    TcpServer::TcpServer(std::string ip_address, int port) : 
    m_ip_address(ip_address), m_port(port), m_socket(), m_new_socket(), m_incoming_message(), m_socketAddress(), 
    m_socketAddress_len(sizeof(m_socketAddress)), m_server_message(buildResponse())
    {
        m_socketAddress.sin_family = AF_INET;
        m_socketAddress.sin_port = htons(m_port);
        m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

        if (startServer() != 0)
        {
            std::ostringstream ss;
            ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
            log(ss.str());
        }
    }
    TcpServer::~TcpServer() {
        closeServer();
    }
    int TcpServer::startServer() {
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket < 0) {
            exitWithError("Unable to create socket");
            return 1;
        }

        if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0)
        {
            exitWithError("Cannot connect socket to address");
            return 1;
        }

        return 0;
    }

    void TcpServer::closeServer() {
        close(m_socket);
        close(m_new_socket);
        exit(0);
    }

    void TcpServer::sendResponse() {
        long bytesSent;

        bytesSent = write(m_new_socket, m_server_message.c_str(), m_server_message.size());

        if (bytesSent == m_server_message.size())
        {
            log("------ Server Response sent to client ------\n\n");
        }
        else
        {
            log("Error sending response to client");
        }
    }

    void TcpServer::acceptConnection(int &new_socket)
    {
        new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
        if (new_socket < 0)
        {
            std::ostringstream ss;
            ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << "; PORT: " << ntohs(m_socketAddress.sin_port);
            exitWithError(ss.str());
        }
    }

    void TcpServer::startListen()
    {
        if (listen(m_socket, 20) < 0)
        {
            exitWithError("Socket listen failed");
        }

        std::ostringstream ss;
        ss << "\n*** Listening on ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << " PORT: " << ntohs(m_socketAddress.sin_port) << " ***\n\n";
        log(ss.str());

        int bytesReceived;

        while (true)
        {
            log("====== Waiting for a new connection ======\n\n\n");
            acceptConnection(m_new_socket);

            char buffer[BUFFER_SIZE] = {0};
            bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
            if (bytesReceived < 0)
            {
                exitWithError("Failed to read bytes from client socket connection");
            }

            std::ostringstream ss;
            ss << "------ Received Request from client ------\n\n";
            log(ss.str());

            sendResponse();

            close(m_new_socket);
        }
    }

}