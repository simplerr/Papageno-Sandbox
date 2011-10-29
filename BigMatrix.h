#ifndef BIGMATRIX_H
#define BIGMATRIX_H

#include <vector>
using namespace std;

class BigMatrix
{
public:
	BigMatrix(){};
	~BigMatrix(){};

	void setSize(int width, int height)	{
		matrix.resize(height);

		for(int i = 0; i < height; i++)
			matrix[i].resize(width);
	}

	void empty()	{
		for(int i = 0; i < matrix.size(); i++)	{
			for(int j = 0; j < matrix[i].size(); j++)
				matrix[i][j] = 0;
		}
	}

	double get(int x, int y)	{
		return matrix[x][y];
	}

	void set(int x, int y, double value)	{
		matrix[x][y] = value;
	}

	void add(int x, int y, double value)	{
		matrix[x][y] += value;
	}

	void sub(int x, int y, double value)	{
		matrix[x][y] -= value;
	}

	double** to2dArray()	{
		double** array = new double*[matrix.size()];
		for(int i = 0; i < matrix.size(); i++)
			array[i] = new double[matrix[i].size()];

		return array;
	}
private:
	vector<vector<double>> matrix;
};

#endif