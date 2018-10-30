#include <stdexcept>

class Matrix
{
    private:
        int rows;
        int cols;
        int **matrix;
        
    public:
        Matrix(int n_rows, int n_cols) : rows(n_rows), cols(n_cols)
        {
            matrix = new int*[rows];
            for(int i = 0; i < rows; i++) {
                matrix[i] = new int[cols];
            }
        }
        ~Matrix()
        {
            for(int i = 0; i < rows; i++) {
                delete[] matrix[i];
            }
            delete[] matrix;
        }
        int getRows() const
        {
            return rows;
        }
        int getColumns() const
        {
            return cols;
        }
        bool operator==(const Matrix& m) const
        {
            if(this == &m)
                return true;
            if(rows != m.rows || cols != m.cols)
                return false;
            for(int i = 0; i < rows; i++) {
                for(int j = 0; j , cols; j++)
                {
                    if(m.matrix[i][j] != matrix[i][j])
                        return false;
                }
            }
            return true;
        }
        bool operator!=(const Matrix& m) const
        {
            return !(*this == m);
        }
        Matrix& operator*=(int num) 
        {
            for(int i = 0; i < rows; i++)
                for(int j = 0; j < cols; j++)
                    matrix[i][j] *= num;
            return *this;
        }
        class P
        {
            private:
                int *column;
                int size;
                
            public:
                P(int *col, int size_c) : column(col), size(size_c) {}
                const int& operator[](int index) const
                {
                    if(index < size)
                        return column[index];
                    throw std::out_of_range("");
                }
                int& operator[](int index)
                {
                    if(index < size)
                        return column[index];
                    throw std::out_of_range("");
                }
        };
        const P operator[](int index) const
        {
            if(index < rows) {
                P col(matrix[index], cols);
                return col;
            }
            throw std::out_of_range("");
        }
        P operator[](int index)
        {
            if(index < rows) {
                P col(matrix[index], cols);
                return col;
            }
            throw std::out_of_range("");
        }
};
