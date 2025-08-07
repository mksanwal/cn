#include <iostream>
#include <cstring>
#include <bitset>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

const char* generator = "10000111";

string xorStrings(string a, string b) {
    string result = "";
    for (size_t i = 0; i < b.size(); ++i)
        result += (a[i] == b[i]) ? '0' : '1';
    return result;
}

string mod2div(string data, string gen) {
    int genLen = gen.length();
    string temp = data.substr(0, genLen);

    for (int i = genLen; i < data.length(); ++i) {
        if (temp[0] == '1')
            temp = xorStrings(temp, gen);
        else
            temp = xorStrings(temp, string(genLen, '0'));

        temp = temp.substr(1) + data[i]; 
    }

    if (temp[0] == '1')
        temp = xorStrings(temp, gen);
    else
        temp = xorStrings(temp, string(genLen, '0'));

    return temp.substr(1);
}

bool isCodewordValid(const string& data) {
    string remainder = mod2div(data, generator);



    return remainder.find('1') == string::npos;
}


string binaryToText(const string& binary) {
    string text = "";
    for (size_t i = 0; i + 7 < binary.size(); i += 8) {
        bitset<8> byte(binary.substr(i, 8));
        text += char(byte.to_ulong());
    }
    return text;
}

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{}, clientAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5060);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSock, 1);
    cout << "Receiver is running on port 5060..." << endl;

    socklen_t clientSize = sizeof(clientAddr);
    int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientSize);

    uint32_t length = 0;
    recv(clientSock, &length, sizeof(length), 0);            

    char* buffer = new char[length + 1];                   
    recv(clientSock, buffer, length, 0);                     
    buffer[length] = '\0';                                   

string receivedCodeword(buffer);
delete[] buffer;

    cout << "Received Codeword: " << receivedCodeword << endl;

    if (isCodewordValid(receivedCodeword)) {
        cout << "Codeword is valid. No error detected." << endl;
        string binaryData = receivedCodeword.substr(0, receivedCodeword.size() - (strlen(generator) - 1));
        string originalText = binaryToText(binaryData);

        cout << "Decoded Binary (Original Message): " << binaryData << endl;
        cout << "Reconstructed Message: " << originalText << endl;
    } else {
        cout << "Codeword is invalid. Error detected during CRC check." << endl;
    }

    close(clientSock);
    close(serverSock);
    return 0;
}
