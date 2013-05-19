/**
 * whery: FieldTuple.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_FIELDTUPLE
#define H_WHERY_FIELDTUPLE

#include "Field.h"

namespace whery {

/**
\brief An instance of a class deriving from this one represents a tuple of fields (e.g. a record or key).
*/
class FieldTuple
{
	//#################### DESTRUCTOR ####################
protected:
	/** Protected to prevent deletion via a FieldTuple pointer. */
	~FieldTuple() {}

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	/**
	Gets the arity (number of fields) of the tuple. This must be strictly greater than zero.

	\return	The arity of the tuple.
	*/
	virtual unsigned int arity() const = 0;

	/**
	Gets the i'th field in the tuple. Attempting to access an out-of-range field will
	cause undefined behaviour - no bounds-checking is done for performance reasons.

	\param i	The index of the field to retrieve from the tuple.
	\return		The i'th field in the tuple.
	*/
	virtual Field field(unsigned int i) const = 0;
};

}

#endif
