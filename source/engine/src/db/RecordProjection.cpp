/**
 * whery: RecordProjection.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/RecordProjection.h"

#include <cassert>

namespace whery {

//#################### CONSTRUCTORS ####################

RecordProjection::RecordProjection(const Record& record, const std::vector<unsigned int>& projectedFields)
:	m_record(record), m_projectedFields(projectedFields)
{
	if(projectedFields.empty())
	{
		throw std::invalid_argument("Record projections must contain at least one field.");
	}
}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int RecordProjection::arity() const
{
	return m_projectedFields.size();
}

Field RecordProjection::field(unsigned int i) const
{
	assert(i < m_projectedFields.size() && m_projectedFields[i] < m_record.arity());
	return m_record.field(m_projectedFields[i]);
}

}
