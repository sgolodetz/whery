/**
 * whery: Tuple.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_TUPLE
#define H_WHERY_TUPLE

#include "Field.h"

namespace whery {

/**
\brief An instance of a class deriving from this one represents a tuple of fields (e.g. a database record or key).
*/
class Tuple
{
	//#################### DESTRUCTOR ####################
protected:
	/** Protected to prevent deletion via a Tuple pointer. */
	~Tuple() {}

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
