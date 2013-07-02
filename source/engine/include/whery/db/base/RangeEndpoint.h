/**
 * whery: RangeEndpoint.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_RANGEENDPOINT
#define H_WHERY_RANGEENDPOINT

#include <boost/shared_ptr.hpp>

#include "ValueKey.h"

namespace whery {

/**
\brief The values of this enum represent possible kinds of range endpoint.
*/
enum RangeEndpointKind
{
	/**
	This end of the range includes the specified endpoint.
	For example, the low end of [9,84) is closed.
	*/
	CLOSED,

	/**
	This end of the range does not include the specified endpoint.
	For example, the high end of [9,84) is open.
	*/
	OPEN
};

/**
\brief	An instance of this class represents one end of a range.
*/
class RangeEndpoint
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The kind of endpoint (open or closed). */
	RangeEndpointKind m_kind;

	/** The value at the endpoint. */
	ValueKey m_value;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a range endpoint.

	\param value	The value at the endpoint.
	\param kind		The kind of endpoint (open or closed).
	*/
	RangeEndpoint(const ValueKey& value, RangeEndpointKind kind);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Gets the kind of endpoint.

	\return	The kind of endpoint (open or closed).
	*/
	RangeEndpointKind& kind();

	/**
	Gets the value at the endpoint.

	\return	The value at the endpoint.
	*/
	ValueKey& value();
};

typedef boost::shared_ptr<RangeEndpoint> RangeEndpoint_Ptr;

}

#endif
