/**
 * whery: BackedTuple.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BACKEDTUPLE
#define H_WHERY_BACKEDTUPLE

#include "Tuple.h"
#include "TupleManipulator.h"

namespace whery {

/**
\brief An instance of this class represents a tuple that is backed by memory (e.g. a database record).
*/
class BackedTuple : public Tuple
{
	//#################### PROTECTED VARIABLES ####################
protected:
	/** The location of the tuple in memory. */
	char *m_location;

	/** The manipulator used to interact with the memory containing the tuple. */
	TupleManipulator m_manipulator;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a tuple that is backed by the memory at location and
	that can be interacted with using the specified manipulator.

	\param location		The location of the tuple in memory.
	\param manipulator	The manipulator used to interact with the memory containing the tuple.
	*/
	BackedTuple(char *location, const TupleManipulator& manipulator);

protected:
	/**
	Constructs a tuple that can be interacted with using the specified manipulator.
	The memory location for the tuple must be set separately. This constructor is
	intended for use by derived classes only.

	\manipulator	The manipulator used to interact with the memory containing the tuple.
	*/
	explicit BackedTuple(const TupleManipulator& manipulator);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual unsigned int arity() const;
	virtual Field field(unsigned int i) const;

	//#################### PUBLIC METHODS ####################
public:
	/**
	Copies the fields of the source tuple across to this tuple. The two tuples must
	have matching arities and the individual field types must be compatible.

	\param source					The tuple from which to copy fields.
	\throw std::bad_cast			If one or more of the individual field types are incompatible.
	\throw std::invalid_argument	If the tuples do not have matching arities.
	*/
	void copy_from(const Tuple& source) const;

	/**
	Gets the location of the tuple in memory.

	\return	The location of the tuple in memory.
	*/
	const char *location() const;

	/**
	Gets the overall size of the tuple (in bytes).

	\return	The overall size of the tuple (in bytes).
	*/
	unsigned int size() const;
};

}

#endif
