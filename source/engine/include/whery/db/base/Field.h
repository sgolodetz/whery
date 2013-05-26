/**
 * whery: Field.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_FIELD
#define H_WHERY_FIELD

#include <string>

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class FieldManipulator;

/**
\brief An instance of this class represents a field in a tuple.

Field objects themselves are immutable; it is only the values to which
they point that can be changed.
*/
class Field
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The location of the field in memory. */
	char *m_location;

	/** The manipulator used to interact with the memory containing the field. */
	const FieldManipulator& m_manipulator;

	/** Whether or not the field is read-only. If so, attempting to modify it will fail. */
	bool m_readOnly;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a field that is backed by the memory at location and
	that can be interacted with using the specified manipulator.

	\param location		The location of the field in memory.
	\param manipulator	The manipulator used to interact with the memory containing the field.
	\param readOnly		Whether or not the field should be read-only.
	*/
	Field(char *location, const FieldManipulator& manipulator, bool readOnly = false);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Compares this field with other. The value of the other field is first converted to the type
	of this field, if possible, after which the two are compared using std::less. If the type
	conversion fails, then the two fields cannot be compared, and an exception will be thrown.

	\param other			The other field being compared.
	\return					-1, if this field is ordered before other;
							1, if this field is ordered after other;
							0, otherwise.
	\throw std::bad_cast	If the type conversion fails.
	*/
	int compare_to(const Field& other) const;

	/**
	Gets the value of this field as a double, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\return					The value of this field as a double.
	\throw std::bad_cast	If the type conversion fails.
	*/
	double get_double() const;

	/**
	Gets the value of this field as an int, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\return					The value of this field as an int.
	\throw std::bad_cast	If the type conversion fails.
	*/
	int get_int() const;

	/**
	Gets the value of this field as a std::string, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\return					The value of this field as a std::string.
	\throw std::bad_cast	If the type conversion fails.
	*/
	std::string get_string() const;

	/**
	Sets this field to the specified value, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param value			The value to which to set the field.
	\throw std::bad_cast	If the type conversion fails.
	*/
	void set_double(double value) const;

	/**
	Sets this field to the value of another field, after first converting it to the right type.
	If the type conversion fails, an exception will be thrown.

	\param source			The source field.
	\throw std::bad_cast	If the type conversion fails.
	*/
	void set_from(const Field& source) const;

	/**
	Sets this field to the specified value, performing type conversion where necessary.
	If the type conversion fails, an exception will be thrown.

	\param value			The value to which to set the field.
	\throw std::bad_cast	If the type conversion fails.
	*/
	void set_int(int value) const;

	//#################### PRIVATE METHODS ####################
private:
	/**
	Checks that the field is not read-only.

	\throw std::logic_error	If the field is read-only.
	*/
	void ensure_writable() const;
};

}

#endif
