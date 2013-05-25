/**
 * whery: TupleProjection.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/TupleProjection.h"

#include <cassert>

namespace whery {

//#################### CONSTRUCTORS ####################

TupleProjection::TupleProjection(const Tuple& source, const std::vector<unsigned int>& projectedFields)
:	m_source(source), m_projectedFields(projectedFields)
{
	if(projectedFields.empty())
	{
		throw std::invalid_argument("Tuple projections must contain at least one field.");
	}
}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int TupleProjection::arity() const
{
	return m_projectedFields.size();
}

Field TupleProjection::field(unsigned int i) const
{
	assert(i < m_projectedFields.size() && m_projectedFields[i] < m_source.arity());
	return m_source.field(m_projectedFields[i]);
}

}
