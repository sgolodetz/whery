/**
 * whery: RangeKey.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_RANGEKEY
#define H_WHERY_RANGEKEY

#include "RangeEndpoint.h"

namespace whery {

/**
\brief An instance of this class represents a key used for range lookups on database relations.
*/
class RangeKey
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** A non-empty array specifying the indices of the fields to be used for the key. */
	std::vector<unsigned int> m_fieldIndices;

	/** The high end of the range (if any). */
	RangeEndpoint_CPtr m_highEndpoint;

	/** The low end of the range (if any). */
	RangeEndpoint_CPtr m_lowEndpoint;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a range key.

	\param fieldIndices				A non-empty array specifying the indices of the fields to be used for the key.
	\throw std::invalid_argument	If fieldIndices is empty.
	*/
	explicit RangeKey(const std::vector<unsigned int>& fieldIndices);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Gets the indices of the fields to be used for the key.

	\return	The indices of the fields to be used for the key.
	*/
	const std::vector<unsigned int>& field_indices() const;

	/**
	Gets the high endpoint of the range (if any).

	\return	The high endpoint of the range, if it exists, or NULL otherwise.
	*/
	const RangeEndpoint *high_endpoint() const;

	/**
	Gets the low endpoint of the range (if any).

	\return	The low endpoint of the range, if it exists, or NULL otherwise.
	*/
	const RangeEndpoint *low_endpoint() const;

	/**
	Sets the low and high endpoints of the range.

	\param lowEndpoint	The low endpoint of the range.
	\param highEndpoint	The high endpoint of the range.
	*/
	void set_endpoints(const RangeEndpoint& lowEndpoint, const RangeEndpoint& highEndpoint);

	/**
	Sets the high endpoint of the range.

	\param endpoint	The high endpoint of the range.
	*/
	void set_high_endpoint(const RangeEndpoint& endpoint);

	/**
	Sets the low endpoint of the range.

	\param endpoint	The low endpoint of the range.
	*/
	void set_low_endpoint(const RangeEndpoint& endpoint);
};

}

#endif
