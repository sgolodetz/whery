/**
 * whery: DataPage.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_DATAPAGE
#define H_WHERY_DATAPAGE

#include <map>
#include <vector>

#include "BackedTuple.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class RangeKey;
class ValueKey;

/**
\brief An instance of this class represents a page of tuples for a database relation.

Pages of tuples are of a fixed size, and as such can hold a maximum number of tuples.
When they are full, additional pages must be allocated.
*/
class DataPage
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The memory buffer used by the page to hold the tuple data. */
	std::vector<char> m_buffer;

	/** A free list of tuples that have been deleted - these can be reallocated by add_tuple(). */
	std::vector<BackedTuple> m_freeList;

	/** The manipulator used to interact with the tuples in the buffer. */
	TupleManipulator m_tupleManipulator;

	/** A map from tuple locations to the corresponding tuples. */
	std::map<const char*,BackedTuple> m_tuples;

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
	DataPage(const std::vector<const FieldManipulator*>& fieldManipulators, unsigned int bufferSize);

	//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################
private:
	/**
	Private and unimplemented for now - the compiler-generated defaults
	would be inadequate, because the copied tuples map would refer to
	the buffer on the original data page.
	*/
	DataPage(const DataPage&);
	DataPage& operator=(const DataPage&);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Adds a tuple to the page, provided there is enough space to do so.

	\return						A BackedTuple object referring to the newly-added tuple, if the add succeeds.
	\throw std::out_of_range	If there is not enough space on the page to add a tuple.
	*/
	BackedTuple add_tuple();

	/**
	Deletes the specified tuple from the page, if it is present. The BackedTuple object
	is invalid after the deletion and should no longer be used by the calling code.

	\param tuple	A BackedTuple object referring to the tuple to delete.
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
	Gets an array of BackedTuple objects that refer to the tuples on the page.

	\return	An array of BackedTuple objects that refer to the tuples on the page.
	*/
	std::vector<BackedTuple> tuples() const;

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
