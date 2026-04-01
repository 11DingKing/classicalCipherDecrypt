#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

const int N = 26;

int charToNum(char c) { return toupper(c) - 'A'; }
char numToChar(int n) { return 'A' + (((n % N) + N) % N); }

vector<int> keyToPermutation(const string& key) {
    vector<int> idx(key.size());
    for (size_t i = 0; i < key.size(); i++) idx[i] = i;
    sort(idx.begin(), idx.end(), [&key](int a, int b) { return key[a] < key[b]; });
    return idx;
}

string columnarEncrypt(const string& plaintext, int m, const vector<int>& colOrder) {
    int len = plaintext.length();
    int rows = (len + m - 1) / m;

    vector<string> cols(m);
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < m; c++)
            if (r * m + c < (int)plaintext.length())
                cols[c] += plaintext[r * m + c];

    string result;
    for (int i = 0; i < m; i++) {
        int c = colOrder[i];
        result += cols[c];
    }
    return result;
}

string reverseColumnar(const string& ciphertext, int m, const vector<int>& colOrder) {
    int len = ciphertext.length();
    int rows = (len + m - 1) / m;
    int remainder = len % m;

    vector<int> colLen(m, rows);
    if (remainder > 0)
        for (int i = remainder; i < m; i++) colLen[i] = rows - 1;

    vector<string> cols(m);
    int pos = 0;
    for (int i = 0; i < m; i++) {
        int c = colOrder[i];
        for (int j = 0; j < colLen[c]; j++)
            cols[c] += ciphertext[pos++];
    }

    string result;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < m; c++)
            if (r < (int)cols[c].size()) result += cols[c][r];
    return result;
}

string vigenereEncrypt(const string& plaintext, const string& key) {
    string result;
    for (size_t i = 0; i < plaintext.length(); i++)
        result += numToChar(charToNum(plaintext[i]) + charToNum(key[i % key.length()]));
    return result;
}

string vigenereDecrypt(const string& ciphertext, const string& key) {
    string result;
    for (size_t i = 0; i < ciphertext.length(); i++)
        result += numToChar(charToNum(ciphertext[i]) - charToNum(key[i % key.length()]));
    return result;
}

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

int main() {
    string KNOWN_PLAINTEXT = "CONFIDENTIALINFORMATION";
    string CIPHERTEXT_PREFIX = "EJBUJFNVHNZZVUXXEYCB";

    cout << "题目给出的条件:" << endl;
    cout << "  明文前20: " << KNOWN_PLAINTEXT.substr(0, 20) << endl;
    cout << "  密文前20: " << CIPHERTEXT_PREFIX << endl << endl;

    vector<vector<int>> A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };

    string key = "KEY";
    vector<int> perm = keyToPermutation(key);

    cout << "使用密钥 KEY 进行加密测试:" << endl;
    cout << "置换规则: [";
    for (int i = 0; i < 3; i++) cout << (i ? ", " : "") << perm[i];
    cout << "]" << endl << endl;

    // 加密流程: 明文 -> Hill -> 维吉尼亚 -> 列置换
    string hill_cipher = hillEncrypt(KNOWN_PLAINTEXT.substr(0, 18), A);
    cout << "1. Hill加密后: " << hill_cipher << endl;

    string vigenere_cipher = vigenereEncrypt(hill_cipher, key);
    cout << "2. 维吉尼亚加密后: " << vigenere_cipher << endl;

    string final_cipher = columnarEncrypt(vigenere_cipher, 3, perm);
    cout << "3. 列置换后: " << final_cipher << endl;
    cout << "题目密文前20: " << CIPHERTEXT_PREFIX << endl;
    cout << "是否匹配? " << (final_cipher == CIPHERTEXT_PREFIX ? "是" : "否") << endl << endl;

    // 现在解密回来看看
    cout << "现在解密回来验证:" << endl;
    string dec1 = reverseColumnar(final_cipher, 3, perm);
    cout << "1. 逆列置换: " << dec1 << endl;

    string dec2 = vigenereDecrypt(dec1, key);
    cout << "2. 维吉尼亚解密: " << dec2 << endl;

    cout << "Hill加密前: " << hill_cipher << endl;
    cout << "是否匹配? " << (dec2 == hill_cipher ? "是" : "否") << endl;

    return 0;
}
