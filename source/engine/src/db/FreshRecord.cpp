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
	m_buffer.resize(m_manipulator.size());
	m_location = &m_buffer[0];
}

FreshRecord::FreshRecord(
	const std::vector<const FieldManipulator*>& fieldManipulators,
	const std::vector<unsigned int>& projectedFields)
:	Record(RecordManipulator(fieldManipulators, projectedFields))
{
	m_buffer.resize(m_manipulator.size());
	m_location = &m_buffer[0];
}

FreshRecord::FreshRecord(const RecordManipulator& manipulator)
:	Record(manipulator), m_buffer(manipulator.size())
{
	m_location = &m_buffer[0];
}

//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################

FreshRecord::FreshRecord(const FreshRecord& rhs)
:	Record(rhs.m_manipulator), m_buffer(rhs.m_buffer)
{
	m_location = &m_buffer[0];
}

FreshRecord& FreshRecord::operator=(const FreshRecord& rhs)
{
	m_buffer = rhs.m_buffer;
	m_location = &m_buffer[0];
	m_manipulator = rhs.m_manipulator;
	return *this;
}

}
