/**
 * whery: RangeKey.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/RangeKey.h"

#include <cassert>

#include "whery/db/base/PrefixTupleComparator.h"

namespace whery {

//#################### CONSTRUCTORS ####################

RangeKey::RangeKey(
	const std::vector<const FieldManipulator*>& fieldManipulators,
	const std::vector<unsigned int>& fieldIndices)
:	m_fieldIndices(fieldIndices), m_fieldManipulators(fieldManipulators)
{}

//#################### PUBLIC METHODS ####################

unsigned int RangeKey::arity() const
{
	return m_fieldIndices.size();
}

void RangeKey::clear_high_endpoint()
{
	m_highEndpoint.reset();
}

void RangeKey::clear_low_endpoint()
{
	m_lowEndpoint.reset();
}

const std::vector<unsigned int>& RangeKey::field_indices() const
{
	return m_fieldIndices;
}

bool RangeKey::has_high_endpoint() const
{
	return m_highEndpoint.get() != NULL;
}

bool RangeKey::has_low_endpoint() const
{
	return m_lowEndpoint.get() != NULL;
}

RangeEndpointKind& RangeKey::high_kind()
{
	ensure_endpoint(m_highEndpoint);
	return m_highEndpoint->kind();
}

RangeEndpointKind RangeKey::high_kind() const
{
	assert(has_high_endpoint());
	return m_highEndpoint->kind();
}

ValueKey& RangeKey::high_value()
{
	ensure_endpoint(m_highEndpoint);
	return m_highEndpoint->value();
}

const ValueKey& RangeKey::high_value() const
{
	assert(has_high_endpoint());
	return m_highEndpoint->value();
}

bool RangeKey::is_open_singleton() const
{
	return
		has_low_endpoint() &&
		has_high_endpoint() &&
		low_kind() == OPEN &&
		high_kind() == OPEN &&
		PrefixTupleComparator().compare(low_value(), high_value()) == 0;
}

bool RangeKey::is_valid() const
{
	return !(
		has_low_endpoint() &&
		has_high_endpoint() &&
		PrefixTupleComparator().compare(low_value(), high_value()) == 1
	);
}

RangeEndpointKind& RangeKey::low_kind()
{
	ensure_endpoint(m_lowEndpoint);
	return m_lowEndpoint->kind();
}

RangeEndpointKind RangeKey::low_kind() const
{
	assert(has_low_endpoint());
	return m_lowEndpoint->kind();
}

ValueKey& RangeKey::low_value()
{
	ensure_endpoint(m_lowEndpoint);
	return m_lowEndpoint->value();
}

const ValueKey& RangeKey::low_value() const
{
	assert(has_low_endpoint());
	return m_lowEndpoint->value();
}

//#################### PRIVATE METHODS ####################

void RangeKey::ensure_endpoint(RangeEndpoint_Ptr& endpoint)
{
	if(endpoint.get() == NULL)
	{
		ValueKey value(m_fieldManipulators, m_fieldIndices);
		endpoint.reset(new RangeEndpoint(value, CLOSED));
	}
}

}
