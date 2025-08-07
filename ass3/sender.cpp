
#include <iostream>
#include <cstring>
#include <bitset>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const char* generator = "10000111";

string toBinary(const string& input) {
    string binary = "";
    for (char c : input) {
        binary += bitset<8>(c).to_string();
    }
    return binary;
}

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


string encodeData(string data) {
    int genSize = string(generator).length();
    string appended = data + string(genSize - 1, '0');
    string remainder = mod2div(appended, generator);
    return data + remainder;
}


void flipRandomBit(string& data) {
    int pos = rand() % data.size();
    cout << "Flipping bit at position: " << pos << endl;
    data[pos] = (data[pos] == '1') ? '0' : '1';
}

int main() {
    srand(time(0)); 

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5060);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.31");

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Connection Failed\n";
        return 1;
    }

    string input = "mnish";
    string binaryData = toBinary(input);
    string codeword = encodeData(binaryData);

    // Display everything
    cout << "Original Message: " << input << endl;
    cout << "Binary (Coded Binary): " << binaryData << endl;
    cout << "Encoded Codeword (Binary + CRC): " << codeword << endl;

    cout << "Send correct codeword? (y/n): ";
    char choice;
    cin >> choice;

    if (choice == 'n') {
        flipRandomBit(codeword);
        cout << "Modified (Error) Codeword: " << codeword << endl;
    }

    uint32_t length = codeword.size();
    send(sock, &length, sizeof(length), 0);            
    send(sock, codeword.c_str(), length, 0); 

    close(sock);
    return 0;
}
