#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main() {
    // Create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Socket creation error\n";
        return -1;
    }

    // Define server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        cerr << "Invalid address\n";
        return -1;
    }

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Connection failed\n";
        close(clientSocket);
        return -1;
    } else {
        std::cout << "Connected to server\n";
    }

    // Send device id to control
    cout << "Enter device ID to control (1-6): "<<endl;
    cout << "1.Lamp"<<endl;
    cout << "2.Fan"<<endl;
    cout << "3.TV"<<endl;
    cout << "4.Refridgerator"<<endl;
    cout << "5.AC"<<endl;
    cout << "6.Washing_machine"<<endl;
    
    string input;
    cin >> input;

    // Validate input
    if (input.size() != 1 || !isdigit(input[0])) {
        cerr << "Invalid input. Please enter a single digit (1-6).\n";
        close(clientSocket);
        return -1;
    }

    // Send device ID to server
    int bytesSent = send(clientSocket, input.c_str(), input.size(), 0);
    if (bytesSent == -1) {
        cerr << "Error sending data to server\n";
        close(clientSocket);
        return -1;
    } else {
        cout << "Sent data to server: " << input << std::endl;
    }

    // Receive response from server
    char buffer[1024] = {0};
    int valread = read(clientSocket, buffer, 1024);
    if (valread == -1) {
        cerr << "Error reading data from server\n";
        close(clientSocket);
        return -1;
    } else if (valread == 0) {
        cerr << "Server closed the connection\n";
        close(clientSocket);
        return -1;
    } else {
        cout << "Server response: " << buffer << endl;
    }


    // Close socket
    close(clientSocket);

    return 0;
}

