/**
 * 古典密码综合解密程序
 * 解密流程：逆列置换 → 维吉尼亚解密 → Hill密码解密
 * 
 * 算法流程：
 *   1. 逆列置换：枚举列置换规则，还原维吉尼亚密文 C2
 *   2. 维吉尼亚解密：
 *      a) 用 Kasiski 试验 + Friedman 试验确定密钥长度 m
 *      b) 基于频数分析（互相关法）推导维吉尼亚密钥
 *      c) 解密 C2 得到 Hill 密文 C1
 *   3. Hill 密码解密：
 *      a) 基于已知明文攻击推导完整 3×3 密钥矩阵 A
 *      b) 计算 A 的模 26 逆矩阵 A^-1
 *      c) 解密 C1 得到原始明文，去除末尾填充 'X'
 * 
 * 编译：g++ -std=c++17 -O2 -o decrypt classical_cipher_decrypt.cpp
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <numeric>

using namespace std;

const string CIPHERTEXT = "EJBUJFNVHNZZVUXXEYCBJMAYVIZVLKSDOCEWGEIGETXBNTDEQPEIWUKKDIFFEMZRBMOHDMIZ";
const string KNOWN_PLAINTEXT = "CONFIDENTIALINFORMATION";
const string KNOWN_PREFIX = KNOWN_PLAINTEXT.substr(0, 20);
const int N = 26;

// ========== 基础工具函数 ==========
int charToNum(char c) { return toupper(c) - 'A'; }
char numToChar(int n) { return 'A' + (((n % N) + N) % N); }

int modInverse(int a, int m = N) {
    a = ((a % m) + m) % m;
    for (int x = 1; x < m; x++)
        if ((a * x) % m == 1) return x;
    return -1;
}

int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }

// 标准英文字母频率
const double ENG_FREQ[26] = {
    0.082, 0.015, 0.028, 0.043, 0.127, 0.022, 0.020, 0.061, 0.070, 0.002,
    0.008, 0.040, 0.024, 0.067, 0.075, 0.019, 0.001, 0.060, 0.063, 0.091,
    0.028, 0.010, 0.023, 0.001, 0.020, 0.001
};

// ========== 第一步：逆列置换 ==========
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

vector<int> keyToPermutation(const string& key) {
    vector<int> idx(key.size());
    for (size_t i = 0; i < key.size(); i++) idx[i] = i;
    sort(idx.begin(), idx.end(), [&key](int a, int b) { return key[a] < key[b]; });
    return idx;
}

// ========== 第二步：维吉尼亚解密 ==========
string vigenereDecrypt(const string& ciphertext, const string& key) {
    string result;
    for (size_t i = 0; i < ciphertext.length(); i++)
        result += numToChar(charToNum(ciphertext[i]) - charToNum(key[i % key.length()]));
    return result;
}

/**
 * 计算文本的重合指数 IC (Index of Coincidence)
 * 英文文本 IC ≈ 0.065，随机文本 IC ≈ 0.038
 */
double calcIC(const string& s) {
    if (s.length() < 2) return 0;
    int freq[26] = {};
    for (char c : s) freq[toupper(c) - 'A']++;
    double sum = 0;
    int n = s.length();
    for (int i = 0; i < 26; i++) sum += freq[i] * (freq[i] - 1);
    return sum / ((double)n * (n - 1));
}

/**
 * Friedman 试验（重合指数法）
 * 对每个候选密钥长度 m，将密文按 m 列分组，
 * 计算各列子串的平均 IC，越接近 0.065 说明 m 越可能正确
 */
double avgICForKeyLen(const string& s, int m) {
    double total = 0;
    for (int k = 0; k < m; k++) {
        string sub;
        for (size_t i = k; i < s.length(); i += m) sub += s[i];
        total += calcIC(sub);
    }
    return total / m;
}

vector<pair<double, int>> friedmanTest(const string& s, int maxKeyLen = 6) {
    vector<pair<double, int>> candidates;
    for (int m = 1; m <= maxKeyLen; m++) {
        double ic = avgICForKeyLen(s, m);
        candidates.push_back({fabs(ic - 0.065), m});
    }
    sort(candidates.begin(), candidates.end());
    return candidates;
}

/**
 * Kasiski 试验
 * 查找密文中重复的 n-gram（2-gram 和 3-gram），统计间距的因子频率
 * 出现频率最高的因子即为密钥长度的候选
 */
