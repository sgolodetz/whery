/**
 * wcl: main.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <iostream>

#include "whery/util/AlignmentTracker.h"

int main()
{
	whery::AlignmentTracker at;
	std::cout << at.max_alignment() << '\n';
	return 0;
}
