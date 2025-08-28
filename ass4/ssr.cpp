#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctime>
#include <vector>
#include <map>

#define PORT 5050
#define WINDOW_SIZE 4
#define TIMEOUT 2

using namespace std;

struct Packet {
    int seq;
    int value;
    bool acked;
    time_t lastSent;
};

int main() {
    int sockfd;
    sockaddr_in receiverAddr;
    socklen_t addrLen = sizeof(receiverAddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.31", &receiverAddr.sin_addr);

    int rawData[] = {5, 15, 25, 35, 45, 55, 65, 75};
    int dataSize = sizeof(rawData) / sizeof(rawData[0]);
    vector<Packet> packets;

    for (int i = 0; i < dataSize; i++) {
        packets.push_back({i, rawData[i], false, 0});
    }

    cout << "Selective Repeat Sender Started\n";

    int base = 0;

    while (base < dataSize) {
        time_t now = time(0);

        // Send packets in window
        for (int i = base; i < min(base + WINDOW_SIZE, dataSize); i++) {
            if (!packets[i].acked && (packets[i].lastSent == 0 || difftime(now, packets[i].lastSent) >= TIMEOUT)) {
                char buffer[1024];
                sprintf(buffer, "%d:%d", packets[i].seq, packets[i].value);
                sendto(sockfd, buffer, strlen(buffer), 0, (sockaddr*)&receiverAddr, addrLen);
                cout << "Sent packet: " << buffer << endl;
                packets[i].lastSent = now;
            }
        }

        // Try to receive ACK
        char ackBuffer[1024];
        struct timeval tv = {0, 100000}; // 100ms
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        int n = recvfrom(sockfd, ackBuffer, sizeof(ackBuffer), 0, (sockaddr*)&receiverAddr, &addrLen);
        if (n > 0) {
            ackBuffer[n] = '\0';
            int ackNum = atoi(ackBuffer);
            if (!packets[ackNum].acked) {
                packets[ackNum].acked = true;
                cout << "Received ACK for packet: " << ackNum << endl;
            }

            // Move base
            while (base < dataSize && packets[base].acked)
                base++;
        }
        usleep(100000); // Sleep 100ms
    }

    close(sockfd);
    cout << "All packets sent and acknowledged.\n";
    return 0;
}
