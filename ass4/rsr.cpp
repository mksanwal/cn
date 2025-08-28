#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
#include <cstdlib>
#include <ctime>

#define PORT 5050
#define WINDOW_SIZE 4

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

    int expectedBase = 0;
    map<int, int> receivedData;

    cout << "Selective Repeat Receiver started\n";

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

        if (seq >= expectedBase && seq < expectedBase + WINDOW_SIZE) {
            if (receivedData.find(seq) == receivedData.end()) {
                receivedData[seq] = val;
                cout << "Received packet " << seq << " -> " << val << endl;
            } else {
                cout << "Duplicate packet " << seq << " ignored\n";
            }
            // Send ACK
            sendto(sockfd, to_string(seq).c_str(), to_string(seq).length(), 0, (sockaddr*)&senderAddr, addrLen);

            // Slide window
            while (receivedData.find(expectedBase) != receivedData.end()) {
                expectedBase++;
            }
        } else {
            cout << "Out of window packet " << seq << " discarded\n";
        }
    }

    close(sockfd);
    return 0;
}
