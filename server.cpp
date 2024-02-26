#include <iostream>
#include <string>
#include <map>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

// Define constants for devices
enum DeviceType {
    LAMP,
    FAN,
    TV,
    REFRIGERATOR,
    AIR_COOLER,
    WASHING_MACHINE
};

// Define device statuses
enum DeviceStatus {
    OFF,
    ON
};

// Define a structure for devices
struct Device {
    DeviceType type;
    DeviceStatus status;
};

// Map to store devices in the home
std::map<int, Device> devices;

// Function to initialize devices in the home
void initializeDevices() {
    devices[1] = {LAMP, OFF};
    devices[2] = {FAN, OFF};
    devices[3] = {TV, OFF};
    devices[4] = {REFRIGERATOR, OFF};
    devices[5] = {AIR_COOLER, OFF};
    devices[6] = {WASHING_MACHINE, OFF};
}

// Function to handle client requests
void handleClient(int clientSocket) {
    // Receive data from client
    char buffer[1024] = {0};
    int valread = read(clientSocket, buffer, 1024);
    if (valread <= 0) {
        cerr << "Error reading from client\n";
        close(clientSocket);
        return;
    }
    
    string message(buffer);

    // Print received message for debugging
    cout << "Received message from client: " << message << std::endl;

    // Parse message from client
    int deviceId;
    try {
        deviceId = stoi(message);
    } catch (const invalid_argument& e) {
        cerr << "Invalid device ID received\n";
        close(clientSocket);
        return;
    }

    // Check if the device exists
    if (devices.find(deviceId) != devices.end()) {
        // Toggle device status
        devices[deviceId].status = (devices[deviceId].status == ON) ? OFF : ON;

        // Send response to client
        string response = (devices[deviceId].status == ON) ?"Device turned ON" : "Device turned OFF";
        int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
        if (bytesSent == -1) {
            cerr << "Error sending response to client\n";
        }
    } else {
        string response = "Device not found";
        int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
        if (bytesSent == -1) {
            cerr << "Error sending response to client\n";
        }
    }

    // Close socket
    close(clientSocket);
}

int main() {
    // Initialize devices
    initializeDevices();

    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Socket creation error\n";
        return -1;
    }

    // Define server address
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        std::cerr << "Bind failed\n";
        return -1;
    }

    // Listen for clients
    if (listen(serverSocket, 3) == -1) {
        std::cerr << "Listen failed\n";
        return -1;
    }

    // Accept incoming connections
    while (true) {
        int clientSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (clientSocket == -1) {
            std::cerr << "Accept failed\n";
            return -1;
        }

        // Handle client request
        handleClient(clientSocket);
    }

    return 0;
}
