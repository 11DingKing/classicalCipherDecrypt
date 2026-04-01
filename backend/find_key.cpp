#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
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

void generateKeys(int m, vector<string>& keys, string cur = "", int used = 0) {
    if ((int)cur.length() == m) { keys.push_back(cur); return; }
    for (int i = 0; i < 26; i++)
        if (!(used & (1 << i)))
            generateKeys(m, keys, cur + char('A' + i), used | (1 << i));
}

int main() {
    string KNOWN_PLAINTEXT = "CONFIDENTIALINFORMATION";
    string CIPHERTEXT_PREFIX = "EJBUJFNVHNZZVUXXEYCB";
    string FULL_CIPHERTEXT = "EJBUJFNVHNZZVUXXEYCBJMAYVIZVLKSDOCEWGEIGETXBNTDEQPEIWUKKDIFFEMZRBMOHDMIZ";

    vector<vector<int>> A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };

    string hill_cipher = hillEncrypt(KNOWN_PLAINTEXT.substr(0, 18), A);
    cout << "Hill加密结果: " << hill_cipher << endl << endl;

    vector<string> keys;
    generateKeys(3, keys);
    cout << "枚举 " << keys.size() << " 个3字母无重复密钥..." << endl << endl;

    int found = 0;
    for (const string& key : keys) {
        vector<int> perm = keyToPermutation(key);
        string vigenere_cipher = vigenereEncrypt(hill_cipher, key);
        string final_cipher = columnarEncrypt(vigenere_cipher, 3, perm);

        if (final_cipher == CIPHERTEXT_PREFIX.substr(0, 18)) {
            cout << "找到匹配密钥: " << key << endl;
            cout << "  置换规则: [";
            for (int i = 0; i < 3; i++) cout << (i ? ", " : "") << perm[i];
            cout << "]" << endl;
            cout << "  加密结果: " << final_cipher << endl;
            cout << "  题目密文: " << CIPHERTEXT_PREFIX.substr(0, 18) << endl;

            // 现在解密完整的密文看看
            string c2 = reverseColumnar(FULL_CIPHERTEXT, 3, perm);
            string c1 = vigenereDecrypt(c2, key);
            cout << "  C1 (Hill密文): " << c1.substr(0, 30) << "..." << endl << endl;
            found++;
        }
    }

    cout << "共找到 " << found << " 个匹配密钥" << endl;
    return 0;
}
