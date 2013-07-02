/**
 * whery: UuidFieldManipulator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/UuidFieldManipulator.h"

#include <boost/uuid/uuid_io.hpp>
using namespace boost::uuids;

namespace whery {

//#################### SINGLETON IMPLEMENTATION ####################

const UuidFieldManipulator& UuidFieldManipulator::instance()
{
	static UuidFieldManipulator s_instance;
	return s_instance;
}

UuidFieldManipulator::UuidFieldManipulator()
{}

//#################### PUBLIC INHERITED METHODS ####################

unsigned int UuidFieldManipulator::alignment_requirement() const
{
	return uuid::static_size();
}

int UuidFieldManipulator::compare_to(
	const char *location,
	const FieldManipulator& otherManipulator,
	const char *otherLocation) const
{
	return compare_with_less(get_uuid(location), otherManipulator.get_uuid(otherLocation));
}

std::string UuidFieldManipulator::get_string(const char *location) const
{
	return to_string(get_uuid(location));
}

boost::uuids::uuid UuidFieldManipulator::get_uuid(const char *location) const
{
	return *reinterpret_cast<const uuid*>(location);
}

void UuidFieldManipulator::set_from(
	char *location,
	const FieldManipulator& sourceManipulator,
	const char *sourceLocation) const
{
	set_uuid(location, sourceManipulator.get_uuid(sourceLocation));
}

void UuidFieldManipulator::set_uuid(char *location, const uuid& value) const
{
	*reinterpret_cast<uuid*>(location) = value;
}

unsigned int UuidFieldManipulator::size() const
{
	return uuid::static_size();
}

}
