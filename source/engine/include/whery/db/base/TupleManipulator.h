/**
 * whery: TupleManipulator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_TUPLEMANIPULATOR
#define H_WHERY_TUPLEMANIPULATOR

#include <vector>

#include <boost/assign/list_of.hpp>

#include "Field.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class FieldManipulator;

/**
\brief An instance of this class is used to manipulate tuples
with a particular signature at some location in memory.

Tuple manipulators do not hold any state relating to the target tuples
on which they operate - the relevant member functions accept pointers
to the memory on which to work.
*/
class TupleManipulator
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The manipulators for the fields in a target tuple. */
	std::vector<const FieldManipulator*> m_fieldManipulators;

	/** The memory offsets of the fields (in bytes) from the start of a target tuple. */
	std::vector<unsigned int> m_fieldOffsets;

	/** The overall size (in bytes) of a target tuple. */
	unsigned int m_size;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a tuple manipulator.

	\param fieldManipulators		A non-empty array of manipulators for the fields in a target tuple.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	explicit TupleManipulator(const std::vector<const FieldManipulator*>& fieldManipulators);

	/**
	Constructs a tuple manipulator.

	\param fieldManipulators		A non-empty list of manipulators for the fields in a target tuple.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	explicit TupleManipulator(const boost::assign_detail::generic_list<const FieldManipulator*>& fieldManipulators);

	/**
	Constructs a tuple manipulator. This constructor is used when constructing tuple
	manipulators for keys, since it allows us to pass in the field manipulators for
	the underlying tuples and then rearrange them as necessary for the key.

	\param fieldManipulators		A non-empty array of manipulators for the fields in an underlying tuple.
	\param fieldIndices				A non-empty array specifying the indices of the fields to be used for the key.
	\throw std::invalid_argument	If fieldManipulators or fieldIndices is empty.
	*/
	TupleManipulator(
		const std::vector<const FieldManipulator*>& fieldManipulators,
		const std::vector<unsigned int>& fieldIndices
	);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Gets the arity (number of fields) of a target tuple.

	\return	The arity of a target tuple.
	*/
	unsigned int arity() const;

	/**
	Gets the i'th field of the tuple at tupleLocation. Attempting to access an
	out-of-range field will cause undefined behaviour - no bounds-checking is
	done for performance reasons.

	\param tupleLocation	The location of a target tuple.
	\param i				The index of the field to retrieve from the tuple.
	\param readOnly			Whether or not the field should be read-only.
	\return					The i'th field of the tuple.
	*/
	Field field(char *tupleLocation, unsigned int i, bool readOnly = false) const;

	/**
	Gets the manipulators for the fields in a target tuple.

	\return	The manipulators for the fields in a target tuple.
	*/
	const std::vector<const FieldManipulator*>& field_manipulators() const;

	/**
	Gets the overall size of a target tuple.

	\return	The overall size of a target tuple.
	*/
	unsigned int size() const;

	//#################### PRIVATE METHODS ####################
private:
	/**
	Initialises the tuple manipulator.

	\param fieldManipulators		A non-empty array of manipulators for the fields in a target tuple.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	void initialise(const std::vector<const FieldManipulator*>& fieldManipulators);
};

}

#endif
