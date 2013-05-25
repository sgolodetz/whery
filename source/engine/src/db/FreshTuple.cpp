/**
 * whery: FreshTuple.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/FreshTuple.h"

namespace whery {

//#################### CONSTRUCTORS ####################

FreshTuple::FreshTuple(const std::vector<const FieldManipulator*>& fieldManipulators)
:	BackedTuple(TupleManipulator(fieldManipulators))
{
	set_fresh_buffer(m_manipulator.size());
}

FreshTuple::FreshTuple(const TupleManipulator& manipulator)
:	BackedTuple(manipulator)
{
	set_fresh_buffer(manipulator.size());
}

//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################

FreshTuple::FreshTuple(const FreshTuple& rhs)
:	BackedTuple(rhs.m_manipulator)
{
	set_buffer(rhs.m_buffer);
}

FreshTuple& FreshTuple::operator=(const FreshTuple& rhs)
{
	m_manipulator = rhs.m_manipulator;
	set_buffer(rhs.m_buffer);
	return *this;
}

//#################### PRIVATE METHODS ####################

void FreshTuple::set_buffer(const std::vector<char>& buffer)
{
	m_buffer = buffer;
	m_location = &m_buffer[0];
}

void FreshTuple::set_fresh_buffer(unsigned int size)
{
	m_buffer = std::vector<char>(size);
	m_location = &m_buffer[0];
}

}
