#pragma once

#include <iostream>
#include <iterator>
#include <cmath>

template <typename T>
class Matrix {
private:
    T** _values; //указатель на двумерный массив
    int _rows, _cols; //число строк и столбцов в матрице
public:
    //конструктор
    Matrix(unsigned rows, unsigned cols) {
        if (!(rows > 0 && cols > 0)) //если размерности не положительны, выбрасываем исключение
        {
            throw std::invalid_argument("Matrix dimensions can't be zero");
        }
        _rows = rows;
        _cols = cols;
	//выделяем память под строки
        _values = (T**) new T * [_rows];
	//выделяем память под элементы в каждой строке
        for (int i = 0; i < _rows; i++)
            _values[i] = (T*) new T[_cols];
	//начальная инициалиализация
        for (int i = 0; i < _rows; i++)
            for (int j = 0; j < _cols; ++j) {
                _values[i][j] = 0;
            }
    }
    //деструктор
    ~Matrix() {
        for (int i = 0; i < _rows; i++) //удаляем элементы в каждой строке
            delete[] _values[i];
        delete[] _values; //удаляем строки
    }
    //конструктор копирования
    Matrix(const Matrix<T>& matrix) {
        _rows = matrix._rows;
        _cols = matrix._cols;

        _values = (T**) new T * [_rows];

        for (int i = 0; i < _rows; i++) {
            _values[i] = (T*) new T[_cols];
        }
        for (int i = 0; i < _rows; i++) {
            for (int j = 0; j < _cols; j++) {
                _values[i][j] = matrix._values[i][j];
            }
        }
    }
    //перегрузка оператора присваивания одному объекту типа матрица значения другого объекта типа матрица
    Matrix<T>& operator=(const Matrix<T>& matrix) {
        if (this == &matrix) //проверили не тот же ли это элемент
            return *this;

        if (_rows != matrix._rows || _cols != matrix._cols) { //если размеры не равны, то очищаем память для данного объекта и выделяем заново
            for (int i = 0; i < _rows; i++)
                delete[] _values[i];
            delete[] _values;

            _rows = matrix._rows;
            _cols = matrix._cols;

            _values = (T**) new T * [matrix._rows];
            for (int i = 0; i < matrix._rows; i++) {
                _values[i] = (T*) new T[matrix._cols];
            }
        }

        for (int i = 0; i < _rows; i++) { //пробегаемся и поэлементно заполняем ячейки данной матрицы значениями той, которую присваиваем
            for (int j = 0; j < _cols; j++) {
                _values[i][j] = matrix._values[i][j];
            }
        }
        return *this;
    }

    template <typename T1> //чтобы не было перекрытия шаблонного параметра
    friend std::ostream& operator<<(std::ostream& os, const Matrix<T1>& matrix) {
        for (int i = 0; i < matrix._rows; i++) {
            os << '[';
            for (int j = 0; j < matrix._cols; j++) {
                os << matrix._values[i][j];
                if (j + 1 != matrix._cols) {
                    os << '\t';
                }
            }
            os << "]\n";
        }
        return os;
    }
    //то же самое что и оператор [][]
    T& getValue(const int i, const int j) {
        return this->_values[i][j];
    }
    //константный оператор [][]
    const T& getValue(const int i, const int j) const {
        return this->_values[i][j];
    }

    class Iterator;
    Iterator begin();
    Iterator end();
};

template <typename T>
class Matrix<T>::Iterator {
private:
    T* _begin, * _end; //начало и конец контейнера
    int _i, _j, _num; //num - номер в улиточном порядке на отрезке [0; rows*cols-1]
    T** _values;
    int _rows, _cols;

public:
    using iterator_category = std::random_access_iterator_tag; //объявили категорию итератора (это тип данных)
    using difference_type = std::ptrdiff_t; //разница между итераторами (также тип данных)
    using value_type = T; //указываем тип данных
    using pointer = value_type*;//тип данных указателя
    using reference = value_type&; //тип данных ссылки
    using const_reference = const value_type&; //тип данных константной ссылки
    
    
    
    void set_i_j(){ //сеттер для определения индексов матрицы по индексу одномерного улиточного массива
    if (_num < _rows - 1){
        _i = _rows - _num - 1;
        _j = 0;
    }
    else{
        int tmp = _num - _rows + 1;
        int n = (int) ((_rows + _cols - 1 - sqrt((_rows + _cols - 1) * (_rows + _cols - 1) - 4 * tmp)) / 2. + 1.);
        tmp -= (n - 1) * (_rows + _cols - n);
        if (n % 2 == 0) { // start from [_rows - n // 2][_cols - n // 2]
            if (tmp >= _cols - n) { // движение вверх
                _i = _rows - n / 2 - (tmp - (_cols - n));
                _j = n / 2;
            }
            else{ // движение влево
                _i = _rows - n / 2;
                _j = _cols - n / 2 - tmp;
            }
        }
        else{ // start from [n // 2][n // 2]
            if (tmp >= _cols - n){  // движение вниз
                _i = n / 2 + (tmp - (_cols - n));
                _j = _cols - (n + 1) / 2;
            }
            else{ // движение вправо
                _i = n / 2;
                _j = n / 2 + tmp;
            }
        }
    }
}

