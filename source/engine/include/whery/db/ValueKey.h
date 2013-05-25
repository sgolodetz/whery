/**
 * whery: ValueKey.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_VALUEKEY
#define H_WHERY_VALUEKEY

#include "FreshTuple.h"

namespace whery {

/**
\brief An instance of this class represents a key used for value lookups on database relations.
*/
class ValueKey : public FreshTuple
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** A non-empty array specifying the indices of the fields to be used for the key. */
	std::vector<unsigned int> m_fieldIndices;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a value key.

	\param fieldManipulators		A non-empty array of manipulators for the fields in the tuples being keyed.
	\param fieldIndices				A non-empty array specifying the indices of the fields to be used for the key.
	\throw std::invalid_argument	If fieldManipulators or fieldIndices is empty.
	*/
	ValueKey(
		const std::vector<const FieldManipulator*>& fieldManipulators,
		const std::vector<unsigned int>& fieldIndices
	);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Gets the indices of the fields to be used for the key.

	\return	The indices of the fields to be used for the key.
	*/
	const std::vector<unsigned int>& field_indices() const;
};

}

#endif