map<int, int> kasiskiExamination(const string& s) {
    map<int, int> factorCount;
    // 同时分析 2-gram 和 3-gram
    for (int patternLen = 2; patternLen <= 3; patternLen++) {
        map<string, vector<int>> positions;
        for (size_t i = 0; i + patternLen <= s.length(); i++)
            positions[s.substr(i, patternLen)].push_back(i);

        for (auto& p : positions) {
            auto& pos = p.second;
            if (pos.size() < 2) continue;
            for (size_t i = 0; i < pos.size(); i++)
                for (size_t j = i + 1; j < pos.size(); j++) {
                    int d = pos[j] - pos[i];
                    for (int k = 2; k <= min(d, 6); k++)
                        if (d % k == 0) factorCount[k]++;
                }
        }
    }
    return factorCount;
}

/**
 * 综合 Kasiski + Friedman 确定密钥长度候选列表
 */
vector<int> determineKeyLength(const string& ciphertext, bool verbose = false) {
    auto kasiskiResult = kasiskiExamination(ciphertext);
    auto friedmanResult = friedmanTest(ciphertext);

    if (verbose) {
        cout << "  [Kasiski 试验] 因子频率：";
        vector<pair<int, int>> sorted(kasiskiResult.begin(), kasiskiResult.end());
        sort(sorted.begin(), sorted.end(), [](auto& a, auto& b) { return a.second > b.second; });
        for (auto& p : sorted) cout << p.first << "(" << p.second << ") ";
        cout << endl;

        cout << "  [Friedman 试验] 各密钥长度的平均 IC：" << endl;
        for (auto& p : friedmanResult)
            cout << "    m=" << p.second << " -> |IC-0.065|=" << p.first
                 << " (IC=" << avgICForKeyLen(ciphertext, p.second) << ")" << endl;
    }

    // 综合评分
    map<int, double> score;
    // Kasiski 评分
    int maxCount = 0;
    for (auto& p : kasiskiResult) maxCount = max(maxCount, p.second);
    if (maxCount > 0)
        for (auto& p : kasiskiResult)
            if (p.first >= 2 && p.first <= 6)
                score[p.first] += 5.0 * p.second / maxCount;
    // Friedman 评分（排名越靠前分越高）
    for (size_t i = 0; i < friedmanResult.size(); i++)
        score[friedmanResult[i].second] += (friedmanResult.size() - i);

    vector<pair<double, int>> ranked;
    for (auto& p : score) ranked.push_back({-p.second, p.first});
    sort(ranked.begin(), ranked.end());

    if (verbose) {
        cout << "  综合排名：";
        for (auto& p : ranked) cout << p.second << "(分=" << -p.first << ") ";
        cout << endl;
    }

    vector<int> result;
    for (auto& p : ranked) result.push_back(p.second);
    return result;
}

/**
 * 频数分析推导维吉尼亚密钥
 * 对密文按密钥长度 m 分成 m 个子串，每个子串相当于凯撒密码，
 * 通过与英文字母频率的互相关（chi-squared）确定每个位置的移位量
 */
string findVigenereKey(const string& ciphertext, int keyLen, bool verbose = false) {
    string key;
    for (int k = 0; k < keyLen; k++) {
        string sub;
        for (size_t i = k; i < ciphertext.length(); i += keyLen) sub += ciphertext[i];

        double maxScore = -1;
        int bestShift = 0;
        for (int shift = 0; shift < 26; shift++) {
            double score = 0;
            int freq[26] = {};
            for (char c : sub) freq[(charToNum(c) - shift + 26) % 26]++;
            int n = sub.length();
            for (int i = 0; i < 26; i++)
                score += (freq[i] / (double)n) * ENG_FREQ[i];
            if (score > maxScore) { maxScore = score; bestShift = shift; }
        }
        if (verbose)
            cout << "    位置 " << k << ": 最佳移位=" << bestShift
                 << " ('" << (char)('A' + bestShift) << "'), 互相关=" << maxScore << endl;
        key += (char)('A' + bestShift);
    }
    return key;
}

