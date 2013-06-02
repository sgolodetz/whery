/**
 * whery: InMemorySortedPage.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_INMEMORYSORTEDPAGE
#define H_WHERY_INMEMORYSORTEDPAGE

#include "SortedPage.h"

namespace whery {

/**
\brief An instance of this class represents a sorted page of tuples that resides in memory.
*/
class InMemorySortedPage : public SortedPage
{
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
	InMemorySortedPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize);

	/**
	Constructs a page to contain tuples that can be manipulated by the specified manipulator.

	\param bufferSize		The size (in bytes) to use for the page's memory buffer.
	\param tupleManipulator	The manipulator to be used to interact with tuples on the page.
	*/
	InMemorySortedPage(unsigned int bufferSize, const TupleManipulator& tupleManipulator);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual void add_tuple(const Tuple& tuple);
	virtual TupleSetCIter begin() const;
	virtual unsigned int buffer_size() const;
	virtual void delete_tuple(const BackedTuple& tuple);
	virtual unsigned int empty_tuple_count() const;
	virtual TupleSetCIter end() const;
	virtual EqualRangeResult equal_range(const RangeKey& key) const;
	virtual EqualRangeResult equal_range(const ValueKey& key) const;
	virtual const std::vector<const FieldManipulator*>& field_manipulators() const;
	virtual TupleSetCIter lower_bound(const RangeKey& key) const;
	virtual TupleSetCIter lower_bound(const ValueKey& key) const;
	virtual unsigned int max_tuple_count() const;
	virtual double percentage_full() const;
	virtual unsigned int tuple_count() const;
	virtual TupleSetCIter upper_bound(const RangeKey& key) const;
	virtual TupleSetCIter upper_bound(const ValueKey& key) const;
};

typedef boost::shared_ptr<InMemorySortedPage> InMemorySortedPage_Ptr;

}

#endif
