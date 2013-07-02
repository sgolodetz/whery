/**
 * whery: UUIDFieldManipulator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_UUIDFIELDMANIPULATOR
#define H_WHERY_UUIDFIELDMANIPULATOR

#include "FieldManipulator.h"

namespace whery {

/**
\brief An instance of this class is used to manipulate Boost UUID fields at some location in memory.
*/
class UUIDFieldManipulator : public FieldManipulator
{
	//#################### SINGLETON IMPLEMENTATION ####################
public:
	/**
	Gets the singleton instance.

	\return	The singleton instance.
	*/
	static const UUIDFieldManipulator& instance();

private:
	/** Private to prevent uncontrolled instantiation. */
	UUIDFieldManipulator();

	/** Private and unimplemented to prevent copying. */
	UUIDFieldManipulator(const UUIDFieldManipulator&);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual unsigned int alignment_requirement() const;
	virtual int compare_to(const char *location, const FieldManipulator& otherManipulator, const char *otherLocation) const;
	virtual std::string get_string(const char *location) const;
	virtual boost::uuids::uuid get_uuid(const char *location) const;
	virtual void set_from(char *location, const FieldManipulator& sourceManipulator, const char *sourceLocation) const;
	virtual void set_uuid(char *location, const boost::uuids::uuid& value) const;
	virtual unsigned int size() const;
};

}

#endif
