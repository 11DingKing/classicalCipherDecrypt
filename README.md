# 古典密码综合解密项目

## 1 How to Run

### Docker 运行（推荐）
```bash
docker compose up --build -d
docker compose logs -f backend   # 查看解密输出
```

支持 ARM64（Apple Silicon）与 AMD64（X86），基础镜像为 `debian:bookworm-slim`。

### 本地 C++ 运行
```bash
cd backend
g++ -std=c++17 -O2 -o decrypt classical_cipher_decrypt.cpp
./decrypt
```

---

## 2 Services

本项目为**古典密码解密**命令行程序，无 Web 服务。主要功能：

- **逆列置换**：还原列置换密码，得到维吉尼亚密文 C2
- **维吉尼亚解密**：通过 Friedman/Kasiski 试验与频数分析破解密钥，得到 Hill 密文 C1
- **Hill 密码解密**：基于已知明文推导密钥矩阵并解密，输出原始明文

---

## 3 测试账号

本项目为密码学解密程序，无用户系统，不涉及测试账号。

---

## 4 题目内容

古典密码综合解密编程题：多层加密密文破解挑战
题目背景
某敏感信息经过 三层古典密码加密 后生成密文，加密流程依次为：Hill 密码（n=3）→ 维吉尼亚密码 → 列置换密码。已知少量明文片段和部分加密参数，要求编写程序逆向破解，还原完整原始明文。
已知条件
1.最终密文（大写字母，无空格）：
EJBUJFNVHNZZVUXXEYCBJMAYVIZVLKSDOCEWGEIGETXBNTDEQPEIWUKKDIFFEMZRBMOHDMIZ
2.已知明文片段：原始明文开头为 CONFIDENTIALINFORMATION（前 20 个字符），对应密文前 20 个字符为 EJBUJFNVHNZZVUXXEYCB。
3.加密流程细节：
o第一层：Hill 密码（n=3），明文按 3 个字符分组，不足补 'X'；密钥矩阵 A 为 3×3 整数矩阵（元素∈[0,25]），且 gcd (det (A),26)=1（保证逆矩阵存在），已知 A 的第一行为 [11, 2, 19]（对应课程例中矩阵的第一行）。
o第二层：维吉尼亚密码，密钥长度未知（≤6），密钥为纯大写英文字母（无重复）。
o第三层：列置换密码，密钥长度 = 维吉尼亚密钥长度（记为 m），置换规则为：将维吉尼亚加密后的字符串按 m 列分行（行优先填充），再按密钥字母的字典序重新排列列（字典序小的列在前），最终按列读取得到密文。
4.所有加密过程均基于 26 个英文字母（A=0, B=1, ..., Z=25），不区分大小写。
解题要求
编写程序完成以下步骤，最终输出完整原始明文：
1.逆列置换：推导列置换的密钥和置换规则，对最终密文进行逆操作，得到维吉尼亚加密后的中间密文 C2。
2.维吉尼亚解密：
o用卡西斯基试验（Kasiski Examination）或弗里德曼试验（Friedman Test）确定密钥长度 m；
o基于频数分析推导维吉尼亚密钥；
o解密 C2 得到 Hill 加密后的中间密文 C1。
3.Hill 密码解密：
o基于已知明文片段和 C1 的对应关系，推导 Hill 密钥矩阵 A 的完整 3×3 矩阵；
o计算 A 的模 26 逆矩阵 A⁻¹；
o解密 C1 得到原始明文（去除末尾填充的 'X'）。
4.输出完整原始明文（要求去除加密时的填充字符，仅保留有效信息）。



评分标准
1.逆列置换正确（20 分）：推导置换密钥和规则，输出 C2；
2.维吉尼亚解密正确（30 分）：密钥长度和密钥推导正确，输出 C1；
3.Hill 密码解密正确（30 分）：完整密钥矩阵 A 和逆矩阵计算正确，输出原始明文；
4.程序健壮性（20 分）：处理分组填充、模运算溢出、逆矩阵不存在等异常情况，代码结构清晰。
5.使用AI生成编程或者写报告，一律得0分。
6.编程语言不限，推荐使用C/C++，同等情况下使用C/C++可适当得更加高的分数。
---

## 5 项目介绍

### 技术栈

- **C++17**：古典密码解密（逆列置换、维吉尼亚、Hill），满足题目加分要求。

### 项目目录

```
label-01452/
├── README.md                  # 项目说明（本文件）
├── .gitignore                 # Git 忽略文件
├── docker-compose.yml         # Docker Compose 配置
└── backend/                   # 解密程序
    ├── Dockerfile             # 多平台镜像（ARM64/AMD64）
    ├── .dockerignore          # Docker 构建忽略
    └── classical_cipher_decrypt.cpp   # C++ 解密主程序
```

### 自测流程

**步骤 1：Docker 构建与运行**

```bash
docker compose up --build -d
```

**预期**：构建成功后出现 `Container classical-cipher-decrypt Started`。

```bash
docker compose logs backend
```

**预期**：输出包含以下关键信息：
- `密钥长度分析（Kasiski + Friedman）` — 密钥长度候选分析
- `密钥长度 m = 3`
- `列置换密钥: KEY`
- `维吉尼亚密钥: KEY`
- `密钥矩阵 A: [11, 2, 19] / [3, 7, 4] / [5, 8, 2]`
- `★ 完整原始明文: CONFIDENTIALINFORMATION...`（以 CONFIDENTIALINFORMATION 开头）

---

**步骤 2：本地 C++ 编译与运行**

```bash
cd backend
g++ -std=c++17 -O2 -o decrypt classical_cipher_decrypt.cpp
./decrypt
```

**预期**：编译无报错；运行输出与 Docker 中一致，含完整解密流程与明文。

---

**验证通过**：Docker 与本地 C++ 均能正确输出解密结果。
