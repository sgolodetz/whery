/**
 * whery: BTreeDataPage.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREEDATAPAGE
#define H_WHERY_BTREEDATAPAGE

#include <set>
#include <vector>

#include "whery/db/base/BackedTuple.h"
#include "whery/db/base/TupleComparator.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class RangeKey;
class ValueKey;

/**
\brief An instance of this class represents a (sorted) page of tuples in a B+-tree.

Pages of tuples are of a fixed size, and as such can hold a maximum number of tuples.
When they are full, additional pages must be allocated.
*/
class BTreeDataPage
{
	//#################### TYPEDEFS ####################
private:
	typedef std::multiset<BackedTuple,TupleComparator> TupleSet;

	//#################### PRIVATE VARIABLES ####################
private:
	/** The memory buffer used by the page to hold the tuple data. */
	std::vector<char> m_buffer;

	/** A free list of tuples that have been deleted - these can be reallocated by add_tuple(). */
	std::vector<char *> m_freeList;

	/** The manipulator used to interact with the tuples in the buffer. */
	TupleManipulator m_tupleManipulator;

	/** The set of tuples on the page (sorted lexicographically in ascending order). */
	TupleSet m_tuples;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a data page to contain tuples whose fields can be manipulated
	by the specified manipulators.

	\param fieldManipulators		A non-empty array of manipulators to be used to manipulate
									the fields of each tuple on the page.
	\param bufferSize				The size (in bytes) to use for the page's memory buffer.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	BTreeDataPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize);

	/**
	Constructs a data page to contain tuples that can be manipulated by the specified manipulator.

	\param bufferSize		The size (in bytes) to use for the page's memory buffer.
	\param tupleManipulator	The manipulator to be used to interact with tuples on the page.
	*/
	BTreeDataPage(unsigned int bufferSize, const TupleManipulator& tupleManipulator);

	//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################
private:
	/** Private and unimplemented for now - the compiler-generated defaults would be inadequate. */
	BTreeDataPage(const BTreeDataPage&);
	BTreeDataPage& operator=(const BTreeDataPage&);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Adds a tuple to the page, provided there is enough space to do so.

	\param tuple				The tuple to add.
	\throw std::out_of_range	If there is not enough space on the page to add a tuple.
	*/
	void add_tuple(const Tuple& tuple);

	/**
	Deletes the specified tuple from the page, if it is present.

	\param tuple	The tuple to delete.
	*/
	void delete_tuple(const BackedTuple& tuple);

	/**
	Gets the number of additional tuples that can fit on the page.

	\return	The number of additional tuples that can fit on the page.
	*/
	unsigned int empty_tuples() const;

	/**
	Gets the manipulators for the fields of the tuples on the page.

	\return	The manipulators for the fields of the tuples on the page.
	*/
	const std::vector<const FieldManipulator*>& field_manipulators() const;

	/**
	Gets the maximum number of tuples that can be stored on the page.

	\return	The maximum number of tuples that can be stored on the page.
	*/
	unsigned int max_tuples() const;

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
	Gets the set of tuples on the page.

	\return	The set of tuples on the page.
	*/
	const TupleSet& tuples() const;

	/**
	Performs a range-based lookup to find the tuples on the page that lie in the
	range specified by the key.

	\param key	The key specifying the range in which matching tuples should lie.
	\return		An array of BackedTuple objects that refer to the matching tuples on the page.
	*/
	std::vector<BackedTuple> tuples_by_range(const RangeKey& key) const;

	/**
	Performs a value-based lookup to find the tuples on the page that match
	the specified key.

	\param key	The key against which to match the tuples.
	\return		An array of BackedTuple objects that refer to the matching tuples on the page.
	*/
	std::vector<BackedTuple> tuples_by_value(const ValueKey& key) const;

	/**
	Gets the size (in bytes) of the page's buffer.

	\return	The size (in bytes) of the page's buffer.
	*/
	unsigned int size() const;
};

}

#endif
