# 多平台支持：ARM64 (Apple Silicon) + AMD64 (X86)
# 使用官方 GCC 镜像，无需 apt 安装编译环境
FROM gcc:12-bookworm

WORKDIR /app

COPY classical_cipher_decrypt.cpp .

RUN g++ -std=c++17 -O2 -o decrypt classical_cipher_decrypt.cpp

CMD ["./decrypt"]
