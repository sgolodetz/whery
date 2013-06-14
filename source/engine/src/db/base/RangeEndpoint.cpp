/**
 * whery: RangeEndpoint.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/RangeEndpoint.h"

namespace whery {

//#################### CONSTRUCTORS ####################

RangeEndpoint::RangeEndpoint(const ValueKey& value, RangeEndpointKind kind)
:	m_kind(kind), m_value(value)
{}

//#################### PUBLIC METHODS ####################

RangeEndpointKind& RangeEndpoint::kind()
{
	return m_kind;
}

ValueKey& RangeEndpoint::value()
{
	return m_value;
}

}
