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
class ValueKey;

/**
\brief An instance of this class represents a sorted page of tuples.

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

	//#################### PRIVATE VARIABLES ####################
private:
	/** The memory buffer used by the page to hold the tuple data. */
	boost::shared_ptr<std::vector<char> > m_buffer;

	/** A free list of tuples that have been deleted - these can be reallocated by add_tuple(). */
	std::vector<char*> m_freeList;

	/** The manipulator used to interact with the tuples in the buffer. */
	TupleManipulator m_tupleManipulator;

	/** The set of tuples on the page (sorted lexicographically in ascending order). */
	TupleSet m_tuples;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a page to contain tuples whose fields can be manipulated by the specified manipulators.

	\param fieldManipulators		A non-empty array of manipulators to be used to manipulate
									the fields of each tuple on the page.
	\param bufferSize				The size (in bytes) to use for the page's memory buffer.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	SortedPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize);

	/**
	Constructs a page to contain tuples that can be manipulated by the specified manipulator.

	\param bufferSize		The size (in bytes) to use for the page's memory buffer.
	\param tupleManipulator	The manipulator to be used to interact with tuples on the page.
	*/
	SortedPage(unsigned int bufferSize, const TupleManipulator& tupleManipulator);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Adds a tuple to the page, provided there is enough space to do so.

	\param tuple				The tuple to add.
	\throw std::out_of_range	If there is not enough space on the page to add a tuple.
	*/
	void add_tuple(const Tuple& tuple);

	/**
	Returns an iterator pointing to the start of the set of tuples on the page.

	\return	An iterator pointing to the start of the set of tuples on the page.
	*/
	TupleSetCIter begin() const;

	/**
	Deletes the specified tuple from the page, if it is present.

	\param tuple	The tuple to delete.
	*/
	void delete_tuple(const BackedTuple& tuple);

	/**
	Gets the number of additional tuples that can fit on the page.

	\return	The number of additional tuples that can fit on the page.
	*/
	unsigned int empty_tuple_count() const;

	/**
	Returns an iterator pointing to the end of the set of tuples on the page.

	\return	An iterator pointing to the end of the set of tuples on the page.
	*/
	TupleSetCIter end() const;

	/**
	Returns the pair [lower_bound(key), upper_bound(key)].

	\return	The pair [lower_bound(key), upper_bound(key)].
	*/
	EqualRangeResult equal_range(const RangeKey& key) const;

	/**
	Returns the pair [lower_bound(key), upper_bound(key)].

	\return	The pair [lower_bound(key), upper_bound(key)].
	*/
	EqualRangeResult equal_range(const ValueKey& key) const;

	/**
	Gets the manipulators for the fields of the tuples on the page.

	\return	The manipulators for the fields of the tuples on the page.
	*/
	const std::vector<const FieldManipulator*>& field_manipulators() const;

	/**
	Returns an iterator pointing to the tuple at the lower end of the range
	specified by key.

	\param key	The search key.
	\return		An iterator pointing to the tuple at the lower end of the
				range specified by key.
	*/
	TupleSetCIter lower_bound(const RangeKey& key) const;

	/**
	Returns an iterator pointing to the first tuple on the page that is not
	ordered before the specified key (using prefix comparison).

	\param key	The search key.
	\return		An iterator pointing to the first tuple on the page that is
				not ordered before key, or end() if all tuples are ordered
				before key.
	*/
	TupleSetCIter lower_bound(const ValueKey& key) const;

	/**
	Gets the maximum number of tuples that can be stored on the page.

	\return	The maximum number of tuples that can be stored on the page.
	*/
	unsigned int max_tuple_count() const;

	/**
	Gets the percentage of the page's buffer that currently contains tuples.

	\return	The percentage of the page's buffer that currently contains tuples (in the range 0-100).
	*/
	double percentage_full() const;

	/**
	Gets the number of tuples that are currently stored on the page.

	\return	The number of tuples that are currently stored on the page.
	*/
	unsigned int tuple_count() const;

	/**
	Returns an iterator pointing one beyond the tuple at the higher end of the
	range specified by key.

	\param key	The search key.
	\return		An iterator pointing one beyond the tuple at the lower end of
				the range specified by key.
	*/
	TupleSetCIter upper_bound(const RangeKey& key) const;

	/**
	Returns an iterator pointing to the first tuple on the page that is ordered
	after the specified key (using prefix comparison).

	\param key	The search key.
	\return		An iterator pointing to the first tuple on the page that is ordered
				after key, or end() if no tuples are ordered after key.
	*/
	TupleSetCIter upper_bound(const ValueKey& key) const;

	/**
	Gets the size (in bytes) of the page's buffer.

	\return	The size (in bytes) of the page's buffer.
	*/
	unsigned int size() const;
};

}

#endif
