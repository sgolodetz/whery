/**
 * whery: RangeKey.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/RangeKey.h"

namespace whery {

//#################### CONSTRUCTORS ####################

RangeKey::RangeKey(const std::vector<unsigned int>& fieldIndices)
:	m_fieldIndices(fieldIndices)
{}

//#################### PUBLIC METHODS ####################

unsigned int RangeKey::arity() const
{
	return m_fieldIndices.size();
}

const std::vector<unsigned int>& RangeKey::field_indices() const
{
	return m_fieldIndices;
}

const RangeEndpoint *RangeKey::high_endpoint() const
{
	return m_highEndpoint.get();
}

const RangeEndpoint *RangeKey::low_endpoint() const
{
	return m_lowEndpoint.get();
}

void RangeKey::set_endpoints(const RangeEndpoint& lowEndpoint, const RangeEndpoint& highEndpoint)
{
	set_low_endpoint(lowEndpoint);
	set_high_endpoint(highEndpoint);
}

void RangeKey::set_high_endpoint(const RangeEndpoint& endpoint)
{
	m_highEndpoint.reset(new RangeEndpoint(endpoint));
}

void RangeKey::set_low_endpoint(const RangeEndpoint& endpoint)
{
	m_lowEndpoint.reset(new RangeEndpoint(endpoint));
}

}
