/**
 * whery: FieldManipulator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_FIELDMANIPULATOR
#define H_WHERY_FIELDMANIPULATOR

#include <functional>
#include <string>

#include <boost/uuid/uuid.hpp>

namespace whery {

/**
\brief An instance of a class deriving from this one is used to manipulate
fields of a particular type at some location in memory (e.g. an int field
manipulator treats the pointed-to memory as an int).

Field manipulators hold no state - their member functions accept pointers
to the memory on which to work. (The intention is to separate the concerns
of field manipulation and field storage - the latter may vary, but the
manipulation of individual fields must be consistent.) As a result, we
only actually need a single instance of each type of field manipulator.
*/
class FieldManipulator
{
	//#################### DESTRUCTOR ####################
protected:
	/** Protected to prevent deletion via a FieldManipulator pointer. */
	~FieldManipulator();

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	/**
	Returns the size (in bytes) of the alignment boundary required for fields of the manipulated type.
	For example, a 32-bit int would need to be aligned on a 4-byte boundary.

	\return	The size (in bytes) of the alignment boundary required for fields of the manipulated type.
	*/
	virtual unsigned int alignment_requirement() const = 0;

	/**
	Compares the field at location (as manipulated by this manipulator) with the field at otherLocation
	(as manipulated by otherManipulator). The value of the field at otherLocation is first converted to
	the type of the field at location, if possible, after which the two are compared using std::less. If
	the type conversion fails, then the two fields cannot be compared, and an exception will be thrown.

	\param location			The memory location of the field on which this manipulator should operate.
	\param otherManipulator	The manipulator to use for the other field being compared.
	\param otherLocation	The memory location of the other field being compared.
	\return					-1, if the field at location is ordered before that at otherLocation;
							1, if the field at location is ordered after that at otherLocation;
							0, otherwise.
	\throw std::bad_cast	If the type conversion fails.
	*/
	virtual int compare_to(const char *location, const FieldManipulator& otherManipulator, const char *otherLocation) const = 0;

	/**
	Sets the field at location (as manipulated by this manipulator) to the value of the field at otherLocation
	(as manipulated by otherManipulator), after first converting it to the right type. If the type conversion
	fails, an exception will be thrown.

	\param location				The memory location of the field on which this manipulator should operate.
	\param sourceManipulator	The manipulator to use for the source field.
	\param sourceLocation		The memory location of the source field.
	\throw std::bad_cast		If the type conversion fails.
	*/
	virtual void set_from(char *location, const FieldManipulator& sourceManipulator, const char *sourceLocation) const = 0;

	/**
	Returns the size (in bytes) of the manipulated type. For example, a 32-bit int would have size 4.

	\return	The size (in bytes) of the manipulated type.
	*/
	virtual unsigned int size() const = 0;

	//#################### PUBLIC METHODS ####################

	/**
	Gets the value of the field at location as a double, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param location			The location of the field.
	\return					The value of the field as a double.
	\throw std::bad_cast	If the type conversion fails.
	*/
	virtual double get_double(const char *location) const;

	/**
	Gets the value of the field at location as an int, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param location			The location of the field.
	\return					The value of the field as an int.
	\throw std::bad_cast	If the type conversion fails.
	*/
	virtual int get_int(const char *location) const;

	/**
	Gets the value of the field at location as a std::string, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param location			The location of the field.
	\return					The value of the field as a std::string.
	\throw std::bad_cast	If the type conversion fails.
	*/
	virtual std::string get_string(const char *location) const;

	/**
	Gets the value of the field at location as a Boost UUID, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param location			The location of the field.
	\return					The value of the field as a Boost UUID.
	\throw std::bad_cast	If the type conversion fails.
	*/
	virtual boost::uuids::uuid get_uuid(const char *location) const;

	/**
	Sets the field at location to the specified value, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param location			The location of the field.
	\param value			The value to which to set the field.
	\throw std::bad_cast	If the type conversion fails.
	*/
	virtual void set_double(char *location, double value) const;

	/**
	Sets the field at location to the specified value, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param location			The location of the field.
	\param value			The value to which to set the field.
	\throw std::bad_cast	If the type conversion fails.
	*/
	virtual void set_int(char *location, int value) const;

	/**
	Sets the field at location to the specified value, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param location			The location of the field.
	\param value			The value to which to set the field.
	\throw std::bad_cast	If the type conversion fails.
	*/
	virtual void set_uuid(char *location, const boost::uuids::uuid& value) const;

	//#################### PROTECTED METHODS ####################
protected:
	/**
	Compares two values of type T using std::less<T>.

	\param lhs	The left-hand operand of the comparison.
	\param rhs	The right-hand operand of the comparison.
	\return		-1, if lhs < rhs;
				1, if rhs < lhs;
				0, otherwise.
	*/
	template <typename T>
	static int compare_with_less(const T& lhs, const T& rhs)
	{
		std::less<T> comp;
		if(comp(lhs, rhs)) return -1;
		else if(comp(rhs, lhs)) return 1;
		else return 0;
	}
};

}

#endif
