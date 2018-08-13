#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <math.h>

/*
Author: Isan Nyat
E-mail: syringe232@gmail.com
Clausewitz Color Factory
Deterministically generates unreserved color values for use with the Clausewitz Engine's provincial maps.
Copyright (C) 2018 Isan Nyat

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

/*
RGB type is a 2d vector containing an R, G, and B value in each column.
MAX_VALUES_TO_GENERATE limits the number of colors the user is allowed to generate. Can be safely changed, but attempting to generate too many values will induce slowdown.
MAX_VALUES_TO_ATTEMPT limits number of generation attempts to the size of the 24-bit RGB spectrum. Changing this could cause unexpected behavior.
*/

typedef std::vector<std::vector<std::string> > RGB;
const int MAX_VALUES_TO_GENERATE = 50000;
const int MAX_VALUES_TO_ATTEMPT = 16777216;

RGB getReservedList(std::string csv);

void validateUserInput(long &pCount, short &contrast, short *clampVals, std::string &confirm);

double getContrast(unsigned short r, unsigned short g, unsigned short b);

std::unordered_map<std::string, std::string> hashReservedList(RGB values);

void selectionRGBSort(RGB &values);

void colorOdometer(unsigned short &r, unsigned short &g, unsigned short &b, int step, short *clampVals, unsigned short &min, unsigned short &colorTurn, bool &justTurned);

RGB generateUnreservedValues(std::unordered_map<std::string, std::string> hashMap, long pCount, short mContrast, short *clampVals);

unsigned long toRGB(std::vector<std::string> color);

void RGBToText(RGB values);

int squarePalette(int numColors);

void RGBToBMP(RGB values, int squaredColors);
