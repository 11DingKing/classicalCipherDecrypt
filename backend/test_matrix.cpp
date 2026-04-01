#include <iostream>
#include <vector>
using namespace std;

const int N = 26;

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

vector<vector<int>> adjugate3_old(const vector<vector<int>>& M) {
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

vector<vector<int>> adjugate3_new(const vector<vector<int>>& M) {
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

void printMatrix(const vector<vector<int>>& M, string name) {
    cout << name << ":" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  [";
        for (int j = 0; j < 3; j++) cout << (j ? ", " : "") << M[i][j];
        cout << "]" << endl;
    }
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

bool inverseMod26(const vector<vector<int>>& M, vector<vector<int>>& inv, bool use_new) {
    int det = ((det3(M) % N) + N) % N;
    int detInv = modInverse(det);
    if (detInv == -1) return false;

    vector<vector<int>> adj = use_new ? adjugate3_new(M) : adjugate3_old(M);
    inv.assign(3, vector<int>(3));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            inv[i][j] = (((adj[i][j] % N) + N) % N * detInv) % N;
    return true;
}

int main() {
    vector<vector<int>> A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };

    printMatrix(A, "A");
    cout << "det(A) = " << det3(A) << endl;
    cout << "det(A) mod 26 = " << ((det3(A) % N) + N) % N << endl;

    vector<vector<int>> inv_old, inv_new;
    inverseMod26(A, inv_old, false);
    inverseMod26(A, inv_new, true);

    printMatrix(inv_old, "旧方法求逆");
    printMatrix(inv_new, "新方法求逆");

    cout << "\n=== 验证 A × A^-1 (旧方法) ===" << endl;
    printMatrix(mulMod26(A, inv_old), "A × inv_old");

    cout << "\n=== 验证 A × A^-1 (新方法) ===" << endl;
    printMatrix(mulMod26(A, inv_new), "A × inv_new");

    return 0;
}
