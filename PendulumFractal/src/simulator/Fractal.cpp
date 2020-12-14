
#include "Fractal.h"
#include "openGL/GpuInterface.h"
#include "openGL/FractalSection.h"

#include <math.h>
#include <iostream>

Fractal::Fractal(int resolution, int shaderType, FractalData::InitialCondition ic) {
	this->currentSections = new FractalSection * [CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE];
	for (int i = 0; i < CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE; i++)
		this->currentSections[i] = nullptr;

	this->resolution = resolution;
	this->resolutionSectionCount = (int)ceil((float)resolution / (float)FractalData::MAX_FRACTAL_SECTION_SIDE);
	this->shaderType = shaderType;
	this->ic = ic;	// ändra ic till pointer och kopiera över denna
}

void Fractal::loadCurrentSection(int xIn, int yIn, unsigned char* generate) {

	int cornerX = xIn / FractalData::MAX_FRACTAL_SECTION_SIDE;
	int cornerY = yIn / FractalData::MAX_FRACTAL_SECTION_SIDE;
	if (xIn < 0 && xIn % FractalData::MAX_FRACTAL_SECTION_SIDE != 0)
		cornerX--;
	if (yIn < 0 && yIn % FractalData::MAX_FRACTAL_SECTION_SIDE != 0)
		cornerY--;

	cornerX *= FractalData::MAX_FRACTAL_SECTION_SIDE;
	cornerY *= FractalData::MAX_FRACTAL_SECTION_SIDE;

	if (cornerX != currentCornerX || cornerY != currentCornerY) {
		shiftCurretSections((currentCornerX - cornerX) / FractalData::MAX_FRACTAL_SECTION_SIDE, (currentCornerY - cornerY) / FractalData::MAX_FRACTAL_SECTION_SIDE);
	}

	this->currentCornerX = cornerX;
	this->currentCornerY = cornerY;

	for (int y = -(CURRENT_SECTIONS_SIDE / 2); y <= (CURRENT_SECTIONS_SIDE / 2); y++) {
		for (int x = -(CURRENT_SECTIONS_SIDE / 2); x <= (CURRENT_SECTIONS_SIDE / 2); x++) {
			if (generate[(y + (CURRENT_SECTIONS_SIDE / 2)) * CURRENT_SECTIONS_SIDE + (x + (CURRENT_SECTIONS_SIDE / 2))] == 0)
				continue;

			FractalData::Section s = { ((float)x * (float)FractalData::MAX_FRACTAL_SECTION_SIDE + cornerX) / (float)resolution,
				((float)y * (float)FractalData::MAX_FRACTAL_SECTION_SIDE + cornerY) / (float)resolution,
				(float)FractalData::MAX_FRACTAL_SECTION_SIDE / (float)resolution, (float)FractalData::MAX_FRACTAL_SECTION_SIDE / (float)resolution };

			currentSections[(y + (CURRENT_SECTIONS_SIDE / 2)) * CURRENT_SECTIONS_SIDE + (x + (CURRENT_SECTIONS_SIDE / 2))] = new FractalSection(GpuInterface::shaders[shaderType],
				FractalData::MAX_FRACTAL_SECTION_SIDE, FractalData::MAX_FRACTAL_SECTION_SIDE,
				&s, &ic);
		}
	}
}
unsigned char Fractal::getBatchChar(int x, int y) {
	x += FractalData::MAX_FRACTAL_SECTION_SIDE;
	y += FractalData::MAX_FRACTAL_SECTION_SIDE;

	int computeX = (x - currentCornerX) + FractalData::MAX_FRACTAL_SECTION_SIDE;
	int computeY = (y - currentCornerY) + FractalData::MAX_FRACTAL_SECTION_SIDE;

	if (computeX >= CURRENT_SECTIONS_SIDE * FractalData::MAX_FRACTAL_SECTION_SIDE ||
		computeY >= CURRENT_SECTIONS_SIDE * FractalData::MAX_FRACTAL_SECTION_SIDE ||
		computeX < 0 || computeY < 0) {
		unsigned char* generate = new unsigned char[CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE];
		for (int i = 0; i < CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE; i++)
			generate[i] = 0;
		generate[CURRENT_SECTIONS_SIDE * (CURRENT_SECTIONS_SIDE / 2) + (CURRENT_SECTIONS_SIDE / 2 + 1)] = 1;

		loadCurrentSection(x, y, generate);

		std::cout << x << " | " << y << std::endl;

		computeX = (x - currentCornerX) + FractalData::MAX_FRACTAL_SECTION_SIDE;
		computeY = (y - currentCornerY) + FractalData::MAX_FRACTAL_SECTION_SIDE;
	}

	int cellX = computeX % FractalData::MAX_FRACTAL_SECTION_SIDE;
	int cellY = computeY % FractalData::MAX_FRACTAL_SECTION_SIDE;

	int sectionX = computeX / FractalData::MAX_FRACTAL_SECTION_SIDE;
	int sectionY = computeY / FractalData::MAX_FRACTAL_SECTION_SIDE;

	if (currentSections[sectionY * CURRENT_SECTIONS_SIDE + sectionX] == nullptr) {
		unsigned char* generate = new unsigned char[CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE];
		for (int i = 0; i < CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE; i++)
			generate[i] = 0;
		generate[sectionY * CURRENT_SECTIONS_SIDE + sectionX] = 1;
		loadCurrentSection(this->currentCornerX, this->currentCornerY, generate);
	}

	return currentSections[sectionY * CURRENT_SECTIONS_SIDE + sectionX]->getBuffer()[cellY * FractalData::MAX_FRACTAL_SECTION_SIDE + cellX];
}
/*unsigned char Fractal::getChar() {
    
}*/

