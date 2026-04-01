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

int det3(const vector<vector<int>>& M) {
    return M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
         - M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0])
         + M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
}

int modInverse(int a, int m = N) {
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

bool inverseMod26(const vector<vector<int>>& M, vector<vector<int>>& inv) {
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

int main() {
    string plaintext = "CONFIDENTIALINFORMATION";

    vector<vector<int>> A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };

    vector<vector<int>> AInv;
    inverseMod26(A, AInv);

    cout << "原始明文: " << plaintext.substr(0, 18) << endl;

    string encrypted = hillEncrypt(plaintext.substr(0, 18), A);
    cout << "加密后: " << encrypted << endl;

    string decrypted = hillDecrypt(encrypted, AInv);
    cout << "解密后: " << decrypted << endl;

    cout << "是否一致? " << (decrypted == plaintext.substr(0, 18) ? "是" : "否") << endl;

    return 0;
}
