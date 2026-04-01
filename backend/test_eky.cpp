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
    string FULL_CIPHERTEXT = "EJBUJFNVHNZZVUXXEYCBJMAYVIZVLKSDOCEWGEIGETXBNTDEQPEIWUKKDIFFEMZRBMOHDMIZ";

    vector<vector<int>> A = {
        {3, 7, 4},
        {11, 2, 19},
        {5, 8, 2}
    };

    string key = "EKY";
    vector<int> perm = keyToPermutation(key);

    cout << "使用密钥 EKY 进行加密测试:" << endl;
    cout << "置换规则: [";
    for (int i = 0; i < 3; i++) cout << (i ? ", " : "") << perm[i];
    cout << "]" << endl << endl;

    // 现在解密完整的密文，然后再加密回去验证
    cout << "=== 解密流程 ===" << endl;
    string c2 = reverseColumnar(FULL_CIPHERTEXT, 3, perm);
    cout << "1. 逆列置换: " << c2 << endl;

    string c1 = vigenereDecrypt(c2, key);
    cout << "2. 维吉尼亚解密: " << c1 << endl << endl;

    cout << "=== 现在用A矩阵解密Hill密文 ===" << endl;
    vector<vector<int>> AInv;
    extern bool inverseMod26(const vector<vector<int>>& M, vector<vector<int>>& inv);
    inverseMod26(A, AInv);

    cout << "A矩阵:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  [";
        for (int j = 0; j < 3; j++) cout << (j ? ", " : "") << A[i][j];
        cout << "]" << endl;
    }
    cout << endl;

    cout << "A逆矩阵:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  [";
        for (int j = 0; j < 3; j++) cout << (j ? ", " : "") << AInv[i][j];
        cout << "]" << endl;
    }
    cout << endl;

    extern string hillDecrypt(const string& c1, const vector<vector<int>>& AInv);
    string plaintext = hillDecrypt(c1, AInv);
    cout << "解密明文: " << plaintext << endl;

    return 0;
}

// 从原程序中复制的函数
bool inverseMod26(const vector<vector<int>>& M, vector<vector<int>>& inv) {
    extern int det3(const vector<vector<int>>& M);
    extern int modInverse(int a, int m);
    extern vector<vector<int>> adjugate3(const vector<vector<int>>& M);

    int det = ((det3(M) % N) + N) % N;
    int detInv = modInverse(det);
    if (detInv == -1) return false;

    vector<vector<int>> adj = adjugate3(M);
    inv.assign(3, vector<int>(3));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            inv[i][j] = (((adj[i][j] % N) + N) % N * detInv) % N;
    return true;
}

int det3(const vector<vector<int>>& M) {
    return M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
         - M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0])
         + M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
}

int modInverse(int a, int m) {
    a = ((a % m) + m) % m;
    for (int x = 1; x < m; x++)
        if ((a * x) % m == 1) return x;
    return -1;
}

vector<vector<int>> adjugate3(const vector<vector<int>>& M) {
    vector<vector<int>> cof(3, vector<int>(3));
    cof[0][0] =  (M[1][1] * M[2][2] - M[1][2] * M[2][1]);
    cof[0][1] = -(M[1][0] * M[2][2] - M[1][2] * M[2][0]);
    cof[0][2] =  (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
    cof[1][0] = -(M[0][1] * M[2][2] - M[0][2] * M[2][1]);
    cof[1][1] =  (M[0][0] * M[2][2] - M[0][2] * M[2][0]);
    cof[1][2] = -(M[0][0] * M[2][1] - M[0][1] * M[2][0]);
    cof[2][0] =  (M[0][1] * M[1][2] - M[0][2] * M[1][1]);
    cof[2][1] = -(M[0][0] * M[1][2] - M[0][2] * M[1][0]);
    cof[2][2] =  (M[0][0] * M[1][1] - M[0][1] * M[1][0]);

    vector<vector<int>> adj(3, vector<int>(3));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            adj[i][j] = cof[j][i];
    return adj;
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
