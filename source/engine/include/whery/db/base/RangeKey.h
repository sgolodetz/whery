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

	/** A non-empty array of manipulators for the fields in the tuples being keyed. */
	std::vector<const FieldManipulator*> m_fieldManipulators;

	/** The high end of the range (if any). */
	RangeEndpoint_Ptr m_highEndpoint;

	/** The low end of the range (if any). */
	RangeEndpoint_Ptr m_lowEndpoint;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a range key.

	\param fieldManipulators		A non-empty array of manipulators for the fields in the tuples being keyed.
	\param fieldIndices				A non-empty array specifying the indices of the fields to be used for the key.
	\throw std::invalid_argument	If fieldManipulators or fieldIndices is empty.
	*/
	explicit RangeKey(
		const std::vector<const FieldManipulator*>& fieldManipulators,
		const std::vector<unsigned int>& fieldIndices
	);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Gets the arity (number of fields) of the key. This must be strictly greater than zero.

	\return	The arity of the key.
	*/
	unsigned int arity() const;

	/**
	Clears the high endpoint of the range.
	*/
	void clear_high_endpoint();

	/**
	Clears the low endpoint of the range.
	*/
	void clear_low_endpoint();

	/**
	Gets the indices of the fields to be used for the key.

	\return	The indices of the fields to be used for the key.
	*/
	const std::vector<unsigned int>& field_indices() const;

	/**
	Returns whether or not the range has a high endpoint.

	\return	true, if the range has a high endpoint, or false otherwise.
	*/
	bool has_high_endpoint() const;

	/**
	Returns whether or not the range has a low endpoint.

	\return	true, if the range has a low endpoint, or false otherwise.
	*/
	bool has_low_endpoint() const;

	/**
	Gets the kind (open or closed) of the range's high endpoint (if any).

	\return	The kind (open or closed) of the range's high endpoint (if any).
	*/
	RangeEndpointKind& high_kind();

	/**
	Gets the kind (open or closed) of the range's high endpoint (if any).

	\return	The kind (open or closed) of the range's high endpoint (if any).
	*/
	RangeEndpointKind high_kind() const;

	/**
	Gets the value of the range's high endpoint (if any).

	\return	The value of the range's high endpoint (if any).
	*/
	FreshTuple& high_value();

	/**
	Gets the value of the range's high endpoint (if any).

	\return	The value of the range's high endpoint (if any).
	*/
	const FreshTuple& high_value() const;

	/**
	Gets the kind (open or closed) of the range's low endpoint (if any).

	\return	The kind (open or closed) of the range's low endpoint (if any).
	*/
	RangeEndpointKind& low_kind();

	/**
	Gets the kind (open or closed) of the range's low endpoint (if any).

	\return	The kind (open or closed) of the range's low endpoint (if any).
	*/
	RangeEndpointKind low_kind() const;

	/**
	Gets the value of the range's low endpoint (if any).

	\return	The value of the range's low endpoint (if any).
	*/
	FreshTuple& low_value();

	/**
	Gets the value of the range's low endpoint (if any).

	\return	The value of the range's low endpoint (if any).
	*/
	const FreshTuple& low_value() const;

	//#################### PRIVATE METHODS ####################
private:
	/**
	Ensures that the specified endpoint is non-NULL.

	\param endpoint	The endpoint that we want to ensure is non-NULL.
	*/
	void ensure_endpoint(RangeEndpoint_Ptr& endpoint);
};

}

#endif
