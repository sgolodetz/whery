/**
 * whery: FreshTuple.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_FRESHTUPLE
#define H_WHERY_FRESHTUPLE

#include <vector>

#include "BackedTuple.h"

namespace whery {

/**
\brief An instance of this class represents a freshly-created tuple that is backed by its own buffer.
*/
class FreshTuple : public BackedTuple
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The buffer backing the tuple. */
	std::vector<char> m_buffer;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a fresh tuple backed by a buffer of the correct size.

	\param fieldManipulators		A non-empty array of manipulators for the fields in the tuple.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	explicit FreshTuple(const std::vector<const FieldManipulator*>& fieldManipulators);

	/**
	Constructs a fresh tuple backed by a buffer of the correct size.

	\param manipulator	The manipulator used to interact with the buffer.
	*/
	explicit FreshTuple(const TupleManipulator& manipulator);

	//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################
public:
	/**
	Constructs a fresh tuple that is a copy of rhs.

	\param rhs	The tuple to copy.
	*/
	FreshTuple(const FreshTuple& rhs);

	/**
	Overwrites the contents of this tuple with the contents of rhs.

	\param rhs	The tuple from which to assign.
	\return		The current tuple.
	*/
	FreshTuple& operator=(const FreshTuple& rhs);

	//#################### PRIVATE METHODS ####################
private:
	/**
	Sets the buffer backing the tuple to the specified buffer.

	\param buffer	The buffer to copy.
	*/
	void set_buffer(const std::vector<char>& buffer);

	/**
	Sets the buffer backing the tuple to a fresh buffer of the specified size.

	\param size	The size of the fresh buffer.
	*/
	void set_fresh_buffer(unsigned int size);
};

}

#endif
