/**
 * whery: IntFieldManipulator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/IntFieldManipulator.h"

#include <boost/lexical_cast.hpp>

namespace whery {

//#################### SINGLETON IMPLEMENTATION ####################

const IntFieldManipulator& IntFieldManipulator::instance()
{
	static IntFieldManipulator s_instance;
	return s_instance;
}

IntFieldManipulator::IntFieldManipulator()
{}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int IntFieldManipulator::alignment_requirement() const
{
	return sizeof(int);
}

int IntFieldManipulator::compare_to(
	const char *location,
	const FieldManipulator& otherManipulator,
	const char *otherLocation) const
{
	return compare_with_less(get_int(location), otherManipulator.get_int(otherLocation));
}

double IntFieldManipulator::get_double(const char *location) const
{
	return static_cast<double>(get_int(location));
}

int IntFieldManipulator::get_int(const char *location) const
{
	return *reinterpret_cast<const int*>(location);
}

std::string IntFieldManipulator::get_string(const char *location) const
{
	return boost::lexical_cast<std::string>(get_int(location));
}

void IntFieldManipulator::set_double(char *location, double value) const
{
	set_int(location, static_cast<int>(value));
}

void IntFieldManipulator::set_from(
	char *location,
	const FieldManipulator& sourceManipulator,
	const char *sourceLocation) const
{
	set_int(location, sourceManipulator.get_int(sourceLocation));
}

void IntFieldManipulator::set_int(char *location, int value) const
{
	*reinterpret_cast<int*>(location) = value;
}

unsigned int IntFieldManipulator::size() const
{
	return sizeof(int);
}

}
