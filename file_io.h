#pragma once
#include "macros.h"
#include <fstream>
#include <iostream>

INLINE std::string readFile(const std::string filename)
{
	// technically this would be a little dodgy if
	// you were reading in more variable files
	// but I'm reading the same one file every time
	// so it's always going to work and is never
	// going to be problem unless the file is changed

	// string for file data to be stored in
	std::string data;

	// reading until \0 null termination character
	std::getline(std::ifstream(filename), data, '\0');

	// if data.size is 0, then the file wasn't found
	if (data.size() == 0)
	{
		std::cerr << "No file found: " << filename << "\t\n";
	}

	// returning any found file data
	return data;
}