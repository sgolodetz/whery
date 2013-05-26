/**
 * whery: IntFieldManipulator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_INTFIELDMANIPULATOR
#define H_WHERY_INTFIELDMANIPULATOR

#include "FieldManipulator.h"

namespace whery {

/**
\brief An instance of this class is used to manipulate int fields at some location in memory.
*/
class IntFieldManipulator : public FieldManipulator
{
	//#################### SINGLETON IMPLEMENTATION ####################
public:
	/**
	Gets the singleton instance.

	\return	The singleton instance.
	*/
	static const IntFieldManipulator& instance();

private:
	/** Private to prevent uncontrolled instantiation. */
	IntFieldManipulator();

	/** Private and unimplemented to prevent copying. */
	IntFieldManipulator(const IntFieldManipulator&);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual unsigned int alignment_requirement() const;
	virtual int compare_to(const char *location, const FieldManipulator& otherManipulator, const char *otherLocation) const;
	virtual double get_double(const char *location) const;
	virtual int get_int(const char *location) const;
	virtual std::string get_string(const char *location) const;
	virtual void set_double(char *location, double value) const;
	virtual void set_from(char *location, const FieldManipulator& sourceManipulator, const char *sourceLocation) const;
	virtual void set_int(char *location, int value) const;
	virtual unsigned int size() const;
};

}

#endif
