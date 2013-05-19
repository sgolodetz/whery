/**
 * test-db: RecordManipulatorTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include "whery/db/DoubleFieldManipulator.h"
#include "whery/db/IntFieldManipulator.h"
#include "whery/db/RecordManipulator.h"
using namespace whery;

#include "Constants.h"

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(RecordManipulatorTest)

BOOST_AUTO_TEST_CASE(field)
{
	std::vector<const FieldManipulator*> fms;
	fms.push_back(&IntFieldManipulator::instance());
	fms.push_back(&DoubleFieldManipulator::instance());
	RecordManipulator rm(fms);

	// Check that the record size is at least the sum of the field sizes.
	BOOST_CHECK_GE(rm.size(), fms[0]->size() + fms[1]->size());

	std::vector<char> buffer(rm.size());
	char *loc = &buffer[0];
	rm.field(loc, 0).set_int(23);
	rm.field(loc, 1).set_double(9.0);

	// Check that we can retrieve the field values from the record correctly.
	BOOST_CHECK_EQUAL(rm.field(loc, 0).get_int(), 23);
	BOOST_CHECK_CLOSE(rm.field(loc, 1).get_double(), 9.0, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()
