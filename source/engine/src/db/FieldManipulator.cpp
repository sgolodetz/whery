/**
 * whery: FieldManipulator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/FieldManipulator.h"

#include <typeinfo>

namespace whery {

//#################### DESTRUCTOR ####################

FieldManipulator::~FieldManipulator()
{}

//#################### PUBLIC METHODS ####################

double FieldManipulator::get_double(const char *location) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast("Cannot convert field value to type 'double'.");
}

int FieldManipulator::get_int(const char *location) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast("Cannot convert field value to type 'int'.");
}

std::string FieldManipulator::get_string(const char *location) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast("Cannot convert field value to type 'string'.");
}

void FieldManipulator::set_double(char *location, double value) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast("Cannot set field value from type 'double'.");
}

void FieldManipulator::set_int(char *location, int value) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast("Cannot set field value from type 'int'.");
}

}
