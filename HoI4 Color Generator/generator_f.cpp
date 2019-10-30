#include "generator_f.h"

/*
Author: Derek Warter
E-mail: derekwarter@gmail.com
Clausewitz Color Factory
Deterministically generates unreserved color values for use with the Clausewitz Engine's provincial maps.
Copyright (C) 2018 Derek Warter

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see https://www.gnu.org/licenses/.
*/

/*Reads description.csv files to find reserved values.*/

RGB getReservedList(std::string csv) {

	RGB values;
	std::ifstream definition(csv);
	unsigned int i = 0;

	/*
		Failsafe.
	*/

	if (definition.eof())
		throw std::runtime_error("Cannot access description.csv. Please make sure the file is present in the executable directory.");

	for (std::string line; std::getline(definition, line); ) {

		/*
			Read description.csv by taking second, third, and forth semicolon-separated values. Will fail if file is improperly formatted, but the correct file is supplied with the program.
		*/

		values.push_back(std::vector<std::string>());
		line = line.substr(line.find_first_of(";") + 1);

		for (unsigned int j = 0; j < 3; j++) {
			values[i].push_back(line.substr(0, line.find_first_of(";")));
			line = line.substr(line.find_first_of(";") + 1);
		}

		i++;

	}

	return values;

}

/*Collects and validates user input. Very ugly function, but it does what it must.*/

