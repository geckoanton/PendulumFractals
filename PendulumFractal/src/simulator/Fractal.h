#pragma once
#include <math.h>
#include <iostream>
#include <list>
#include <vector>

class FractalSection;

struct BorderArea {
	static const int size = 64;
	static const int box_dimension_count = 6;

	// (1: 0), (2: 1), (4: 2), (8: 3) ...
	// Box count for different sizes
	int boxSize[box_dimension_count] = {};
};

namespace FractalData {
	enum Type {
		flipFractal,
		size
	};

	struct Section {
		float cornerX, cornerY;
		float spanX, spanY;
	};
	struct InitialCondition {
		float m1, m2;
		float l1, l2;
		float g;
	};
};

struct Position {
	int x;
	int y;
	char dir; // Direction
	Position(int t_x, int t_y, int direction): x(t_x), y(t_y), dir(direction) {}
};

class Fractal {
public:
	Fractal(int resolution, int shaderType, FractalData::InitialCondition ic);

	unsigned long long countBoxes(unsigned char boxType);
	unsigned long long countEdges(unsigned char boxType);
	double getCompassDimension(int compass_length);
	std::list<Position> getCompassBorderCrossing();
	BorderArea getBorderArea(int border_area_start_x, int border_area_start_y, std::list<Position> border_crossings);

	// For testing only
	unsigned char* temp_char_arr = nullptr;
	unsigned char getCharArr(int x, int y) {
		return temp_char_arr[(((x % 4096) + 4096) % 4096) + (((y % 4096) + 4096) % 4096) * 4096];
	}
private:
	void loadCurrentSection(int xIn, int yIn, unsigned char* generate);
	//unsigned char getCharArr(int x, int y);

	void shiftCurretSections(int shiftX, int shiftY);

	const int CURRENT_SECTIONS_SIDE = 5;
	FractalSection** currentSections;

	int resolution, resolutionSectionCount, shaderType;
	int currentCornerX = 0, currentCornerY = 0;
	FractalData::InitialCondition ic;

	friend struct CompassStruct;
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
	void getNextPoint(Fractal* fractal) {
		direction = (direction + 4 + spin) % 8;
		for (int i = 0; i < 8; i++) {
			int pointing_x_pos;
			int pointing_y_pos;
			if (direction == POINTING_UP_LEFT ||
				direction == POINTING_LEFT ||
				direction == POINTING_BOTTOM_LEFT) {
				pointing_x_pos = current_x - 1;
			}
			else if (direction == POINTING_UP_RIGHT ||
				direction == POINTING_RIGHT ||
				direction == POINTING_BOTTOM_RIGHT) {
				pointing_x_pos = current_x + 1;
			}
			else {
				pointing_x_pos = current_x;
			}

			if (direction == POINTING_UP_LEFT ||
				direction == POINTING_UP ||
				direction == POINTING_UP_RIGHT) {
				pointing_y_pos = current_y - 1;
			}
			else if (direction == POINTING_BOTTOM_LEFT ||
				direction == POINTING_BOTTOM ||
				direction == POINTING_BOTTOM_RIGHT) {
				pointing_y_pos = current_y + 1;
			}
			else {
				pointing_y_pos = current_y;
			}

			if (pointing_x_pos >= 0 && pointing_x_pos < grid_width &&
				pointing_y_pos >= 0 && pointing_y_pos < grid_height &&
				fractal->getCharArr(pointing_x_pos, pointing_y_pos) == 0) {
				current_x = pointing_x_pos;
				current_y = pointing_y_pos;
				return;
			}

			direction = (direction + spin + 8) % 8;
		}
	}
};

inline double Fractal::getCompassDimension(int compass_length) {
	// Create a copy of the old grid
	CompassStruct compass_info;

	compass_info.start_x = resolution / 2;
	compass_info.start_y = 0;
	compass_info.length = compass_length;

	// Reset the compass starting pos
	compass_info.last_compass_point_x = compass_info.start_x;
	compass_info.last_compass_point_y = compass_info.start_y;
	compass_info.current_x = compass_info.start_x;
	compass_info.current_y = compass_info.start_y;
	compass_info.grid_height = resolution;
	compass_info.grid_width = resolution;

	// Get the next point, walking around the fractal counter clockwise
	compass_info.getNextPoint(this);

	// Iterate while it has not gone around the fractal
	while (!(compass_info.start_x == compass_info.current_x &&
		compass_info.start_y == compass_info.current_y)) {
		// Get the next point, walking around the fractal counter clockwise
		compass_info.getNextPoint(this);

		// See if the point is the right length from the last compass point
		if (pow(compass_info.last_compass_point_x - compass_info.current_x, 2) +
			pow(compass_info.last_compass_point_y - compass_info.current_y, 2) >=
			pow(compass_info.length, 2)) {
			// Increase the compass count
			compass_info.compass_count++;

			// Update the compass position to check the distance from
			compass_info.last_compass_point_x = compass_info.current_x;
			compass_info.last_compass_point_y = compass_info.current_y;

			if (compass_info.compass_count > compass_info.grid_width* compass_info.grid_height) {
				goto exit_walkWithCompass;
			}
		}
	}
exit_walkWithCompass:
	//std::cout << "Compass count: " << compass_info.compass_count << std::endl;
	double length_left = pow((double)pow(compass_info.last_compass_point_x - compass_info.start_x, 2) +
		pow(compass_info.last_compass_point_y - compass_info.start_y, 2), 0.5);
	//std::cout << "To begin: " << length_left << std::endl;

	double compass_count = compass_info.compass_count + length_left / compass_info.length;
	std::cout << "Compass Count: " << compass_count << std::endl;
	
	return compass_count;
}

