#include <iostream>
#include <vector>
#include <string>
using namespace std;

const int N = 26;

int charToNum(char c) { return toupper(c) - 'A'; }
char numToChar(int n) { return 'A' + (((n % N) + N) % N); }

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

vector<vector<int>> mulMod26(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    vector<vector<int>> C(3, vector<int>(3, 0));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) C[i][j] += A[i][k] * B[k][j];
            C[i][j] = ((C[i][j] % N) + N) % N;
        }
    return C;
}

void printMatrix(const vector<vector<int>>& M, string name) {
    cout << name << ":" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  [";
        for (int j = 0; j < 3; j++) cout << (j ? ", " : "") << M[i][j];
        cout << "]" << endl;
    }
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

bool solveHillMatrix(const string& plaintext, const string& c1,
                     vector<vector<int>>& A, vector<vector<int>>& AInv) {
    int numBlocks = min((int)plaintext.length(), (int)c1.length()) / 3;
    if (numBlocks < 3) return false;
    numBlocks = min(numBlocks, 6);

    vector<vector<int>> Pmat(numBlocks, vector<int>(3)), Cmat(numBlocks, vector<int>(3));
    for (int i = 0; i < numBlocks; i++)
        for (int j = 0; j < 3; j++) {
            Pmat[i][j] = charToNum(plaintext[i * 3 + j]);
            Cmat[i][j] = charToNum(c1[i * 3 + j]);
        }

    for (int i0 = 0; i0 < numBlocks; i0++)
        for (int i1 = i0 + 1; i1 < numBlocks; i1++)
            for (int i2 = i1 + 1; i2 < numBlocks; i2++) {
                int idx[3] = {i0, i1, i2};
                vector<vector<int>> P(3, vector<int>(3)), C(3, vector<int>(3));
                for (int r = 0; r < 3; r++)
                    for (int c = 0; c < 3; c++) {
                        P[r][c] = Pmat[idx[c]][r];
                        C[r][c] = Cmat[idx[c]][r];
                    }

                cout << "使用分组: " << i0 << ", " << i1 << ", " << i2 << endl;
                printMatrix(P, "P");
                printMatrix(C, "C");

                vector<vector<int>> PInv;
                if (!inverseMod26(P, PInv)) continue;
                printMatrix(PInv, "PInv");

                A = mulMod26(C, PInv);
                printMatrix(A, "A = C * PInv");

                if (!inverseMod26(A, AInv)) continue;
                return true;
            }
    return false;
}

int main() {
    string plaintext = "CONFIDENTIALINFORMATION";

    vector<vector<int>> A_true = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };

    printMatrix(A_true, "真实A矩阵");

    string c1 = hillEncrypt(plaintext.substr(0, 18), A_true);
    cout << "加密后的C1: " << c1 << endl << endl;

    vector<vector<int>> A, AInv;
    solveHillMatrix(plaintext.substr(0, 18), c1, A, AInv);

    cout << endl << "求解得到的A矩阵是否等于真实A矩阵? " << (A == A_true ? "是" : "否") << endl;

    return 0;
}
