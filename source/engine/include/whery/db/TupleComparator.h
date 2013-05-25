/**
 * whery: TupleComparator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_TUPLECOMPARATOR
#define H_WHERY_TUPLECOMPARATOR

#include <functional>
#include <utility>
#include <vector>

#include "SortDirection.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class Tuple;

/**
\brief An instance of this class is used to perform partial lexicographic comparisons on tuples (e.g. database records).

This can be used as the comparator for a multi-column sort, e.g. first sort by column A and then by column B.
The arities of the tuples being compared must match in order for comparisons to be valid.
*/
class TupleComparator : std::binary_function<Tuple,Tuple,bool>
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
	Constructs a tuple comparator.

	\param fieldIndices				The non-empty list of field indices on which to sort (in order),
									together with their sort directions.
	\throw std::invalid_argument	If fieldIndices is empty.
	*/
	explicit TupleComparator(const std::vector<std::pair<unsigned int,SortDirection> >& fieldIndices);

	//#################### PUBLIC OPERATORS ####################
public:
	/**
	Orders two tuples based on the fields specified when constructing the comparator.

	\param lhs						The left-hand tuple.
	\param rhs						The right-hand tuple.
	\return							true, if lhs is ordered before rhs, or false otherwise.
	\throw std::invalid_argument	If the arities of the tuples being compared do not match.
	*/
	bool operator()(const Tuple& lhs, const Tuple& rhs) const;

	//#################### PUBLIC METHODS ####################
public:
	/**
	Compares two tuples based on the fields specified when constructing the comparator.

	\param lhs						The left-hand tuple.
	\param rhs						The right-hand tuple.
	\return							-1, if lhs is ordered before rhs;
									1, if lhs is ordered after rhs;
									0, otherwise.
	\throw std::invalid_argument	If the arities of the tuples being compared do not match.
	*/
	int compare(const Tuple& lhs, const Tuple& rhs) const;
};

}

#endif