inline std::list<Position> Fractal::getCompassBorderCrossing() {
	int border_size = BorderArea::size;
	std::list<Position> border_crossings = {};
	// Create a copy of the old grid
	CompassStruct compass_info;

	compass_info.start_x = resolution / 2;
	compass_info.start_y = 0;
	compass_info.length = 1;

	// Reset the compass starting pos
	compass_info.last_compass_point_x = compass_info.start_x;
	compass_info.last_compass_point_y = compass_info.start_y;
	compass_info.current_x = compass_info.start_x;
	compass_info.current_y = compass_info.start_y;
	compass_info.grid_height = resolution;
	compass_info.grid_width = resolution;

	// Get the next point, walking around the fractal counter clockwise
	compass_info.getNextPoint(this);

	int border_area_x = compass_info.current_x / border_size;
	int border_area_y = compass_info.current_y / border_size;

	// Iterate while it has not gone around the fractal
	while (!(compass_info.start_x == compass_info.current_x &&
		compass_info.start_y == compass_info.current_y)) {
		// Get the next point, walking around the fractal counter clockwise
		compass_info.getNextPoint(this);

		if (border_area_x != compass_info.current_x / border_size ||
			border_area_y != compass_info.current_y / border_size) {
			border_area_x = compass_info.current_x / border_size;
			border_area_y = compass_info.current_y / border_size;
			border_crossings.push_back(Position(
				compass_info.current_x, 
				compass_info.current_y, 
				compass_info.direction));
		}
	}
	return border_crossings;
}


template<class T>
class TwoDArray {
public:
	TwoDArray(int width, int height): m_width(width), m_height(height) {
		//m_array.resize(m_width * m_height);
		//m_array.clear();
	}
	T* getRef(int x, int y) {
		return &m_array[x + m_width * y];
	}
protected:
	T m_array[BorderArea::size * BorderArea::size] = {};
	int m_width;
	int m_height;
};

inline BorderArea Fractal::getBorderArea(int border_area_start_x, int border_area_start_y, std::list<Position> border_crossings) {
	BorderArea border_area;
	auto arr = new TwoDArray<bool>(BorderArea::size, BorderArea::size);

	for (auto it = border_crossings.begin(); it != border_crossings.end(); it++) {
		if ((*it).x - border_area_start_x >= 0 && (*it).x - border_area_start_x < BorderArea::size &&
			(*it).y - border_area_start_y >= 0 && (*it).y - border_area_start_y < BorderArea::size) {
			// Entrance detected!
			CompassStruct compass_info;

			compass_info.start_x = (*it).x;
			compass_info.start_y = (*it).y;
			compass_info.direction = (*it).dir;
			compass_info.length = 1;

			// Reset the compass starting pos
			compass_info.last_compass_point_x = compass_info.start_x;
			compass_info.last_compass_point_y = compass_info.start_y;
			compass_info.current_x = compass_info.start_x;
			compass_info.current_y = compass_info.start_y;
			compass_info.grid_height = resolution;
			compass_info.grid_width = resolution;

			// Mark the point
			*arr->getRef(compass_info.current_x - border_area_start_x,
				compass_info.current_y - border_area_start_y) = true;

			// Get the next point, walking around the fractal counter clockwise
			compass_info.getNextPoint(this);

			// Iterate while it has not gone around the fractal
			while (!(compass_info.start_x == compass_info.current_x &&
				compass_info.start_y == compass_info.current_y ) &&
				compass_info.current_x - border_area_start_x >= 0 &&
				compass_info.current_x - border_area_start_x < BorderArea::size &&
				compass_info.current_y - border_area_start_y >= 0 &&
				compass_info.current_y - border_area_start_y < BorderArea::size
				) {

				// Mark the point
				*arr->getRef(compass_info.current_x - border_area_start_x,
					compass_info.current_y - border_area_start_y) = true;

				// Get the next point, walking around the fractal counter clockwise
				compass_info.getNextPoint(this);
			}
		}
	}

	// Do some box counting
	int box_size = 1;
	for (int i = 0; i < BorderArea::box_dimension_count; i++) {
		for (int x = 0; x < BorderArea::size; x += box_size) {
			for (int y = 0; y < BorderArea::size; y += box_size) {
				for (int x_box = x; x_box < box_size + x; x_box += 1) {
					for (int y_box = y; y_box < box_size + y; y_box += 1) {
						if (*arr->getRef(x_box, y_box)) {
							border_area.boxSize[i] += 1;
							goto break_box_loop;
						}
					}
				}
				break_box_loop:
				int _; // To use goto
			}
		}

		box_size = box_size * 2;
	}
	return border_area;
}

