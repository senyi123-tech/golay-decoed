#include <numeric>
#include <cstdio>
#include<iostream> 
#include <chrono>  // 添加计时器头文件
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
// Golay(24,12) 解码函数声明
int* decode_golay24(const int* encoded_data, const int(*H)[24]);

// 错误纠正函数声明
int* correct_errors(const int* encoded_data, const int* syndrome, const int(*H)[24]);
const int arraySize = 2040, blocks = 1600;
int datalen = 0;
int encoded_data[arraySize * blocks] = { 0 };
int encoded_data_use[24];
int decoded_data[arraySize * blocks / 24 * 12];
int main() {
    //int encoded_data[24] = { 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1 }; 
    // 定义 H 矩阵
    int H[12][24] = {
        {1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {1 ,0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    };
    //读取数据
    string line;
    ifstream file("C:\\Users\\LENOVO\\Desktop\\data\\encodedata.txt");

    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream ss(line);
            int value;
            while (ss >> value && datalen < arraySize * blocks) {
                encoded_data[datalen++] = value;
            }
        }
        file.close();

        // 打印读取的数据
       /* for (int i = 0; i < datalen; i++) {
            std::cout << encoded_data[i] << " ";
        }
        std::cout<< std::endl;*/
        std::cout << "读入数据长度: " << datalen;
        std::cout << std::endl;
    }
    else {
        std::cout << "Unable to open file.";
        std::cout << std::endl;
    }
    // 启动计时器
    auto start = std::chrono::steady_clock::now();
    int count = 0;
    // 解码数据 
    for (int i = 0; i < arraySize * blocks; i++)
    {
        encoded_data_use[i % 24] = encoded_data[i];
        if ((i + 1) % 24 == 0) // 每30个数据进行一次解码
        {
            int* decoded_data_use = decode_golay24(encoded_data_use, H);

            for (int j = 0; j < 12; j++)
                decoded_data[i / 24 * 12 + j] = decoded_data_use[j];

            delete[] decoded_data_use; // 释放内存
        }
    }
    // 停止计时器并计算时间差
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Decoding time: " << elapsed_seconds.count()<< "s\n";

    // 输出解码后的数据
  /*  std::cout << "Decoded Golay(24,12) data: ";
    for (int i = 0; i < arraySize * blocks / 24 * 12; ++i) {
        std::cout << decoded_data[i] << " ";
    }
    std::cout << std::endl;*/

    return 0;
}

// Golay(24,12) 解码函数定义
int* decode_golay24(const int* encoded_data, const int(*H)[24]) {
    // Golay(24,12) 解码的实现
    int syndrome[12] = { 0 };

    // 计算 syndrome
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 24; ++j) {
            syndrome[i] += encoded_data[j] * H[i][j];
        }
        syndrome[i] %= 2;
    }

    // 如果 syndrome 全为 0，直接返回原始数据
    if (std::accumulate(syndrome, syndrome + 12, 0) == 0) {
        int* decoded_data = new int[12];
        for (int i = 0; i < 12; ++i) {
            decoded_data[i] = encoded_data[i];
        }
        return decoded_data;
    }
    // 否则进行错误纠正
    return correct_errors(encoded_data, syndrome, H);
}
// 错误纠正函数定义
int* correct_errors(const int* encoded_data, const int* syndrome, const int(*H)[24]) {
    int error_vector[24] = { 0 };

    // 查找对应的错误位索引
    for (int i = 0; i < 12; ++i) {
        int match = 1;
        for (int j = 0; j < 12; ++j) {
            if (syndrome[j] != H[j][i]) {
                match = 0;
                break;
            }
        }
        if (match) {
            error_vector[i] = 1;
        }
    }

    // 翻转错误位
    int* corrected_data = new int[24];
    for (int i = 0; i < 24; ++i) {
        corrected_data[i] = encoded_data[i] ^ error_vector[i];
    }

    return corrected_data;
}
