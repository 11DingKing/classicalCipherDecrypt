
#include &lt;iostream&gt;
#include &lt;vector&gt;
#include &lt;string&gt;
#include &lt;cstdlib&gt;
#include &lt;ctime&gt;

using namespace std;

const int N = 26;

int charToNum(char c) { return toupper(c) - 'A'; }
char numToChar(int n) { return 'A' + (((n % N) + N) % N); }

int modInverse(int a, int m = N) {
    a = ((a % m) + m) % m;
    for (int x = 1; x &lt; m; x++)
        if ((a * x) % m == 1) return x;
    return -1;
}

int det3(const vector&lt;vector&lt;int&gt;&gt;&amp; M) {
    return M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
         - M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0])
         + M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
}

vector&lt;vector&lt;int&gt;&gt; adjugate3(const vector&lt;vector&lt;int&gt;&gt;&amp; M) {
    vector&lt;vector&lt;int&gt;&gt; adj(3, vector&lt;int&gt;(3));
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

vector&lt;vector&lt;int&gt;&gt; adjugate3_correct(const vector&lt;vector&lt;int&gt;&gt;&amp; M) {
    vector&lt;vector&lt;int&gt;&gt; adj(3, vector&lt;int&gt;(3));
    // 正确的伴随矩阵：伴随矩阵是代数余子式矩阵的转置
    // 代数余子式 C_ij = (-1)^(i+j) * 余子式 M_ij
    adj[0][0] =  M[1][1] * M[2][2] - M[1][2] * M[2][1];  // +
    adj[0][1] = -(M[1][0] * M[2][2] - M[1][2] * M[2][0]); // -
    adj[0][2] =  M[1][0] * M[2][1] - M[1][1] * M[2][0];  // +
    adj[1][0] = -(M[0][1] * M[2][2] - M[0][2] * M[2][1]); // -
    adj[1][1] =  M[0][0] * M[2][2] - M[0][2] * M[2][0];  // +
    adj[1][2] = -(M[0][0] * M[2][1] - M[0][1] * M[2][0]); // -
    adj[2][0] =  M[0][1] * M[1][2] - M[0][2] * M[1][1];  // +
    adj[2][1] = -(M[0][0] * M[1][2] - M[0][2] * M[1][0]); // -
    adj[2][2] =  M[0][0] * M[1][1] - M[0][1] * M[1][0];  // +
    return adj;
}

bool inverseMod26(const vector&lt;vector&lt;int&gt;&gt;&amp; M, vector&lt;vector&lt;int&gt;&gt;&amp; inv) {
    int det = ((det3(M) % N) + N) % N;
    int detInv = modInverse(det);
    if (detInv == -1) return false;

    vector&lt;vector&lt;int&gt;&gt; adj = adjugate3(M);
    inv.assign(3, vector&lt;int&gt;(3));
    for (int i = 0; i &lt; 3; i++)
        for (int j = 0; j &lt; 3; j++)
            inv[i][j] = (((adj[i][j] % N) + N) % N * detInv) % N;
    return true;
}

vector&lt;vector&lt;int&gt;&gt; mulMod26(const vector&lt;vector&lt;int&gt;&gt;&amp; A, const vector&lt;vector&lt;int&gt;&gt;&amp; B) {
    vector&lt;vector&lt;int&gt;&gt; C(3, vector&lt;int&gt;(3, 0));
    for (int i = 0; i &lt; 3; i++)
        for (int j = 0; j &lt; 3; j++) {
            for (int k = 0; k &lt; 3; k++) C[i][j] += A[i][k] * B[k][j];
            C[i][j] = ((C[i][j] % N) + N) % N;
        }
    return C;
}

void printMatrix(const vector&lt;vector&lt;int&gt;&gt;&amp; M, const string&amp; name) {
    cout &lt;&lt; name &lt;&lt; ":" &lt;&lt; endl;
    for (int i = 0; i &lt; 3; i++) {
        cout &lt;&lt; "  [";
        for (int j = 0; j &lt; 3; j++)
            cout &lt;&lt; (j ? ", " : "") &lt;&lt; M[i][j];
        cout &lt;&lt; "]" &lt;&lt; endl;
    }
}

int main() {
    // 测试用程序中找到的密钥矩阵
    vector&lt;vector&lt;int&gt;&gt; A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };

    printMatrix(A, "原始矩阵 A");
    cout &lt;&lt; endl;

    vector&lt;vector&lt;int&gt;&gt; adj_old = adjugate3(A);
    printMatrix(adj_old, "旧伴随矩阵");
    cout &lt;&lt; endl;

    vector&lt;vector&lt;int&gt;&gt; adj_new = adjugate3_correct(A);
    printMatrix(adj_new, "正确伴随矩阵");
    cout &lt;&lt; endl;

    cout &lt;&lt; "===== 测试伴随矩阵对比 =====" &lt;&lt; endl;
    return 0;
}
