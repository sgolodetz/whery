/**
 * whery: Field.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/Field.h"

#include "whery/db/base/FieldManipulator.h"

namespace whery {

//#################### CONSTRUCTORS ####################

Field::Field(char *location, const FieldManipulator& manipulator, bool readOnly)
:	m_location(location), m_manipulator(manipulator), m_readOnly(readOnly)
{}

//#################### PUBLIC METHODS ####################

int Field::compare_to(const Field& other) const
{
	return m_manipulator.compare_to(m_location, other.m_manipulator, other.m_location);
}

double Field::get_double() const
{
	return m_manipulator.get_double(m_location);
}

int Field::get_int() const
{
	return m_manipulator.get_int(m_location);
}

std::string Field::get_string() const
{
	return m_manipulator.get_string(m_location);
}

void Field::set_double(double value) const
{
	ensure_writable();
	m_manipulator.set_double(m_location, value);
}

void Field::set_from(const Field& source) const
{
	ensure_writable();
	m_manipulator.set_from(m_location, source.m_manipulator, source.m_location);
}

void Field::set_int(int value) const
{
	ensure_writable();
	m_manipulator.set_int(m_location, value);
}

//#################### PRIVATE METHODS ####################

void Field::ensure_writable() const
{
	if(m_readOnly)
	{
		throw std::logic_error("Cannot modify a read-only field.");
	}
}

}
