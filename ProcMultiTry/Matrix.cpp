#include "Matrix.h"
#include <cmath>

int Matrix::count = 0;

float prec1 = 1000000;

Matrix::Matrix(int rows, int columns) : _rows(rows), _columns(columns) {
	id = count;
	++count;
	arr = new float* [_rows];
	float** arrTmp = this->arr;
	int cols = _columns;
	int rowst = _rows;
#pragma omp parallel shared(arrTmp, cols, rowst) //num_threads(8)
{
#pragma omp for
		for (int i = 0; i < rowst; ++i) {
			arrTmp[i] = new float[cols];
			for (int j = 0; j < cols; ++j){
				arrTmp[i][j] = 0;
			}
		}
	}
}

Matrix::Matrix(const Matrix& other) :Matrix(other._rows, other._columns) {
	//std::cout << "def copy constructor from " << other.id << " to " << id << "\n";
	float** arrTmp = this->arr;
	int cols = _columns;
	int rows = _rows;
	float** otharr = other.arr;
#pragma omp parallel shared(arrTmp, rows, cols, otharr) //num_threads(8)
	{
#pragma omp for
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				arrTmp[i][j] = otharr[i][j];
			}
		}
	}
}

Matrix::Matrix(Matrix&& other) {
	id = count;
	//std::cout << "rv copy constructor from " << other.id << " to " << id << "\n";
	++count;
	arr = other.arr;
	_columns = other._columns;
	_rows = other._rows;
	other.arr = nullptr;
}

Matrix::Matrix() :_rows(0), _columns(0) {
	id = count;
	++count;
	arr = nullptr;
}

Matrix& Matrix::operator=(const Matrix& other) {
	//std::cout << "def copy operator from " << other.id << " to " << id << "\n";
	for (int i = 0; i < _rows; ++i) {
		delete[] arr[i];
	}
	delete[] arr;

	_rows = other._rows;
	_columns = other._columns;

	arr = new float* [_rows];
	float** arrTmp = this->arr;
	int cols = _columns;
	int rows = _rows;
	float** otharr = other.arr;
#pragma omp parallel shared(arrTmp, rows, cols, otharr) //num_threads(8)
	{
#pragma omp for
		for (int i = 0; i < rows; ++i) {
			arr[i] = new float[cols];
			for (int j = 0; j < cols; ++j)
				arrTmp[i][j] = otharr[i][j];
		}
	}
	return *this;
}

Matrix::~Matrix() {
	//std::cout << "deleting matrix " << id << "\n";
	if (arr == nullptr) return;
	for (int i = 0; i < _rows; ++i) {
		delete[] arr[i];
	}
	delete[] arr;
}

Matrix& Matrix::operator+=(const Matrix& other) {
	float** otharr = other.arr;
	float** arrTmp = this->arr;
	int cols = _columns;
	int rows = _rows;
#pragma omp parallel shared(arrTmp, otharr, rows, cols) //num_threads(8)
	{
#pragma omp for
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				arrTmp[i][j] += otharr[i][j];
			}
		}
	}

	return *this;

}

Matrix Matrix::operator+(const Matrix& other) {
	Matrix ret(*this);

	return ret += other;
}
//
//Matrix& Matrix::operator=*(const Matrix& other) {
//	Matrix matrixNew = Matrix(_rows, other._columns);
//	for (int i = 0; i < matrixNew._rows; ++i)
//		for (int j = 0; j < matrixNew._columns; ++j)
//			for (int r = 0; r < _columns; ++r)
//				matrixNew.arr[i][j] += arr[i][r] * other.arr[r][j];
//	return matrixNew;
//}

Matrix& Matrix::operator*=(float l) {
	float** arrTmp = this->arr;
	int cols = _columns;
	int rows = _rows;
#pragma omp parallel shared(arrTmp, l, rows, cols) //num_threads(8)
	{
#pragma omp for
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				arrTmp[i][j] *= l;
			}
		}
	}

	return *this;
}

Matrix Matrix::operator*(float l) {
	Matrix ret(*this);
	return ret *= l;
}

Matrix& Matrix::operator/=(float l) {
	float** arrTmp = this->arr;
	int cols = _columns;
	int rows = _rows;
#pragma omp parallel shared(arrTmp, l, rows, cols) //num_threads(8)
	{
#pragma omp for
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				arrTmp[i][j] /= l;
			}
		}
	}

	return *this;
}

Matrix Matrix::operator/(float l) {
	Matrix ret(*this);
	return ret /= l;
}

Matrix& Matrix::operator=(Matrix&& other) {
	//std::cout << "rv copy operator from " << other.id << " to " << id << "\n";
	int i = 0;
	int j = 0;
	for (int i = 0; i < _rows; ++i) {
		delete[] arr[i];
	}
	delete[] arr;
	arr = other.arr;
	//arr[i][i] = arr[j][j];
	_columns = other._columns;
	_rows = other._rows;
	other.arr = nullptr;

	return *this;
}


void Matrix::MultiplyRow(int row, float l) {
	float** arrTmp = this->arr;
	int cols = _columns;
	
#pragma omp parallel shared(arrTmp, row, l, cols)
	{
#pragma omp for
		for (int i = 0; i < cols; ++i) arrTmp[row][i] *= l;
	}
}

void Matrix::PlusRows(int row1, int row2) {
	float** arrTmp = this->arr;
	int cols = _columns;
	int rows = _rows;
#pragma omp parallel shared(arrTmp, row1, row2, cols) //num_threads(8)
	{
#pragma omp for
		for (int i = 0; i < cols; ++i) arrTmp[row1][i] += arrTmp[row2][i];
	}
	//for (int i = 0; i < _columns; ++i) arr[row1][i] += arr[row2][i];
}

void Matrix::MinusRows(int row1, int row2) {
	float** arrTmp = this->arr;
	int cols = _columns;
	int rows = _rows;
#pragma omp parallel shared(arrTmp, row1, l, cols) //num_threads(8)
	{
#pragma omp for
		for (int i = 0; i < cols; ++i) arrTmp[row1][i] -= arrTmp[row2][i];
	}
}

void Matrix::swapLines(int line1, int line2) {
	float* temp = arr[line1];
	arr[line1] = arr[line2];
	arr[line2] = temp;
}

void Matrix::ToUpTriangle() {
	int min = _rows < _columns ? _rows : _columns;

	for (int i = 0; i < min; ++i) {

		MultiplyRow(i, 1 / arr[i][i]);
		for (int j = i + 1; j < _rows; ++j) {
			float c = arr[j][i];
			if (c == 0) continue;

			MultiplyRow(i, c);
			MinusRows(j, i);
			MultiplyRow(i, 1 / c);

		}
	}
}

Matrix::operator std::string() const {
	std::string ans = "";

	for (int i = 0; i < _rows; i++) {
		for (int j = 0; j < _columns; ++j)
			ans += std::to_string((floorf(arr[i][j] * prec1) / prec1)) + " ";
		ans += "\n";
	}

	return ans;
}
