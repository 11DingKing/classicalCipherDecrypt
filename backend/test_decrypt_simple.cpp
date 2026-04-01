
#include <iostream>
#include <vector>
#include <string>
using namespace std;

const int N = 26;

int charToNum(char c) { return toupper(c) - 'A'; }
char numToChar(int n) { return 'A' + (((n % N) + N) % N; }

int det3(const vector<vector<int>>& M) {
    return M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
         - M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0])
         + M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
}

vector<vector<int>> adjugate3(const vector<vector<int>>& M) {
    vector<vector<int>> adj(3, vector<int>(3));
    adj[0][0] = M[1][1] * M[2][2] - M[1][2] * M[2][1];
    adj[0][1] = M[0][2] * M[2][1] - M[0][1] * M[2][2];
    adj[0][2] = M[0][1] * M[1][2] - M[0][2] * M[1][1];
    adj[1][0] = M[1][2] * M[2][0] - M[1][0] * M[2][2];
    adj[1][1] = M[0][0] * M[2][2] - M[0][2] * M[2][0];
    adj[1][2] = M[0][2] * M[1][0] - M[0][0] * M[1][2];
    adj[2][0] = M[1][0] * M[2][1] - M[1][1] * M[2][0];
    adj[2][1] = M[0][1] * M[2][0] - M[0][0] * M[2][1];
    adj[2][2] = M[0][0] * M[1][1] - M[0][1] * M[1][0];
    return adj;
}

int modInverse(int a, int m = N) {
    a = ((a % m) + m) % m;
    for (int x = 1; x < m; x++)
        if ((a * x) % m == 1) return x;
    return -1;
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

vector<vector<int>> mulMod26(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    vector<vector<int>> C(3, vector<int>(3, 0));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) C[i][j] += A[i][k] * B[k][j];
            C[i][j] = ((C[i][j] % N) + N) % N;
        }
    return C;
}

vector<vector<int>> transpose(const vector<vector<int>>& M) {
    vector<vector<int>> res(3, vector<int>(3));
    for (int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            res[i][j] = M[j][i];
    return res;
}

void printMatrix(const vector<vector<int>>& M, const string& name) {
    cout << name << ":" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  [";
        for (int j = 0; j < 3; j++)
            cout << (j ? ", " : "") << M[i][j];
        cout << "]" << endl;
    }
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

string hillDecryptAlt(const string& c1, const vector<vector<int>>& AInv) {
    string result;
    for (size_t i = 0; i + 3 <= c1.length(); i += 3) {
        int c[3] = {charToNum(c1[i]), charToNum(c1[i+1]), charToNum(c1[i+2])};
        for (int row = 0; row < 3; row++) {
            int p = 0;
            for (int col = 0; col < 3; col++)
                p = (p + c[col] * AInv[col][row]) % N;
            result += numToChar((p + N) % N);
        }
    }
    return result;
}

int main() {
    vector<vector<int>> A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };
    printMatrix(A, "A");

    vector<vector<int>> AInv;
    inverseMod26(A, AInv);
    printMatrix(AInv, "AInv");

    vector<vector<int>> AInvT = transpose(AInv);
    printMatrix(AInvT, "AInv 转置");

    string knownPlain = "CONFIDENTIALINFORMATION";
    string c1 = "LASYFRPXGLQKBFYABWIJMTRMEDFSJKUVHMVHWRGUQOYTBWTTUADJUONYQRXJDFFJIOTWKUUB";

    cout << "\n===== 测试解密 =====" << endl;
    cout << "已知明文前 9 字符: " << knownPlain.substr(0, 9) << endl;
    cout << "C1 前 9 字符: " << c1.substr(0, 9) << endl;

    string pt1 = hillDecrypt(c1.substr(0, 9), AInv);
    cout << "\n使用 AInv 解密（row * col）: " << pt1 << endl;

    string pt2 = hillDecryptAlt(c1.substr(0, 9), AInv);
    cout << "使用 AInv 解密（col * row）: " << pt2 << endl;

    string pt3 = hillDecrypt(c1.substr(0, 9), AInvT);
    cout << "使用 AInvT 解密（row * col）: " << pt3 << endl;

    string pt4 = hillDecryptAlt(c1.substr(0, 9), AInvT);
    cout << "使用 AInvT 解密（col * row）: " << pt4 << endl;

    return 0;
}
