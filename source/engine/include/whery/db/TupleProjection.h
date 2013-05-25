/**
 * whery: TupleProjection.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_TUPLEPROJECTION
#define H_WHERY_TUPLEPROJECTION

#include <vector>

#include "Tuple.h"

namespace whery {

/**
\brief An instance of this class represents the projection of a specified set of fields
from a tuple to form a derived tuple.
*/
class TupleProjection : public Tuple
{
	//#################### PRIVATE VARIABLES ####################
private:
	/**
	The indices of the fields that are to be projected. Each index must be less than the
	arity of the source tuple, and duplicates are allowed.
	*/
	std::vector<unsigned int> m_projectedFields;

	/** The tuple whose fields are to be projected. */
	const Tuple& m_source;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a tuple projection.

	\param source					The tuple whose fields are to be projected.
	\param projectedFields			The indices of the fields that are to be projected.
	\throw std::invalid_argument	If projectedFields is empty.
	*/
	TupleProjection(const Tuple& source, const std::vector<unsigned int>& projectedFields);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual unsigned int arity() const;
	virtual Field field(unsigned int i) const;
};

}

#endif
