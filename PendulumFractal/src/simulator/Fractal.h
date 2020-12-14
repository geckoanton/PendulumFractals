#pragma once

#include <math.h>
#include <iostream>
#include <list>
#include <vector>
#include "precisionType.h"

class FractalSection;

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

	unsigned long long countBoxes(unsigned char boxType);
	//BorderArea countEdges(unsigned char boxType);
	double getCompassDimension(int compass_length);

private:
	void loadCurrentSection(int xIn, int yIn, unsigned char* generate);
	void shiftCurretSections(int shiftX, int shiftY);

	unsigned char getBatchChar(int x, int y);
	unsigned char getChar(int x, int y);

	//std::list<Position> getCompassBorderCrossing();
	//BorderArea getBorderArea(int border_area_start_x, int border_area_start_y, std::list<Position> border_crossings);

	const int CURRENT_SECTIONS_SIDE = 5;
	FractalSection** currentSections;

	int resolution, resolutionSectionCount, shaderType;
	int currentCornerX = 0, currentCornerY = 0;
	FractalData::InitialCondition ic;

	friend struct CompassStruct;
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
	int start_x;
	int start_y;
	int current_x;
	int current_y;
	int grid_width;
	int grid_height;
	int last_compass_point_x;
	int last_compass_point_y;
	char direction = POINTING_RIGHT;
	int length = 5;
	int compass_count = 0;
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

template<class T>
class TwoDArray {
public:
	TwoDArray(int width, int height): m_width(width), m_height(height) {}
	T* getRef(int x, int y) { return &m_array[x + m_width * y]; }
protected:
	T m_array[BorderArea::size * BorderArea::size] = {};
	int m_width;
	int m_height;
};
