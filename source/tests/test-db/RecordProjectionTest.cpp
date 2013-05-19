/**
 * test-db: RecordProjectionTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

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
	std::vector<const FieldManipulator*> fms;
	fms.push_back(&DoubleFieldManipulator::instance());
	fms.push_back(&IntFieldManipulator::instance());

	FreshRecord record(fms);
	record.field(0).set_double(7.0);
	record.field(1).set_int(8);

	std::vector<unsigned int> projectedFields;
	projectedFields.push_back(1);
	projectedFields.push_back(0);
	projectedFields.push_back(0);
	projectedFields.push_back(1);
	RecordProjection projection(record, projectedFields);

	// Check that we can retrieve the field values from the projection correctly.
	BOOST_CHECK_EQUAL(projection.field(0).get_int(), 8);
	BOOST_CHECK_CLOSE(projection.field(1).get_double(), 7.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_CLOSE(projection.field(2).get_double(), 7.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(projection.field(3).get_int(), 8);
}

BOOST_AUTO_TEST_SUITE_END()
