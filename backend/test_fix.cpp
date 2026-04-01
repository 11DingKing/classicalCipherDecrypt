
#include <iostream>
#include <vector>
using namespace std;

const int N = 26;

int det3(const vector<vector<int>>& M) {
    return M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
         - M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0])
         + M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
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

vector<vector<int>> adjugate3_correct(const vector<vector<int>>& M) {
    vector<vector<int>> adj(3, vector<int>(3));
    adj[0][0] =  M[1][1] * M[2][2] - M[1][2] * M[2][1];
    adj[0][1] = -(M[1][0] * M[2][2] - M[1][2] * M[2][0]);
    adj[0][2] =  M[1][0] * M[2][1] - M[1][1] * M[2][0];
    adj[1][0] = -(M[0][1] * M[2][2] - M[0][2] * M[2][1]);
    adj[1][1] =  M[0][0] * M[2][2] - M[0][2] * M[2][0];
    adj[1][2] = -(M[0][0] * M[2][1] - M[0][1] * M[2][0]);
    adj[2][0] =  M[0][1] * M[1][2] - M[0][2] * M[1][1];
    adj[2][1] = -(M[0][0] * M[1][2] - M[0][2] * M[1][0]);
    adj[2][2] =  M[0][0] * M[1][1] - M[0][1] * M[1][0];
    return adj;
}

int modInverse(int a, int m = N) {
    a = ((a % m) + m) % m;
    for (int x = 1; x < m; x++)
        if ((a * x) % m == 1) return x;
    return -1;
}

bool inverseMod26(const vector<vector<int>>& M, vector<vector<int>>& inv, bool useCorrect = false) {
    int det = ((det3(M) % N) + N) % N;
    int detInv = modInverse(det);
    if (detInv == -1) return false;

    vector<vector<int>> adj = useCorrect ? adjugate3_correct(M) : adjugate3_old(M);
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

void printMatrix(const vector<vector<int>>& M, const string& name) {
    cout << name << ":" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  [";
        for (int j = 0; j < 3; j++)
            cout << (j ? ", " : "") << M[i][j];
        cout << "]" << endl;
    }
}

int main() {
    vector<vector<int>> A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };
    printMatrix(A, "矩阵 A");

    vector<vector<int>> AInvOld;
    inverseMod26(A, AInvOld, false);
    printMatrix(AInvOld, "旧逆矩阵 AInv_old");

    vector<vector<int>> prodOld = mulMod26(A, AInvOld);
    printMatrix(prodOld, "A * AInv_old");

    cout << endl << "--- 使用正确伴随矩阵 ---" << endl;
    vector<vector<int>> AInvNew;
    inverseMod26(A, AInvNew, true);
    printMatrix(AInvNew, "新逆矩阵 AInv_new");

    vector<vector<int>> prodNew = mulMod26(A, AInvNew);
    printMatrix(prodNew, "A * AInv_new");

    return 0;
}
