#include <iostream>
#include <vector>
#include <string>
using namespace std;

const int N = 26;

int charToNum(char c) { return toupper(c) - 'A'; }
char numToChar(int n) { return 'A' + (((n % N) + N) % N); }

vector<vector<int>> mulMod26(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    vector<vector<int>> C(3, vector<int>(3, 0));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) C[i][j] += A[i][k] * B[k][j];
            C[i][j] = ((C[i][j] % N) + N) % N;
        }
    return C;
}

string hillEncrypt(const string& plaintext, const vector<vector<int>>& A) {
    string result;
    for (size_t i = 0; i + 3 <= plaintext.length(); i += 3) {
        int p[3] = {charToNum(plaintext[i]), charToNum(plaintext[i+1]), charToNum(plaintext[i+2])};
        for (int row = 0; row < 3; row++) {
            int c = 0;
            for (int col = 0; col < 3; col++)
                c = (c + A[row][col] * p[col]) % N;
            result += numToChar((c + N) % N);
        }
    }
    return result;
}

string hillDecrypt(const string& c1, const vector<vector<int>>& AInv) {
    string result;
    for (size_t i = 0; i + 3 <= c1.length(); i += 3) {
        int c[3] = {charToNum(c1[i]), charToNum(c1[i+1]), charToNum(c1[i+2])};
        for (int row = 0; row < 3; row++) {
            int p = 0;
            for (int col = 0; col < 3; col++)
                p = (p + AInv[row][col] * c[col]) % N;
            result += numToChar((p + N) % N);
        }
    }
    return result;
}

int main() {
    string KNOWN_PLAINTEXT = "CONFIDENTIALINFORMATION";
    string c1 = "LASYFRPXGLQKBFYABWIJMTRMEDFSJKUVHMVHWRGUQOYTBWTTUADJUONYQRXJDFFJIOTWKUUB";

    vector<vector<int>> A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };

    vector<vector<int>> AInv = {
        {22, 4, 17},
        {6, 17, 13},
        {25, 0, 23}
    };

    cout << "已知明文: " << KNOWN_PLAINTEXT << endl;
    cout << "C1: " << c1 << endl << endl;

    cout << "=== 用A加密前18字符明文 ===" << endl;
    string encrypted = hillEncrypt(KNOWN_PLAINTEXT.substr(0, 18), A);
    cout << "加密结果: " << encrypted << endl;
    cout << "C1前18字符: " << c1.substr(0, 18) << endl;
    cout << "是否匹配? " << (encrypted == c1.substr(0, 18) ? "是" : "否") << endl << endl;

    cout << "=== 用AInv解密C1前18字符 ===" << endl;
    string decrypted = hillDecrypt(c1.substr(0, 18), AInv);
    cout << "解密结果: " << decrypted << endl;
    cout << "已知明文前18: " << KNOWN_PLAINTEXT.substr(0, 18) << endl;
    cout << "是否匹配? " << (decrypted == KNOWN_PLAINTEXT.substr(0, 18) ? "是" : "否") << endl << endl;

    cout << "=== 现在解密整个C1 ===" << endl;
    string full_decrypted = hillDecrypt(c1, AInv);
    cout << "完整解密: " << full_decrypted << endl;

    return 0;
}
