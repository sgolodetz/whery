/**
 * whery: PrefixTupleComparator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_PREFIXTUPLECOMPARATOR
#define H_WHERY_PREFIXTUPLECOMPARATOR

#include <functional>

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class Tuple;

/**
\brief An instance of this class is used to perform prefix comparisons on tuples (e.g. database records).

Note that the types of the tuples' corresponding fields must be compatible for the comparison to succeed.
As an example of such a comparison: (7,8) = (7,8,51) < (17) = (17,10) < (23,9) = (23,9,84).
*/
class PrefixTupleComparator : std::binary_function<Tuple,Tuple,bool>
{
	//#################### PUBLIC OPERATORS ####################
public:
	/**
	Orders two tuples using prefix comparison.

	\param lhs				The left-hand tuple.
	\param rhs				The right-hand tuple.
	\return					true, if lhs is ordered before rhs, or false otherwise.
	\throw std::bad_cast	If the types of the tuples' corresponding fields are not compatible.
	*/
	bool operator()(const Tuple& lhs, const Tuple& rhs) const;

	//#################### PUBLIC METHODS ####################
public:
	/**
	Compares two tuples using prefix comparison.

	\param lhs				The left-hand tuple.
	\param rhs				The right-hand tuple.
	\return					-1, if lhs is ordered before rhs;
							1, if lhs is ordered after rhs;
							0, otherwise.
	\throw std::bad_cast	If the types of the tuples' corresponding fields are not compatible.
	*/
	int compare(const Tuple& lhs, const Tuple& rhs) const;
};

}

#endif
