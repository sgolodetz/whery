/**
 * whery: FieldTupleComparator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_FIELDTUPLECOMPARATOR
#define H_WHERY_FIELDTUPLECOMPARATOR

#include <functional>
#include <utility>
#include <vector>

#include "whery/util/SortDirection.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class FieldTuple;

/**
\brief An instance of this class is used to perform partial lexicographic comparisons on field tuples (e.g. records).

This can be used as the comparator for a multi-column sort, e.g. first sort by column A and then by column B.
The arities of the tuples being compared must match in order for comparisons to be valid.
*/
class FieldTupleComparator : std::binary_function<FieldTuple,FieldTuple,bool>
{
	//#################### PRIVATE VARIABLES ####################
private:
	/**
	The non-empty list of field indices on which to sort (in order),
	together with their sort directions.
	*/
	std::vector<std::pair<unsigned int,SortDirection> > m_fieldIndices;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a field tuple comparator.

	\param fieldIndices				The non-empty list of field indices on which to sort (in order),
									together with their sort directions.
	\throw std::invalid_argument	If fieldIndices is empty.
	*/
	explicit FieldTupleComparator(const std::vector<std::pair<unsigned int,SortDirection> >& fieldIndices);

	//#################### PUBLIC OPERATORS ####################
public:
	/**
	Orders two field tuples based on the fields specified when constructing the comparator.

	\param lhs						The left-hand tuple.
	\param rhs						The right-hand tuple.
	\return							true, if lhs is ordered before rhs, or false otherwise.
	\throw std::invalid_argument	If the arities of the tuples being compared do not match.
	*/
	bool operator()(const FieldTuple& lhs, const FieldTuple& rhs) const;
};

}

#endif
