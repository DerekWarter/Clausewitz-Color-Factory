#include <iostream>
#include <iomanip>
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

int main() {

	RGB values;												// Reserved values to acquire.
	RGB unreservedValues;									// Values to generate.
	long pCount;											// Number of provinces (colors) to generate.
	short mContrast;										// Minimum contrast between luminance values.
	short clampVals[6] = { 0, 0, 0, 255, 255, 255 };		// Minimum and maximum RGB values.
	short priorityVals[3] = { 1, 0, 0 };					// UNUSED.
	std::string confirm;									// User input determining whether or not to sort. DEPRECATED.
	long maxFind;											// Max values to look for based upon number of provinces found. Deprecated.
	std::unordered_map<std::string, std::string> hashMap;	// Hash table containing keys for all reserved colors.

	std::cout << "Clausewitz Color Factory" << std::endl << std::endl;

	/*
		Start by acquiring the list of reserved colors. If none is found, the software will simply generate every color in 24-bit space. Try/catch is simply a failsafe.
	*/

	try {
		values = getReservedList("definition.csv");
	}
	catch (...) {
		std::cout << "Could not open definition.csv. Please ensure the file is in the installation directory.";
		std::cin.get();
		return 0;
	}

	std::cout << "There are " << values.size() << " provinces in the definitions.csv file." << std::endl;

	// maxFind = MAX_VALUES_TO_GENERATE - values.size(); DEPRECATED.

	/*
		Keep seeking user input until we get the right values.
	*/

	while (true) {

		validateUserInput(pCount, mContrast, clampVals, confirm);

		/*
			Build hashMap from the reserved values RGB vector.
		*/

		hashMap = hashReservedList(values);

		try {

			/*
				Generate unused values. Sort them if the user desires. 
				Catch exception thrown if MAX_VALUES_TO_ATTEMPT is reached by the timeout counter in generateUnreservedValues.
			*/

			unreservedValues = generateUnreservedValues(hashMap, pCount, mContrast, clampVals);
			if (confirm.compare("y") == 0)
				selectionRGBSort(unreservedValues);
		}
		catch (...) {
			std::cout << "Could not generate the desired number of values.\nTry reducing minimum contrast or desired value count." << std::endl << std::endl;
			continue;
		}

		break;

	}

	std::cout << "\nGenerated " << unreservedValues.size() << " colors. Writing to file..." << std::endl;

	/*
		Write values to text-based list and a BMP output image.
	*/

	RGBToText(unreservedValues);
	int imgSize = squarePalette((int)pCount);
	RGBToBMP(unreservedValues, imgSize);

	std::cout << "Done! Press any key to exit." << std::endl;

	std::cin.get();
	std::cin.get();

	return 0;

}