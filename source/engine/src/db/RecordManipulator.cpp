/**
 * whery: RecordManipulator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/RecordManipulator.h"

#include <cassert>

#include "whery/db/FieldManipulator.h"
#include "whery/util/AlignmentTracker.h"

namespace whery {

//#################### CONSTRUCTORS ####################

RecordManipulator::RecordManipulator(const std::vector<const FieldManipulator*>& fieldManipulators)
:	m_fieldManipulators(fieldManipulators)
{
	if(fieldManipulators.empty())
	{
		throw std::invalid_argument("Records must contain at least one field.");
	}

	// Calculate the memory offsets of the fields (in bytes) from the start of a target record.
	AlignmentTracker alignmentTracker;
	m_fieldOffsets.reserve(m_fieldManipulators.size());
	for(size_t i = 0, size = m_fieldManipulators.size(); i < size; ++i)
	{
		alignmentTracker.advance_to_boundary(m_fieldManipulators[i]->alignment_requirement());
		m_fieldOffsets.push_back(alignmentTracker.offset());
		alignmentTracker.advance(m_fieldManipulators[i]->size());
	}

	// Advance to the next maximum-alignment boundary and store the size of the record.
	alignmentTracker.advance_to_boundary(alignmentTracker.max_alignment());
	m_size = alignmentTracker.offset();
}

//#################### PUBLIC METHODS ####################

unsigned int RecordManipulator::arity() const
{
	return m_fieldManipulators.size();
}

Field RecordManipulator::field(char *recordLocation, unsigned int i) const
{
	assert(i < m_fieldManipulators.size());
	return Field(recordLocation + m_fieldOffsets[i], *m_fieldManipulators[i]);
}

unsigned int RecordManipulator::size() const
{
	return m_size;
}

}