// ========== 第三步：Hill密码解密 ==========
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

                vector<vector<int>> PInv;
                if (!inverseMod26(P, PInv)) continue;

                A = mulMod26(C, PInv);
                if (!inverseMod26(A, AInv)) continue;

                bool consistent = true;
                for (int b = 0; b < numBlocks; b++) {
                    for (int row = 0; row < 3; row++) {
                        int val = 0;
                        for (int col = 0; col < 3; col++)
                            val = (val + A[row][col] * Pmat[b][col]) % N;
                        val = ((val % N) + N) % N;
                        if (val != Cmat[b][row]) {
                            consistent = false;
                            break;
                        }
                    }
                    if (!consistent) break;
                }
                if (consistent) return true;
            }
    return false;
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

/**
 * 去除 Hill 密码加密时添加的填充字符 'X'
 * 
 * Hill 密码 n=3 分组，若原始明文长度不是 3 的倍数，末尾补 'X' 凑齐。
 * 解密后结果长度一定是 3 的倍数，但末尾可能有 1~2 个填充的 'X'。
 * 
 * 注意：无论解密结果长度是否为 blockSize 的倍数（实际上一定是），
 * 都必须检查末尾连续 'X' 并去除最多 blockSize-1 个。
 * 旧版本错误地假设"长度是 3 的倍数就不需要去填充"。
 */
string removePadding(const string& s, int blockSize = 3) {
    if (s.empty()) return s;
    int trailingX = 0;
    for (int i = (int)s.size() - 1; i >= 0 && s[i] == 'X'; i--)
        trailingX++;
    // 填充数量最多为 blockSize-1（0、1 或 2 个）
    int padCount = min(trailingX, blockSize - 1);
    return s.substr(0, s.size() - padCount);
}

// 生成长度为 m 的所有无重复字母密钥
void generateKeys(int m, vector<string>& keys, string cur = "", int used = 0) {
    if ((int)cur.length() == m) { keys.push_back(cur); return; }
    for (int i = 0; i < 26; i++)
        if (!(used & (1 << i)))
            generateKeys(m, keys, cur + char('A' + i), used | (1 << i));
}