void validateUserInput(long &pCount, short &contrast, short *clampVals, std::string &confirm) {

	std::cout << "Number of province colors to generate (1-" << MAX_VALUES_TO_GENERATE << "): ";

	while (!(std::cin >> pCount) || pCount > MAX_VALUES_TO_GENERATE || pCount < 1) {
		std::cout << "Please enter a valid number: ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Minimum contrast between colors (luminance based) (1-255): ";

	while (!(std::cin >> contrast) || contrast < 1 || contrast > 255) {
		std::cout << "Please enter a valid number (1-255): ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Minimum red value (0-255): ";

	while (!(std::cin >> clampVals[0]) || clampVals[0] < 0 || clampVals[0] > 255) {
		std::cout << "Please enter a valid number (0-255): ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Minimum green value (0-255): ";

	while (!(std::cin >> clampVals[1]) || clampVals[1] < 0 || clampVals[1] > 255) {
		std::cout << "Please enter a valid number (0-255): ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Minimum blue value (0-255): ";

	while (!(std::cin >> clampVals[2]) || clampVals[2] < 0 || clampVals[2] > 255) {
		std::cout << "Please enter a valid number (0-255): ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Maximum red value (" << clampVals[0] << "-255): ";

	while (!(std::cin >> clampVals[3]) || clampVals[3] < clampVals[0] || clampVals[3] > 255) {
		std::cout << "Please enter a valid number (" << clampVals[0] << "-255): ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Maximum green value (" << clampVals[1] << "-255): ";

	while (!(std::cin >> clampVals[4]) || clampVals[4] < clampVals[1] || clampVals[4] > 255) {
		std::cout << "Please enter a valid number (" << clampVals[1] << "-255): ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Maximum blue value (" << clampVals[2] << "-255): ";

	while (!(std::cin >> clampVals[5]) || clampVals[5] < clampVals[2] || clampVals[4] > 255) {
		std::cout << "Please enter a valid number (" << clampVals[2] << "-255): ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	std::cout << "Would you like to sort the generated colors based upon brightness?" << std::endl;
	std::cout << "(Significantly increases generation time!) (y/n): ";

	while (true) {

		while (!(std::cin >> confirm)) {
			std::cout << "Please enter y or n: ";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		/*Transform input to lowercase and validate.*/

		std::transform(confirm.begin(), confirm.end(), confirm.begin(), ::tolower);

		if (confirm.compare("y") != 0 && confirm.compare("n") != 0) {
			std::cout << "Please enter y or n: ";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		break;

	}

	return;

}

/*Get integer-based contrast ratio*/

double getContrast(unsigned short r, unsigned short g, unsigned short b) {
	return (double)((299 * r + 587 * g + 114 * b) / 1000);
}

/*Generate map for reserved values so that the color generator can reject unacceptable outputs.*/

std::unordered_map<std::string, std::string> hashReservedList(RGB values) {

	std::unordered_map<std::string, std::string> hashMap;

	for (unsigned long i = 0; i < values.size(); i++) {
		std::string rgb = values[i][0] + values[i][1] + values[i][2];
		hashMap[rgb] = rgb;
	}

	return hashMap;

}

/*Horribly inefficient means of sorting RGB values by luminance.
Unfortunately merge sort and quicksort quickly overwhelm the callstack, and insertion sort does not offer much of an improvement.
Given that most users will attempt to generate less than 1000 values, this is an acceptable algorithm.*/

void selectionRGBSort(RGB &values) {

	std::cout << "Sorting colors by luminance..." << std::endl;

	int size = values.size();

	for (int j = 0; j < size - 1; j++) {

		int iMin = j;

		for (int i = j + 1; i < size; i++) {

			std::vector<std::string> compTarget = values[i];
			std::vector<std::string> assumedLowest = values[iMin];
			unsigned short ctR = (unsigned short)std::stoi(compTarget[0]);
			unsigned short ctG = (unsigned short)std::stoi(compTarget[1]);
			unsigned short ctB = (unsigned short)std::stoi(compTarget[2]);
			unsigned short alR = (unsigned short)std::stoi(assumedLowest[0]);
			unsigned short alG = (unsigned short)std::stoi(assumedLowest[1]);
			unsigned short alB = (unsigned short)std::stoi(assumedLowest[2]);

			if (getContrast(ctR, ctG, ctB) < getContrast(alR, alG, alB))
				iMin = i;
			

		}

		if (iMin != j) {
			std::vector<std::string> temp = values[j];
			values[j] = values[iMin];
			values[iMin] = temp;
		}

		int percent = ((double)j / (double)(size - 2)) * 100;
		std::cout << percent << "%\r" << std::flush;

	}

}

/*
	Combinatorial method of generating original color values.
	This functions by stepping through color values similarly to, but not exactly like, an odomoter.
	Starts with red values, then blue values, then green values.
	Each time the odometer "completes" a cycle of 768 colors, returns to red and increases other colors by a set amount.
	This ensures that every possible color in the gamut can be generated.
	generateUnreservedValues can reject a color that the odometer generates if it is reserved or already generated.
*/

void colorOdometer(unsigned short &r, unsigned short &g, unsigned short &b, int step, short *clampVals, unsigned short &min, unsigned short &colorTurn, bool &justTurned) {

	switch (colorTurn) {
	case 0:
		if (r == (clampVals[0] + min) && justTurned) {
			r = clampVals[0];
			justTurned = false;
		}
		if (r < clampVals[3]) {
			r = std::min(r + step, (int)clampVals[3]);
			b = clampVals[2] + min;
			g = clampVals[1] + min;
		}
		else {
			colorTurn++;
			r = clampVals[0] + min;
			justTurned = true;
		}
		break;
	case 1:
		if (g == (clampVals[1] + min) && justTurned) {
			g = clampVals[1];
			justTurned = false;
		}
		if (g < clampVals[4]) {
			g = std::min(g + step, (int)clampVals[4]);
			r = clampVals[0] + min;
			b = clampVals[2] + min;
		}
		else {
			colorTurn++; g = clampVals[1] + min;
			justTurned = true;
		}
		break;
	case 2:
		if (b == (clampVals[2] + min) && justTurned) {
			b = clampVals[2];
			justTurned = false;
		}
		if (b < clampVals[5]) {
			b = std::min(b + step, (int)clampVals[5]);
			r = clampVals[0] + min;
			g = clampVals[1] + min;
		}
		else {
			colorTurn = 0; min += step;
			r = clampVals[0];
			g = clampVals[1] + min;
			b = clampVals[2] + min;
			justTurned = true;
		}
		break;
	}

}

RGB generateUnreservedValues(std::unordered_map<std::string, std::string> hashMap, long pCount, short mContrast, short *clampVals) {

	RGB newValues;											// Unreserved color values to generate.

	long i = 0;												// How many colors have been successfully generated? Stop when we generate the number of colors the user wants.
	long timeout = 0;										// Stop if timeout value exceeds MAX_VALUES_TO_ATTEMPT.
	unsigned short min = 0;									// Helps the color odometer update each time a cycle is completed.
	unsigned short colorTurn = 0;							// Which color is being focused upon? 0 = red, 1 = green, 2 = blue.
	bool justTurned = true;									// Did the odometer just turn over?
	unsigned short r = clampVals[0] + min;					// Starting r value (clamped to user-set minimum).
	unsigned short g = clampVals[1] + min;					// Starting b value (clamped to user-set minimum).
	unsigned short b = clampVals[2] + min;					// Starting c value (clamped to user-set minimum).
	std::unordered_map<std::string, std::string> noDupes;	// Ensures that no duplicate colors are generated by the odometer in the case that the user has clamped colors/set a high minimum contrast.
	double previousContrast = 0;							// Store the luminance value of the previously accepted RGB value. If the next generated value is too close to the old one, then reject it until we reach an acceptable level of contrast.
	std::cout << "Generating unreserved colors..." << std::endl;

	while (i < pCount) {

		/* Get the contrast of the current color values. Determine how much the rgb values need to be changed based on difference between luminances. */

		double curContrast = std::abs(getContrast(r, g, b) - previousContrast);
		int desiredContrastStep = std::max((int)(mContrast - curContrast), 1);

		colorOdometer(r, g, b, desiredContrastStep, clampVals, min, colorTurn, justTurned);

		/* Concatenate color values into string key that can be compared against hashes. */

		std::string rs = std::to_string(r);
		std::string gs = std::to_string(g);
		std::string bs = std::to_string(b);
		std::string concateRGB = rs + gs + bs;

		if (hashMap.find(concateRGB) == hashMap.end() && noDupes.find(concateRGB) == noDupes.end()) {

			/*If key matches neither hash for reserved values or hash for accepted values, then add current rgb values to newValues RGB vector.*/

			newValues.push_back(std::vector<std::string>());
			newValues[i].push_back(rs);
			newValues[i].push_back(gs);
			newValues[i].push_back(bs);
			i++;
			previousContrast = curContrast;
			int percent = (((double)i / (double)(pCount - 1)) * 100);
			std::cout << percent << "%\r" << std::flush;
			noDupes[concateRGB] = concateRGB;
		}

		timeout++;

		if (timeout > MAX_VALUES_TO_ATTEMPT)
			throw std::runtime_error("Could not generate requested colors.");


	}

	return newValues;

}

/*Convert integer RGB values to hex codes.*/
/*https://stackoverflow.com/questions/14375156/how-to-convert-a-rgb-color-value-to-an-hexadecimal-value-in-c*/

unsigned long toRGB(std::vector<std::string> color) {

	int r = std::stoi(color[0]);
	int g = std::stoi(color[1]);
	int b = std::stoi(color[2]);

	return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

}

/*Convert RGB vector to text file.*/

void RGBToText(RGB values) {

	std::ofstream outText("unreserved.txt");

	if (outText) {

		outText << "(R, G, B)" << std::endl;

		for (unsigned long i = 0; i < values.size(); i++) {
			unsigned long hex = toRGB(values[i]);
			outText << "(" << values[i][0] << ", " << values[i][1] << ", " << values[i][2] << ") " << std::hex << hex << std::endl;
		}

	}

	outText.close();

}

/*Get smallest 1:1 image size to contain all generated values.*/
/*Consider starting at numColors and then decrementing until we find a value with a root.*/

int squarePalette(int numColors) {

	int i = 1;

	while (i * i < numColors) {
		i++;
	}

	return i;

}

/*https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries*/

void RGBToBMP(RGB values, int squaredColors) {

	FILE *outImg;
	unsigned char *img = NULL;
	int filesize = 54 + 3 * squaredColors * squaredColors;

	img = (unsigned char *)malloc(3 * squaredColors * squaredColors);
	memset(img, 0, 3 * squaredColors * squaredColors);
	int pixel = 0; // Since we're taking from a 2d vector where each column is a complete RGB value,
				   // we simply want to increment one value to get the color.
				   // The nested loop controls where that color goes.

	for (int w = 0; w < squaredColors; w++) {
		for (int h = 0; h < squaredColors; h++) {
			int x = w; int y = (squaredColors - 1) - h;
			int r;
			int g;
			int b;
			if (pixel >= values.size()) {
				r = 255;
				g = 255;
				b = 255;
			}
			else {
				r = std::stoi(values[pixel][0]);
				g = std::stoi(values[pixel][1]);
				b = std::stoi(values[pixel][2]);
				pixel++;
			}
			img[(x + y * squaredColors) * 3 + 2] = (unsigned char)(r);
			img[(x + y * squaredColors) * 3 + 1] = (unsigned char)(g);
			img[(x + y * squaredColors) * 3 + 0] = (unsigned char)(b);
		}
	}

	unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
	unsigned char bmppad[3] = { 0,0,0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(squaredColors);
	bmpinfoheader[5] = (unsigned char)(squaredColors >> 8);
	bmpinfoheader[6] = (unsigned char)(squaredColors >> 16);
	bmpinfoheader[7] = (unsigned char)(squaredColors >> 24);
	bmpinfoheader[8] = (unsigned char)(squaredColors);
	bmpinfoheader[9] = (unsigned char)(squaredColors >> 8);
	bmpinfoheader[10] = (unsigned char)(squaredColors >> 16);
	bmpinfoheader[11] = (unsigned char)(squaredColors >> 24);


	errno_t err = fopen_s(&outImg, "unreserved.bmp", "wb");
	fwrite(bmpfileheader, 1, 14, outImg);
	fwrite(bmpinfoheader, 1, 40, outImg);

	for (int i = 0; i < squaredColors; i++) {
		fwrite(img + (squaredColors*(squaredColors - i - 1) * 3), 3, squaredColors, outImg);
		fwrite(bmppad, 1, (4 - (squaredColors * 3) % 4) % 4, outImg);
	}

	free(img);
	fclose(outImg);

}
