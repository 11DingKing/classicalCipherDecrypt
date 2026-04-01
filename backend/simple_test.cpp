
#include &lt;iostream&gt;
#include &lt;vector&gt;
using namespace std;

const int N = 26;

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

int modInverse(int a, int m = N) {
    a = ((a % m) + m) % m;
    for (int x = 1; x &lt; m; x++)
        if ((a * x) % m == 1) return x;
    return -1;
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
    vector&lt;vector&lt;int&gt;&gt; A = {
        {11, 2, 19},
        {3, 7, 4},
        {5, 8, 2}
    };
    printMatrix(A, "A");

    vector&lt;vector&lt;int&gt;&gt; AInv;
    inverseMod26(A, AInv);
    printMatrix(AInv, "AInv (原代码计算的)");

    vector&lt;vector&lt;int&gt;&gt; product = mulMod26(A, AInv);
    printMatrix(product, "A * AInv");

    cout &lt;&lt; endl &lt;&lt; "===== 现在计算正确的伴随矩阵和逆矩阵 =====" &lt;&lt; endl;

    vector&lt;vector&lt;int&gt;&gt; correctAdj(3, vector&lt;int&gt;(3));
    correctAdj[0][0] =  A[1][1] * A[2][2] - A[1][2] * A[2][1];
    correctAdj[0][1] = -(A[0][1] * A[2][2] - A[0][2] * A[2][1]);
    correctAdj[0][2] =  A[0][1] * A[1][2] - A[0][2] * A[1][1];
    correctAdj[1][0] = -(A[1][0] * A[2][2] - A[1][2] * A[2][0]);
    correctAdj[1][1] =  A[0][0] * A[2][2] - A[0][2] * A[2][0];
    correctAdj[1][2] = -(A[0][0] * A[1][2] - A[0][2] * A[1][0]);
    correctAdj[2][0] =  A[1][0] * A[2][1] - A[1][1] * A[2][0];
    correctAdj[2][1] = -(A[0][0] * A[2][1] - A[0][1] * A[2][0]);
    correctAdj[2][2] =  A[0][0] * A[1][1] - A[0][1] * A[1][0];

    printMatrix(correctAdj, "正确伴随矩阵（代数余子式的转置）");

    int det = ((det3(A) % N) + N) % N;
    int detInv = modInverse(det);

    vector&lt;vector&lt;int&gt;&gt; correctAInv(3, vector&lt;int&gt;(3));
    for (int i = 0; i &lt; 3; i++)
        for (int j = 0; j &lt; 3; j++)
            correctAInv[i][j] = (((correctAdj[i][j] % N) + N) % N * detInv) % N;

    printMatrix(correctAInv, "正确的逆矩阵 AInv'");

    vector&lt;vector&lt;int&gt;&gt; correctProduct = mulMod26(A, correctAInv);
    printMatrix(correctProduct, "A * AInv'");

    return 0;
}
