#pragma once

#include "../util/util.h"

#include <math.h>
#include <iostream>
#include <list>
#include <vector>
#include "precisionType.h"
#include <ostream>
#include <streambuf>
#include <sstream>
#include <iostream>
#include <fstream>

class FractalSection;
struct CompassStruct;

namespace FractalData {
	enum Type {
		flipFractal,
		size
	};

	struct Section {
		PREDEC cornerX, cornerY;
		PREDEC spanX, spanY;
	};
	struct InitialCondition {
		PREDEC m1, m2;
		PREDEC l1, l2;
		PREDEC g;
	};
};

class Fractal {
public:
	Fractal(int resolution, int shaderType, FractalData::InitialCondition ic);

	unsigned long long countBoxes();
	virtual void getCompassStartPos(CompassStruct& compass_info);
	void getCompassDimension(PrintOutput& print_output);

	int iteration_count = 1000;
	double time_step = 0.01;

	int getResolution() { return resolution; }
	FractalData::InitialCondition getIc() { return ic; }

	unsigned char getBatchChar(int x, int y);
	virtual unsigned char getChar(int x, int y);
private:
	void loadCurrentSection(int xIn, int yIn, unsigned char* generate);
	void shiftCurretSections(int shiftX, int shiftY);

	//std::list<Position> getCompassBorderCrossing();
	//BorderArea getBorderArea(int border_area_start_x, int border_area_start_y, std::list<Position> border_crossings);

	const int CURRENT_SECTIONS_SIDE = 5;
	FractalSection** currentSections;
	int currentCornerX = 0, currentCornerY = 0;

	FractalData::InitialCondition ic;

	friend struct CompassStruct;
protected:
	int resolution, resolutionSectionCount, shaderType;

	Fractal() {}
};

struct BorderArea {
	static const int size = 1024;
	static const int box_dimension_count = 9;

	// (1: 0), (2: 1), (4: 2), (8: 3) ...
	// Box count for different sizes
	int boxSize[box_dimension_count] = {};
};

struct Position {
	int x;
	int y;
	char dir; // Direction
	Position(int t_x, int t_y, int direction): x(t_x), y(t_y), dir(direction) {}
};

struct CompassStruct {
public:
	int current_x;
	int current_y;
	int grid_width;
	int grid_height;
	char direction = POINTING_RIGHT;
	char spin = CLOCKWISE_SPIN;
	enum {
		CLOCKWISE_SPIN = 2,
		CLOCKWISE_SPIN_DIAGONAL = 1,
		COUNTER_CLOCKWISE_SPIN = -2,
		COUNTER_CLOCKWISE_SPIN_DIAGONAL = -1
	};
	enum {
		POINTING_UP,
		POINTING_UP_RIGHT,
		POINTING_RIGHT,
		POINTING_BOTTOM_RIGHT,
		POINTING_BOTTOM,
		POINTING_BOTTOM_LEFT,
		POINTING_LEFT,
		POINTING_UP_LEFT,
	};
	void getNextPoint(Fractal* fractal);
};


template<class TYPE>
class Array2DRef;

// 2D Array that can get custom size during runtime
template<class TYPE>
class Array2D {
	std::vector<TYPE> arr;
	unsigned int m_size_1;
	unsigned int m_size_2;
public:
	Array2D(unsigned int size_1, unsigned int size_2) :
		m_size_1(size_1), m_size_2(size_2) {
		arr.resize(m_size_1 * m_size_2);
	}
	Array2DRef<TYPE> operator[](unsigned int index_1);
	TYPE& getPosition(unsigned int index_1, unsigned int index_2) {
		return(arr[index_1 * (long)m_size_1 + index_2]);
	}
	unsigned int size() { return m_size_1; }
	friend Array2DRef<TYPE>;
};

// Help class for Array2D
template<class TYPE>
class Array2DRef {
	Array2D<TYPE>* arr_2d;
	unsigned int index_1;
public:
	TYPE& operator[](unsigned int index_2) {
		return arr_2d->getPosition(index_1, index_2);
	}
	unsigned int size() {
		return arr_2d->m_size_2;
	}
	friend Array2D<TYPE>;
};
template<class TYPE>
Array2DRef<TYPE> Array2D<TYPE>::operator[](unsigned int index_1) {
	Array2DRef<TYPE> arr_ref;
	arr_ref.arr_2d = this;
	arr_ref.index_1 = index_1;
	return arr_ref;
}


