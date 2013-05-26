/**
 * test-db: TupleManipulatorTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/TupleManipulator.h"
using namespace whery;

#include "Constants.h"

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(TupleManipulatorTest)

BOOST_AUTO_TEST_CASE(field)
{
	std::vector<const FieldManipulator*> fieldManipulators = list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&DoubleFieldManipulator::instance());
	TupleManipulator tupleManipulator(fieldManipulators);

	// Check that the tuple size is at least the sum of the field sizes.
	BOOST_CHECK_GE(tupleManipulator.size(), fieldManipulators[0]->size() + fieldManipulators[1]->size());

	std::vector<char> buffer(tupleManipulator.size());
	char *loc = &buffer[0];
	tupleManipulator.field(loc, 0).set_int(23);
	tupleManipulator.field(loc, 1).set_double(9.0);

	// Check that we can retrieve the field values from the tuple correctly.
	BOOST_CHECK_EQUAL(tupleManipulator.field(loc, 0).get_int(), 23);
	BOOST_CHECK_CLOSE(tupleManipulator.field(loc, 1).get_double(), 9.0, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()
