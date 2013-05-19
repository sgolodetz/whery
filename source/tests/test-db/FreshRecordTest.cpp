/**
 * test-db: FreshRecordTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include "whery/db/DoubleFieldManipulator.h"
#include "whery/db/FreshRecord.h"
#include "whery/db/IntFieldManipulator.h"
using namespace whery;

#include "Constants.h"

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(FreshRecordTest)

BOOST_AUTO_TEST_CASE(field)
{
	std::vector<const FieldManipulator*> fms;
	fms.push_back(&IntFieldManipulator::instance());
	fms.push_back(&DoubleFieldManipulator::instance());
	fms.push_back(&IntFieldManipulator::instance());

	FreshRecord record(fms);
	record.field(0).set_int(23);
	record.field(1).set_double(9.0);
	record.field(2).set_int(84);

	// Check that we can retrieve the field values from the record correctly.
	BOOST_CHECK_EQUAL(record.field(0).get_int(), 23);
	BOOST_CHECK_CLOSE(record.field(1).get_double(), 9.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(record.field(2).get_int(), 84);
}

BOOST_AUTO_TEST_SUITE_END()
