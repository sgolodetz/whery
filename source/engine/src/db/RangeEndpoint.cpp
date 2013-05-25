/**
 * whery: RangeEndpoint.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/RangeEndpoint.h"

namespace whery {

//#################### CONSTRUCTORS ####################

RangeEndpoint::RangeEndpoint(const FreshTuple& value, RangeEndpointKind kind)
:	m_kind(kind), m_value(value)
{}

//#################### PUBLIC METHODS ####################

RangeEndpointKind RangeEndpoint::kind() const
{
	return m_kind;
}

const FreshTuple& RangeEndpoint::value() const
{
	return m_value;
}

}
