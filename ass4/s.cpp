#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctime>

#define PORT 5050
#define WINDOW_SIZE 4
#define TIMEOUT 2

using namespace std;

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

    int data[] = {10, 20, 30, 40, 50, 60, 70, 80};
    int dataSize = sizeof(data) / sizeof(data[0]);
    int base = 0, nextSeq = 0;
    bool acked[100] = {false};
    time_t timers[100];

    cout << "Go-Back-N Sender Started\n";

    while (base < dataSize) {
        while (nextSeq < base + WINDOW_SIZE && nextSeq < dataSize) {
            char buffer[1024];
            sprintf(buffer, "%d:%d", nextSeq, data[nextSeq]);
            sendto(sockfd, buffer, strlen(buffer), 0, (sockaddr*)&receiverAddr, addrLen);
            cout << "Sent: " << buffer << endl;
            timers[nextSeq] = time(0);
            nextSeq++;
        }

        char ackBuffer[1024];
        struct timeval tv = {TIMEOUT, 0};
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        int n = recvfrom(sockfd, ackBuffer, sizeof(ackBuffer), 0, (sockaddr*)&receiverAddr, &addrLen);
        if (n > 0) {
            ackBuffer[n] = '\0';
            int ackNum = atoi(ackBuffer);
            cout << "Received ACK: " << ackNum << endl;
            acked[ackNum] = true;

            while (base < dataSize && acked[base]) {
                base++;
            }
        } else {
            cout << "Timeout, resending window...\n";
            nextSeq = base;
        }
    }

    close(sockfd);
    cout << "All packets sent and acknowledged.\n";
    return 0;
}