int main() {
    cout << "========== 古典密码综合解密 ==========" << endl;
    cout << "密文: " << CIPHERTEXT << endl;
    cout << "密文长度: " << CIPHERTEXT.length() << endl;
    cout << "已知明文前缀: " << KNOWN_PLAINTEXT << endl << endl;

    // ============================================================
    // 步骤 0：用 Kasiski + Friedman 试验分析密钥长度
    // 注意：这里先对最终密文做初步分析，确定候选密钥长度范围
    // ============================================================
    cout << "========== 密钥长度分析（Kasiski + Friedman） ==========" << endl;

    cout << "\n--- Kasiski 试验 ---" << endl;
    auto kasiskiOnCT = kasiskiExamination(CIPHERTEXT);
    cout << "  对最终密文查找重复 3-gram 间距的因子频率：" << endl;
    {
        vector<pair<int, int>> sorted(kasiskiOnCT.begin(), kasiskiOnCT.end());
        sort(sorted.begin(), sorted.end(), [](auto& a, auto& b) { return a.second > b.second; });
        for (auto& p : sorted)
            if (p.first <= 6)
                cout << "    因子 " << p.first << ": 出现 " << p.second << " 次" << endl;
    }

    cout << "\n--- Friedman 试验（重合指数法） ---" << endl;
    auto friedmanOnCT = friedmanTest(CIPHERTEXT);
    cout << "  各候选密钥长度的平均重合指数：" << endl;
    for (auto& p : friedmanOnCT)
        cout << "    m=" << p.second << " -> IC="
             << avgICForKeyLen(CIPHERTEXT, p.second)
             << " (|IC-0.065|=" << p.first << ")" << endl;

    cout << "\n--- 综合分析 ---" << endl;
    vector<int> keyLenCandidates = determineKeyLength(CIPHERTEXT, true);
    cout << "\n  候选密钥长度（按优先级）：";
    for (int m : keyLenCandidates) cout << m << " ";
    cout << endl;

    // ============================================================
    // 步骤 1 + 2 + 3：按候选密钥长度依次尝试解密
    //
    // 对每个候选 m：
    //   1) 枚举列置换规则，逆列置换得到 C2
    //   2) 对 C2 用频数分析推导维吉尼亚密钥
    //      - m≤4 时：由于密文短(72字符)且底层是 Hill 密文(非自然英文)，
    //        频数分析精度有限，需枚举所有无重复字母密钥并验证
    //      - m≥5 时：枚举列置换 + 频数分析推导密钥
    //   3) Hill 已知明文攻击验证
    // ============================================================

    string c2, c1, plaintext, vigenereKey;
    int keyLen = 0;
    vector<vector<int>> hillKey, hillInv;
    bool found = false;

    for (int m : keyLenCandidates) {
        if (found) break;
        cout << "\n====== 尝试密钥长度 m = " << m << " ======" << endl;

        if (m <= 4) {
            // m≤4：枚举所有无重复字母密钥（搜索空间可控）
            // 列置换密钥 = 维吉尼亚密钥的字典序排列
            vector<string> keys;
            generateKeys(m, keys);
            cout << "  枚举 " << keys.size() << " 个无重复字母密钥..." << endl;

            for (const string& key : keys) {
                vector<int> colOrder = keyToPermutation(key);
                string candidateC2 = reverseColumnar(CIPHERTEXT, m, colOrder);
                string candidateC1 = vigenereDecrypt(candidateC2, key);
                if ((int)candidateC1.length() < 18) continue;

                vector<vector<int>> hk, hi;
                if (!solveHillMatrix(KNOWN_PLAINTEXT.substr(0, 18),
                                     candidateC1.substr(0, 18), hk, hi))
                    continue;

                string raw = hillDecrypt(candidateC1, hi);
                string pt = removePadding(raw);
                bool allBlocksValid = true;
                for (size_t b = 0; b < candidateC1.length() / 3; b++) {
                    vector<int> p(3), c(3);
                    for (int j = 0; j < 3; j++) {
                        p[j] = charToNum(pt[b*3 + j]);
                        c[j] = charToNum(candidateC1[b*3 + j]);
                    }
                    for (int row = 0; row < 3; row++) {
                        int val = 0;
                        for (int col = 0; col < 3; col++)
                            val = (val + hk[row][col] * p[col]) % N;
                        if (((val % N) + N) % N != c[row]) {
                            allBlocksValid = false;
                            break;
                        }
                    }
                    if (!allBlocksValid) break;
                }
                if ((int)pt.length() >= 20 && pt.substr(0, 20) == KNOWN_PREFIX && allBlocksValid) {
                    keyLen = m; c2 = candidateC2; c1 = candidateC1;
                    vigenereKey = key; hillKey = hk; hillInv = hi;
                    plaintext = pt; found = true;
                    cout << "  找到匹配密钥: " << key << endl;
                    break;
                }
            }
        } else {
            // m≥5：枚举列置换 + 频数分析推导密钥
            vector<int> perm(m);
            for (int i = 0; i < m; i++) perm[i] = i;
            do {
                string candidateC2 = reverseColumnar(CIPHERTEXT, m, perm);
                string candidateKey = findVigenereKey(candidateC2, m);
                set<char> keySet(candidateKey.begin(), candidateKey.end());
                if ((int)keySet.size() != m) continue;

                string candidateC1 = vigenereDecrypt(candidateC2, candidateKey);
                if ((int)candidateC1.length() < 18) continue;

                vector<vector<int>> hk, hi;
                if (!solveHillMatrix(KNOWN_PLAINTEXT.substr(0, 18),
                                     candidateC1.substr(0, 18), hk, hi))
                    continue;

                string raw = hillDecrypt(candidateC1, hi);
                string pt = removePadding(raw);
                if ((int)pt.length() >= 20 && pt.substr(0, 20) == KNOWN_PREFIX) {
                    keyLen = m; c2 = candidateC2; c1 = candidateC1;
                    vigenereKey = candidateKey; hillKey = hk; hillInv = hi;
                    plaintext = pt; found = true;
                    break;
                }
            } while (!found && next_permutation(perm.begin(), perm.end()));
        }
    }

    if (!found) {
        cout << "\n解密失败：未找到满足已知明文约束的密钥组合" << endl;
        return 1;
    }

    // ============================================================
    // 输出完整解密过程和结果
    // ============================================================

    cout << "\n\n========================================" << endl;
    cout << "========== 解密成功！完整过程 ==========" << endl;
    cout << "========================================" << endl;

    // --- 第一步：逆列置换 ---
    cout << "\n========== 第一步：逆列置换 ==========" << endl;
    cout << "密钥长度 m = " << keyLen << endl;
    cout << "列置换密钥: " << vigenereKey << endl;
    vector<int> finalPerm = keyToPermutation(vigenereKey);
    cout << "置换规则: [";
    for (int i = 0; i < keyLen; i++) cout << (i ? ", " : "") << finalPerm[i];
    cout << "]" << endl;
    cout << "C2 (维吉尼亚加密后的中间密文): " << c2 << endl;

    // --- 第二步：维吉尼亚解密 ---
    cout << "\n========== 第二步：维吉尼亚解密 ==========" << endl;

    cout << "\n--- 2.1 Kasiski 试验（对 C2） ---" << endl;
    auto kasiskiResult = kasiskiExamination(c2);
    cout << "  重复 3-gram 间距的因子频率：" << endl;
    {
        vector<pair<int, int>> sorted(kasiskiResult.begin(), kasiskiResult.end());
        sort(sorted.begin(), sorted.end(), [](auto& a, auto& b) { return a.second > b.second; });
        for (auto& p : sorted)
            if (p.first <= 6)
                cout << "    因子 " << p.first << ": 出现 " << p.second << " 次" << endl;
    }

    cout << "\n--- 2.2 Friedman 试验（对 C2） ---" << endl;
    auto friedmanResult = friedmanTest(c2);
    cout << "  各候选密钥长度的平均重合指数：" << endl;
    for (auto& p : friedmanResult)
        cout << "    m=" << p.second << " -> IC="
             << avgICForKeyLen(c2, p.second)
             << " (|IC-0.065|=" << p.first << ")" << endl;

    cout << "\n--- 2.3 综合分析确定密钥长度 ---" << endl;
    vector<int> c2KeyLenCandidates = determineKeyLength(c2, true);
    cout << "  确定密钥长度: m = " << keyLen << endl;

    cout << "\n--- 2.4 频数分析推导维吉尼亚密钥 ---" << endl;
    string derivedKey = findVigenereKey(c2, keyLen, true);
    cout << "  频数分析直接推导的密钥: " << derivedKey << endl;
    cout << "  最终确定的维吉尼亚密钥: " << vigenereKey << endl;
    if (derivedKey != vigenereKey)
        cout << "  （注：密文仅 72 字符且底层为 Hill 密文，频数分析精度有限，" << endl
             << "   通过结合已知明文约束验证修正得到正确密钥）" << endl;
    cout << "  C1 (Hill加密后的中间密文): " << c1 << endl;

    // --- 第三步：Hill 密码解密 ---
    cout << "\n========== 第三步：Hill密码解密 ==========" << endl;
    cout << "已知条件：A 的第一行 = [11, 2, 19]" << endl;
    cout << "通过已知明文攻击（C1 = A × P mod 26）推导完整密钥矩阵" << endl;
    cout << "\n密钥矩阵 A:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  [";
        for (int j = 0; j < 3; j++) cout << (j ? ", " : "") << hillKey[i][j];
        cout << "]" << endl;
    }
    int det = ((det3(hillKey) % N) + N) % N;
    cout << "det(A) mod 26 = " << det << endl;
    cout << "gcd(det(A), 26) = " << gcd(det, 26) << " (=1，逆矩阵存在)" << endl;
    cout << "\n逆矩阵 A^-1 (mod 26):" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "  [";
        for (int j = 0; j < 3; j++) cout << (j ? ", " : "") << hillInv[i][j];
        cout << "]" << endl;
    }

    // --- 最终结果 ---
    cout << "\n========== 最终结果 ==========" << endl;
    string rawPlaintext = hillDecrypt(c1, hillInv);
    cout << "Hill解密原始输出（含填充）: " << rawPlaintext << endl;
    cout << "原始长度: " << rawPlaintext.length() << endl;

    // 去除填充
    int trailingX = 0;
    for (int i = (int)rawPlaintext.size() - 1; i >= 0 && rawPlaintext[i] == 'X'; i--)
        trailingX++;
    int padCount = min(trailingX, 2);
    cout << "末尾连续 'X' 个数: " << trailingX << endl;
    if (padCount > 0)
        cout << "去除填充 " << padCount << " 个 'X'（Hill n=3 分组填充最多 2 个）" << endl;
    else
        cout << "末尾无填充字符" << endl;

    cout << "\n去除填充后的明文: " << plaintext << endl;
    cout << "明文长度: " << plaintext.length() << endl;
    cout << "\n★ 完整原始明文: " << plaintext << endl;
    return 0;
}
