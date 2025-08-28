#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <ctime>

#define PORT 5050

using namespace std;

int main() {
    srand(time(0));
    int sockfd;
    sockaddr_in receiverAddr, senderAddr;
    socklen_t addrLen = sizeof(senderAddr);
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.31", &receiverAddr.sin_addr);

    if (bind(sockfd, (sockaddr*)&receiverAddr, sizeof(receiverAddr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    int expected = 0;
    cout << "Go-Back-N Receiver started\n";

    while (true) {
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &addrLen);
        if (n <= 0) continue;
        buffer[n] = '\0';

        int seq, val;
        sscanf(buffer, "%d:%d", &seq, &val);

        if (rand() % 10 < 2) {
            cout << "Simulated ACK loss for packet " << seq << endl;
            continue;
        }

        if (seq == expected) {
            cout << "Received in order: " << seq << " -> " << val << endl;
            sendto(sockfd, to_string(seq).c_str(), to_string(seq).length(), 0, (sockaddr*)&senderAddr, addrLen);
            expected++;
        } else {
            cout << "Out of order packet: " << seq << ", Expected: " << expected << endl;
            int lastAck = expected - 1;
            sendto(sockfd, to_string(lastAck).c_str(), to_string(lastAck).length(), 0, (sockaddr*)&senderAddr, addrLen);
        }
    }

    close(sockfd);
    return 0;
}
