/**
 * test-db: RecordProjectionTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/DoubleFieldManipulator.h"
#include "whery/db/FreshRecord.h"
#include "whery/db/IntFieldManipulator.h"
#include "whery/db/RecordProjection.h"
using namespace whery;

#include "Constants.h"

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(RecordProjectionTest)

BOOST_AUTO_TEST_CASE(field)
{
	std::vector<const FieldManipulator*> fms = list_of<const FieldManipulator*>
		(&DoubleFieldManipulator::instance())
		(&IntFieldManipulator::instance());

	FreshRecord record(fms);
	record.field(0).set_double(7.0);
	record.field(1).set_int(8);

	RecordProjection projection(record, list_of(1)(0)(0)(1));

	// Check that we can retrieve the field values from the projection correctly.
	BOOST_CHECK_EQUAL(projection.field(0).get_int(), 8);
	BOOST_CHECK_CLOSE(projection.field(1).get_double(), 7.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_CLOSE(projection.field(2).get_double(), 7.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(projection.field(3).get_int(), 8);
}

BOOST_AUTO_TEST_SUITE_END()
