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

FreshRecord::FreshRecord(const RecordManipulator& manipulator)
:	Record(manipulator), m_buffer(manipulator.size())
{
	m_location = &m_buffer[0];
}

}
