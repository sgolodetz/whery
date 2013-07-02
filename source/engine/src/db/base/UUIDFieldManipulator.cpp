/**
 * whery: UUIDFieldManipulator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/UUIDFieldManipulator.h"

#include <boost/uuid/uuid_io.hpp>
using namespace boost::uuids;

namespace whery {

//#################### SINGLETON IMPLEMENTATION ####################

const UUIDFieldManipulator& UUIDFieldManipulator::instance()
{
	static UUIDFieldManipulator s_instance;
	return s_instance;
}

UUIDFieldManipulator::UUIDFieldManipulator()
{}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int UUIDFieldManipulator::alignment_requirement() const
{
	return uuid::static_size();
}

int UUIDFieldManipulator::compare_to(
	const char *location,
	const FieldManipulator& otherManipulator,
	const char *otherLocation) const
{
	return compare_with_less(get_uuid(location), otherManipulator.get_uuid(otherLocation));
}

std::string UUIDFieldManipulator::get_string(const char *location) const
{
	return to_string(get_uuid(location));
}

uuid UUIDFieldManipulator::get_uuid(const char *location) const
{
	return *reinterpret_cast<const uuid*>(location);
}

void UUIDFieldManipulator::set_from(
	char *location,
	const FieldManipulator& sourceManipulator,
	const char *sourceLocation) const
{
	set_uuid(location, sourceManipulator.get_uuid(sourceLocation));
}

void UUIDFieldManipulator::set_uuid(char *location, const uuid& value) const
{
	*reinterpret_cast<uuid*>(location) = value;
}

unsigned int UUIDFieldManipulator::size() const
{
	return uuid::static_size();
}

}
