/**
 * whery: ProjectedTuple.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/ProjectedTuple.h"

#include <cassert>
#include <stdexcept>

namespace whery {

//#################### CONSTRUCTORS ####################

ProjectedTuple::ProjectedTuple(const Tuple& source, const std::vector<unsigned int>& projectedFields)
:	m_source(source), m_projectedFields(projectedFields)
{
	if(projectedFields.empty())
	{
		throw std::invalid_argument("Projected tuples must contain at least one field.");
	}
}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int ProjectedTuple::arity() const
{
	return m_projectedFields.size();
}

Field ProjectedTuple::field(unsigned int i) const
{
	assert(i < m_projectedFields.size() && m_projectedFields[i] < m_source.arity());
	return m_source.field(m_projectedFields[i]);
}

}