    //конструктор инициализации
    Iterator(pointer begin, pointer end, int num, int rows, int cols, value_type** values) :
        _begin(begin), _end(end), _num(num), _rows(rows), _cols(cols), _values(values) {
        set_i_j();
    }
    //операторы сравнения итераторов
    bool operator==(const Iterator& it) const { return  _num == it._num; }
    bool operator!=(const Iterator& it) const { return _num != it._num; }
    bool operator<(const Iterator& it) const { return _num - it._num < 0; }
    bool operator<=(const Iterator& it) const { return _num - it._num <= 0; }
    bool operator>(const Iterator& it) const { return _num - it._num > 0; }
    bool operator>=(const Iterator& it) const { return _num - it._num >= 0; }

    //перегрузка оператора разыменования
    reference operator*() {
        if (_num == _rows * _cols) //проверяем, чтобы не разыменовать конец
            throw std::out_of_range("Dereference of an end pointer");
        return (_values[_i][_j]);
    } 

    //перегрузка оператора константного разыменования
    const_reference operator*() const {
        if (_num == _rows * _cols)
            throw std::out_of_range("Dereference of an end pointer");
        return (_values[_i][_j]);
    }
	
    //префиксная инкрементация
    Iterator& operator++() //++it
    {
        if (_num == _rows * _cols - 1) { //если уже находимся в конце улитки, то дальше идти некуда, ставим нулевой указатель
            *this = Iterator(this->_begin,
                static_cast<T*>(nullptr),
                _rows * _cols, this->_rows,
                this->_cols, this->_values);
            return *this;
        }

        _num++;
        set_i_j();

        return *this;
    }
	
    //постфиксная икрементация
    Iterator operator++(int) //it++
    {
        Iterator prev = *this;

        *this = ++(*this);

        return prev;
    }
	
    //префиксная декрементация
    Iterator& operator--() //--it
    {
        if (_num == _rows * _cols) { //если уже находимся в ячейке, следующей за концом матрицы, то можем вернуться назад
            *this = Iterator(this->_begin, this->_end,
                this->_num - 1, this->_rows, 
                this->_cols, this->_values);
            return *this;
        }

        if (_num != 0) {
            _num--;
            set_i_j();
        }

        return *this;
    }
	
    //постфиксная декрементация
    Iterator operator--(int) //it--
    {
        Iterator prev = *this;

        *this = --(*this);

        return prev;
    }

    //оператор прибавления
    Iterator operator+(difference_type n) const {
        Iterator shiftedIt = *this;

        for (difference_type i = 0; i < n; i++) {
            ++shiftedIt;
        }
        return shiftedIt;
    }

    //оператор прибавления и присваивания
    Iterator operator+=(difference_type n) {

        for (difference_type i = 0; i < n; i++) {
            ++(*this);
        }
        return *this;
    }

    //оператор вычитания
    Iterator operator-(difference_type n) const {
        Iterator shiftedIt = *this;

        for (difference_type i = 0; i < n; i++) {
            --shiftedIt;
        }
        return shiftedIt;
    }

    //оператор вычитания и присваивания
    Iterator operator-=(difference_type n) {

        for (difference_type i = 0; i < n; i++) {
            --(*this);
        }
        return *this;
    }
	
    //разность между итераторами
    difference_type operator-(const Iterator& it) const {
        return _num - it._num;
    }

    //дружественная перегрузка для того чтобы делать не it + 5, а ещё и 5 + it (коммутативность)
    template <typename T1>
    friend typename Matrix<T1>::Iterator& operator+(difference_type n, const typename Matrix<T1>::Iterator& it) {
        typename Matrix<T1>::Iterator shiftedIt = it;

        for (difference_type i = 0; i < n; i++) {
            if (shiftedIt._num == shiftedIt._rows * shiftedIt._cols) //если дошли до конца — дальше не идём
                return shiftedIt;

            shiftedIt._num++;
            shiftedIt.set_i_j();
        }
        return shiftedIt;
    } 

   //смена итераторов местами
    void swap(Iterator& a, Iterator& b) {
        Iterator tmp(std::move(a));
        a = std::move(b);   
        b = std::move(tmp);
    }
};

//итератор начала смотрит на первую ячейку матрицы
template <typename T>
typename Matrix<T>::Iterator Matrix<T>::begin() {
    return Iterator(&(_values[_rows][0]), static_cast<T*>(nullptr), 0, _rows, _cols, _values);
}

//итератор окончания смотрит на место ЗА последней ячейкой матрицы
template <typename T>
typename Matrix<T>::Iterator Matrix<T>::end() {
    return Iterator(&(_values[_rows][0]), static_cast<T*>(nullptr), _rows * _cols, _rows, _cols, _values);
}
