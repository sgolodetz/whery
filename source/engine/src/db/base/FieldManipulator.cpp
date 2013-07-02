/**
 * whery: FieldManipulator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/FieldManipulator.h"

#include <typeinfo>

namespace whery {

//#################### DESTRUCTOR ####################

FieldManipulator::~FieldManipulator()
{}

//#################### PUBLIC METHODS ####################

double FieldManipulator::get_double(const char *location) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast(/*"Cannot convert field value to type 'double'."*/);
}

int FieldManipulator::get_int(const char *location) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast(/*"Cannot convert field value to type 'int'."*/);
}

std::string FieldManipulator::get_string(const char *location) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast(/*"Cannot convert field value to type 'string'."*/);
}

uuid FieldManipulator::get_uuid(const char *location) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast(/*"Cannot convert field value to type 'uuid'."*/);
}

void FieldManipulator::set_double(char *location, double value) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast(/*"Cannot set field value from type 'double'."*/);
}

void FieldManipulator::set_int(char *location, int value) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast(/*"Cannot set field value from type 'int'."*/);
}

void FieldManipulator::set_uuid(char *location, const uuid& value) const
{
	// Default implementation (intended to be overridden as necessary).
	throw std::bad_cast(/*"Cannot set field value from type 'uuid'."*/);
}

}