/*BorderArea Fractal::getBorderArea(int border_area_start_x, int border_area_start_y, std::list<Position> border_crossings) {
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
				compass_info.start_y == compass_info.current_y) &&
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
}*/

void CompassStruct::getNextPoint(Fractal* fractal) {
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
			fractal->getBatchChar(pointing_x_pos, pointing_y_pos) == 0) {
			current_x = pointing_x_pos;
			current_y = pointing_y_pos;
			return;
		}

		direction = (direction + spin + 8) % 8;
	}
}

/*std::list<Position> Fractal::getCompassBorderCrossing() {
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

	if (border_crossings.size() == 0) {
		border_crossings.push_back(Position(
			compass_info.current_x,
			compass_info.current_y,
			compass_info.direction));
	}

	return border_crossings;
}*/

double Fractal::getCompassDimension(int compass_length) {
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

void Fractal::shiftCurretSections(int shiftX, int shiftY) {
	FractalSection** newSections = new FractalSection * [CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE];

	for (int y = 0; y < CURRENT_SECTIONS_SIDE; y++) {
		for (int x = 0; x < CURRENT_SECTIONS_SIDE; x++) {
			int getX = x - shiftX;
			int getY = y - shiftY;


			if (getX < 0 || getX >= CURRENT_SECTIONS_SIDE || getY < 0 || getY >= CURRENT_SECTIONS_SIDE) {
				delete currentSections[(CURRENT_SECTIONS_SIDE - 1 - y) * CURRENT_SECTIONS_SIDE + (CURRENT_SECTIONS_SIDE - 1 - x)];
				newSections[y * CURRENT_SECTIONS_SIDE + x] = nullptr;
			}
			else {
				newSections[y * CURRENT_SECTIONS_SIDE + x] = currentSections[getY * CURRENT_SECTIONS_SIDE + getX];
			}
		}
	}

	delete currentSections;
	currentSections = newSections;
}

unsigned long long Fractal::countBoxes(unsigned char boxType) {
	/*for (int a = 0; a < FractalData::MAX_FRACTAL_SECTION_SIDE * 3; a++) {
		for (int b = 0; b < FractalData::MAX_FRACTAL_SECTION_SIDE * 3; b++) {
			if (getBatchChar(b - FractalData::MAX_FRACTAL_SECTION_SIDE, a - FractalData::MAX_FRACTAL_SECTION_SIDE) == 0)
				std::cout << "-";
			else
				std::cout << "#";
		}
		std::cout << std::endl;
	}*/
	for (int y = 0; y < resolution; y++) {
		for (int x = 0; x < resolution; x++) {
			if (getBatchChar(x, y) == 0)
				std::cout << "-";
			else
				std::cout << "#";
		}
		std::cout << std::endl;
	}
	return 0;
}
/*BorderArea Fractal::countEdges(unsigned char boxType) {

	std::list<Position> crossings = getCompassBorderCrossing();
	BorderArea border_info = getBorderArea(0, 0, crossings);

	return border_info;

	long long edgeBoxes = 0;


	std::cout << resolutionSectionCount << std::endl;

	for (int sectionY = 0; sectionY < resolutionSectionCount; sectionY++) {
		for (int sectionX = 0; sectionX < resolutionSectionCount; sectionX++) {

			//std::cout << "generating: " << sectionX << " | " << sectionY << std::endl;
			//std::cout << "sections: " << sectionX << " | " << sectionY << std::endl;

			for (int inY = 0; inY < FractalData::MAX_FRACTAL_SECTION_SIDE; inY++) {
				for (int inX = 0; inX < FractalData::MAX_FRACTAL_SECTION_SIDE; inX++) {
					int x = (sectionX * FractalData::MAX_FRACTAL_SECTION_SIDE) + inX;
					int y = (sectionY * FractalData::MAX_FRACTAL_SECTION_SIDE) + inY;

					if (y >= resolution)
						goto CONTINUE_LARGE_LOOP;
					if (x >= resolution)
						break;

					//getBatchChar(x, y);

					for (int smallY = -1; smallY < 2; smallY++) {
						for (int smallX = -1; smallX < 2; smallX++) {
							if (smallX == 0 && smallY == 0)
								continue;

							if (getBatchChar(x + smallX, y + smallY) != boxType) {
								if (getBatchChar(x, y) == boxType) {
									edgeBoxes++;
									goto BOX_COUNTED;
								}
							}
						}
					}
				BOX_COUNTED:
					inX = inX * 1;
					//std::cout << "box counted" << std::endl;

				}
			}

			//std::cout << "outside generation" << std::endl;

		CONTINUE_LARGE_LOOP:
			sectionX = sectionX * 1;
			//std::cout << "continue large loop" << std::endl;
		}
	}


	return edgeBoxes;
}*/
