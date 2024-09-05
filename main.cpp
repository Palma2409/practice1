#include <iostream>
#include "matrix.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

int main(int argc, char** argv) {
    std::srand(std::time(nullptr));
    
    int rows = 5, cols = 5;
    if(argc >= 3){
    	rows = atoi(argv[1]);
    	cols = atoi(argv[2]);
    }

    Matrix<int> matrix(rows, cols);

    for (auto& it : matrix) {
        it = std::rand() % 100;
    }
    
    std::cout << "Исходная матрица:\n";
    std::cout << matrix;

    std::sort(matrix.begin(), matrix.end());
    
    std::cout << "Отсортированная матрица:\n";
    std::cout << matrix;
    return 0;
}
