/**
 * whery: FreshRecord.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/FreshRecord.h"

namespace whery {

//#################### CONSTRUCTORS ####################

FreshRecord::FreshRecord(const std::vector<const FieldManipulator*>& fieldManipulators)
:	Record(RecordManipulator(fieldManipulators))
{
	set_fresh_buffer(m_manipulator.size());
}

FreshRecord::FreshRecord(const RecordManipulator& manipulator)
:	Record(manipulator)
{
	set_fresh_buffer(manipulator.size());
}

//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################

FreshRecord::FreshRecord(const FreshRecord& rhs)
:	Record(rhs.m_manipulator)
{
	set_buffer(rhs.m_buffer);
}

FreshRecord& FreshRecord::operator=(const FreshRecord& rhs)
{
	m_manipulator = rhs.m_manipulator;
	set_buffer(rhs.m_buffer);
	return *this;
}

//#################### PRIVATE METHODS ####################

void FreshRecord::set_buffer(const std::vector<char>& buffer)
{
	m_buffer = buffer;
	m_location = &m_buffer[0];
}

void FreshRecord::set_fresh_buffer(unsigned int size)
{
	m_buffer = std::vector<char>(size);
	m_location = &m_buffer[0];
}

}
