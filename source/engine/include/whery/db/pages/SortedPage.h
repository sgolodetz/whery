/**
 * whery: SortedPage.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_SORTEDPAGE
#define H_WHERY_SORTEDPAGE

#include <set>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "whery/db/base/BackedTuple.h"
#include "whery/db/base/PrefixTupleComparator.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class RangeKey;
class Tuple;
class ValueKey;

/**
\brief An instance of a class deriving from this one represents a sorted page of tuples.

Pages of tuples are of a fixed size, and as such can hold a maximum number of tuples.
When they are full, additional pages must be allocated.
*/
class SortedPage
{
	//#################### TYPEDEFS ####################
public:
	typedef std::multiset<BackedTuple,PrefixTupleComparator> TupleSet;
	typedef TupleSet::const_iterator TupleSetCIter;
	typedef std::pair<TupleSetCIter,TupleSetCIter> EqualRangeResult;

	//#################### DESTRUCTOR ####################
public:
	/**
	Destroys the sorted page.
	*/
	virtual ~SortedPage() {}

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	/**
	Adds a tuple to the page, provided there is enough space to do so.

	\param tuple				The tuple to add.
	\throw std::out_of_range	If there is not enough space on the page to add a tuple.
	*/
	virtual void add_tuple(const Tuple& tuple) = 0;

	/**
	Returns an iterator pointing to the start of the set of tuples on the page.

	\return	An iterator pointing to the start of the set of tuples on the page.
	*/
	virtual TupleSetCIter begin() const = 0;

	/**
	Gets the size (in bytes) of the page's buffer.

	\return	The size (in bytes) of the page's buffer.
	*/
	virtual unsigned int buffer_size() const = 0;

	/**
	Deletes the specified tuple from the page, if it is present.

	\param tuple	The tuple to delete.
	*/
	virtual void delete_tuple(const BackedTuple& tuple) = 0;

	/**
	Gets the number of additional tuples that can fit on the page.

	\return	The number of additional tuples that can fit on the page.
	*/
	virtual unsigned int empty_tuple_count() const = 0;

	/**
	Returns an iterator pointing to the end of the set of tuples on the page.

	\return	An iterator pointing to the end of the set of tuples on the page.
	*/
	virtual TupleSetCIter end() const = 0;

	/**
	Returns the pair [lower_bound(key), upper_bound(key)].

	\return	The pair [lower_bound(key), upper_bound(key)].
	*/
	virtual EqualRangeResult equal_range(const RangeKey& key) const = 0;

	/**
	Returns the pair [lower_bound(key), upper_bound(key)].

	\return	The pair [lower_bound(key), upper_bound(key)].
	*/
	virtual EqualRangeResult equal_range(const ValueKey& key) const = 0;

	/**
	Gets the manipulators for the fields of the tuples on the page.

	\return	The manipulators for the fields of the tuples on the page.
	*/
	virtual const std::vector<const FieldManipulator*>& field_manipulators() const = 0;

	/**
	Returns an iterator pointing to the tuple at the lower end of the range
	specified by key.

	\param key	The search key.
	\return		An iterator pointing to the tuple at the lower end of the
				range specified by key.
	*/
	virtual TupleSetCIter lower_bound(const RangeKey& key) const = 0;

	/**
	Returns an iterator pointing to the first tuple on the page that is not
	ordered before the specified key (using prefix comparison).

	\param key	The search key.
	\return		An iterator pointing to the first tuple on the page that is
				not ordered before key, or end() if all tuples are ordered
				before key.
	*/
	virtual TupleSetCIter lower_bound(const ValueKey& key) const = 0;

	/**
	Gets the maximum number of tuples that can be stored on the page.

	\return	The maximum number of tuples that can be stored on the page.
	*/
	virtual unsigned int max_tuple_count() const = 0;

	/**
	Gets the percentage of the page's buffer that currently contains tuples.

	\return	The percentage of the page's buffer that currently contains tuples (in the range 0-100).
	*/
	virtual double percentage_full() const = 0;

	/**
	Gets the number of tuples that are currently stored on the page.

	\return	The number of tuples that are currently stored on the page.
	*/
	virtual unsigned int tuple_count() const = 0;

	/**
	Returns an iterator pointing one beyond the tuple at the higher end of the
	range specified by key.

	\param key	The search key.
	\return		An iterator pointing one beyond the tuple at the lower end of
				the range specified by key.
	*/
	virtual TupleSetCIter upper_bound(const RangeKey& key) const = 0;

	/**
	Returns an iterator pointing to the first tuple on the page that is ordered
	after the specified key (using prefix comparison).

	\param key	The search key.
	\return		An iterator pointing to the first tuple on the page that is ordered
				after key, or end() if no tuples are ordered after key.
	*/
	virtual TupleSetCIter upper_bound(const ValueKey& key) const = 0;
};

typedef boost::shared_ptr<SortedPage> SortedPage_Ptr;

}

#endif
