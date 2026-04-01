
#include <iostream>
#include <vector>
#include <string>
using namespace std;

const int N = 26;

int charToNum(char c) {
    return c - 'A';
}
char numToChar(int n) {
    n = (n % N + N) % N;
    return n + 'A';
}

int main() {
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
    string knownPlain = "CONFIDENTIALINFORMATION";
    string C1 = "LASYFRPXGLQKBFYABWIJMTRMEDFSJKUVHMVHWRGUQOYTBWTTUADJUONYQRXJDFFJIOTWKUUB";

    cout << "=== 测试1: 验证加密 C = A * P ===" << endl;
    cout << "已知明文前 20 字符: " << knownPlain << endl;
    cout << "C1 前 24 字符: " << C1.substr(0,24) << endl;

    bool pass = true;
    for (int b = 0; b < 6; b++) {
        int p[3] = {
            charToNum(knownPlain[b*3+0]),
            charToNum(knownPlain[b*3+1]),
            charToNum(knownPlain[b*3+2])
        };
        int c[3] = {
            charToNum(C1[b*3+0]),
            charToNum(C1[b*3+1]),
            charToNum(C1[b*3+2])
        };
        int expected[3];
        for (int r = 0; r < 3; r++) {
            expected[r] = 0;
            for (int k = 0; k < 3; k++) {
                expected[r] += A[r][k] * p[k];
            }
            expected[r] = ((expected[r] % N) + N) % N;
        }
        cout << "块 " << b << ": P = [" << p[0] << "," << p[1] << "," << p[2] << "]"
             << ", 计算的 C = [" << expected[0] << "," << expected[1] << "," << expected[2] << "]"
             << ", 实际的 C = [" << c[0] << "," << c[1] << "," << c[2] << "]"
             << ", 匹配: " << (expected[0] == c[0] && expected[1] == c[1] && expected[2] == c[2] ? "是" : "否") << endl;
        if (!(expected[0] == c[0] && expected[1] == c[1] && expected[2] == c[2])) {
            pass = false;
        }
    }
    cout << "加密匹配: " << (pass ? "是" : "否") << endl << endl;

    cout << "=== 测试2: 验证解密 P = AInv * C ===" << endl;
    for (int b = 0; b < 6; b++) {
        int p[3] = {
            charToNum(knownPlain[b*3+0]),
            charToNum(knownPlain[b*3+1]),
            charToNum(knownPlain[b*3+2])
        };
        int c[3] = {
            charToNum(C1[b*3+0]),
            charToNum(C1[b*3+1]),
            charToNum(C1[b*3+2])
        };
        int decrypted[3];
        for (int r = 0; r < 3; r++) {
            decrypted[r] = 0;
            for (int col = 0; col < 3; col++) {
                decrypted[r] += AInv[r][col] * c[col];
            }
            decrypted[r] = ((decrypted[r] % N) + N) % N;
        }
        cout << "块 " << b << ": C = [" << c[0] << "," << c[1] << "," << c[2] << "]"
             << ", 解密的 P = [" << decrypted[0] << "," << decrypted[1] << "," << decrypted[2] << "]"
             << " (" << numToChar(decrypted[0]) << numToChar(decrypted[1]) << numToChar(decrypted[2]) << ")"
             << ", 期望 P = [" << p[0] << "," << p[1] << "," << p[2] << "]"
             << " (" << knownPlain.substr(b*3,3) << ")"
             << ", 匹配: " << (decrypted[0] == p[0] && decrypted[1] == p[1] && decrypted[2] == p[2] ? "是" : "否") << endl;
    }
    cout << endl;

    cout << "=== 测试3: 解密所有 C1 的块 ===" << endl;
    string decryptedText;
    for (int b = 0; b < (int)C1.size()/3; b++) {
        int c[3] = {
            charToNum(C1[b*3+0]),
            charToNum(C1[b*3+1]),
            charToNum(C1[b*3+2])
        };
        int decrypted[3];
        for (int r = 0; r < 3; r++) {
            decrypted[r] = 0;
            for (int col = 0; col < 3; col++) {
                decrypted[r] += AInv[r][col] * c[col];
            }
            decrypted[r] = ((decrypted[r] % N) + N) % N;
        }
        decryptedText += numToChar(decrypted[0]);
        decryptedText += numToChar(decrypted[1]);
        decryptedText += numToChar(decrypted[2]);
    }
    cout << "完整解密文本: " << decryptedText << endl;
    return 0;
}
