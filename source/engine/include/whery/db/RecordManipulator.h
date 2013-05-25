/**
 * whery: RecordManipulator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_RECORDMANIPULATOR
#define H_WHERY_RECORDMANIPULATOR

#include <vector>

#include "Field.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class FieldManipulator;

/**
\brief An instance of this class is used to manipulate database records
with a particular signature at some location in memory.

Record manipulators do not hold any state relating to the target records
on which they operate - the relevant member functions accept pointers to
the memory on which to work.
*/
class RecordManipulator
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The manipulators for the fields in a target record. */
	std::vector<const FieldManipulator*> m_fieldManipulators;

	/** The memory offsets of the fields (in bytes) from the start of a target record. */
	std::vector<unsigned int> m_fieldOffsets;

	/** The overall size (in bytes) of a target record. */
	unsigned int m_size;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a record manipulator.

	\param fieldManipulators		A non-empty array of manipulators for the fields in a target record.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	explicit RecordManipulator(const std::vector<const FieldManipulator*>& fieldManipulators);

	/**
	Constructs a record manipulator. This constructor is primarily useful when constructing record
	manipulators for keys, since it allows us to pass in the field manipulators for the underlying
	records and then rearrange them as necessary for the key.

	\param fieldManipulators		A non-empty array of manipulators for the fields in a target record.
	\param projectedFields			A non-empty array specified a way in which the manipulators should be rearranged.
	\throw std::invalid_argument	If fieldManipulators or projectedFields is empty.
	*/
	RecordManipulator(
		const std::vector<const FieldManipulator*>& fieldManipulators,
		const std::vector<unsigned int>& projectedFields
	);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Gets the arity (number of fields) of a target record.

	\return	The arity of a target record.
	*/
	unsigned int arity() const;

	/**
	Gets the i'th field of the record at recordLocation. Attempting to access
	an out-of-range field will cause undefined behaviour - no bounds-checking
	is done for performance reasons.

	\param recordLocation	The location of a target record.
	\param i				The index of the field to retrieve from the record.
	\return					The i'th field of the record.
	*/
	Field field(char *recordLocation, unsigned int i) const;

	/**
	Gets the manipulators for the fields in a target record.

	\return	The manipulators for the fields in a target record.
	*/
	const std::vector<const FieldManipulator*>& field_manipulators() const;

	/**
	Gets the overall size of a target record.

	\return	The overall size of a target record.
	*/
	unsigned int size() const;

	//#################### PRIVATE METHODS ####################
private:
	/**
	Initialises the record manipulator.

	\param fieldManipulators		A non-empty array of manipulators for the fields in a target record.
	\throw std::invalid_argument	If fieldManipulators is empty.
	*/
	void initialise(const std::vector<const FieldManipulator*>& fieldManipulators);
};

}

#endif
