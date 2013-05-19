/**
 * whery: RecordProjection.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_RECORDPROJECTION
#define H_WHERY_RECORDPROJECTION

#include <vector>

#include "Record.h"

namespace whery {

/**
\brief An instance of this class represents the projection of a specified set of fields
from a record to form a derived field tuple.
*/
class RecordProjection : public FieldTuple
{
	//#################### PRIVATE VARIABLES ####################
private:
	/**
	The indices of the fields that are to be projected. Each index must be less than the
	arity of the record, and duplicates are allowed.
	*/
	std::vector<unsigned int> m_projectedFields;

	/** The record whose fields are to be projected. */
	Record m_record;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a record projection.

	\param record					The record whose fields are to be projected.
	\param projectedFields			The indices of the fields that are to be projected.
	\throw std::invalid_argument	If projectedFields is empty.
	*/
	RecordProjection(const Record& record, const std::vector<unsigned int>& projectedFields);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual unsigned int arity() const;
	virtual Field field(unsigned int i) const;
};

}

#endif
