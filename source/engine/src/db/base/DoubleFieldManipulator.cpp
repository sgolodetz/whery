/**
 * whery: DoubleFieldManipulator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/DoubleFieldManipulator.h"

#include <boost/lexical_cast.hpp>

namespace whery {

//#################### SINGLETON IMPLEMENTATION ####################

const DoubleFieldManipulator& DoubleFieldManipulator::instance()
{
	static DoubleFieldManipulator s_instance;
	return s_instance;
}

DoubleFieldManipulator::DoubleFieldManipulator()
{}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int DoubleFieldManipulator::alignment_requirement() const
{
	return sizeof(double);
}

int DoubleFieldManipulator::compare_to(
	const char *location,
	const FieldManipulator& otherManipulator,
	const char *otherLocation) const
{
	return compare_with_less(get_double(location), otherManipulator.get_double(otherLocation));
}

double DoubleFieldManipulator::get_double(const char *location) const
{
	return *reinterpret_cast<const double*>(location);
}

int DoubleFieldManipulator::get_int(const char *location) const
{
	return static_cast<int>(get_double(location));
}

std::string DoubleFieldManipulator::get_string(const char *location) const
{
	return boost::lexical_cast<std::string>(get_double(location));
}

void DoubleFieldManipulator::set_double(char *location, double value) const
{
	*reinterpret_cast<double*>(location) = value;
}

void DoubleFieldManipulator::set_from(
	char *location,
	const FieldManipulator& sourceManipulator,
	const char *sourceLocation) const
{
	set_double(location, sourceManipulator.get_double(sourceLocation));
}

void DoubleFieldManipulator::set_int(char *location, int value) const
{
	set_double(location, static_cast<double>(value));
}

unsigned int DoubleFieldManipulator::size() const
{
	return sizeof(double);
}

}
