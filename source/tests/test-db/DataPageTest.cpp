/**
 * test-db: DataPageTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include "whery/db/DataPage.h"
#include "whery/db/DoubleFieldManipulator.h"
#include "whery/db/FreshRecord.h"
#include "whery/db/IntFieldManipulator.h"
using namespace whery;

#include "Constants.h"

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(DataPageTest)

BOOST_AUTO_TEST_CASE(records_by_value)
{
	std::vector<const FieldManipulator*> recordFieldManipulators;
	recordFieldManipulators.push_back(&IntFieldManipulator::instance());
	recordFieldManipulators.push_back(&DoubleFieldManipulator::instance());
	recordFieldManipulators.push_back(&IntFieldManipulator::instance());
	DataPage page(recordFieldManipulators);

	Record record = page.add_record();
	record.field(0).set_int(23);
	record.field(1).set_double(9.0);
	record.field(2).set_int(84);

	record = page.add_record();
	record.field(0).set_int(7);
	record.field(1).set_double(8.0);
	record.field(2).set_int(51);

	record = page.add_record();
	record.field(0).set_int(17);
	record.field(1).set_double(10.0);
	record.field(2).set_int(51);

	std::vector<unsigned int> projectedFields;
	projectedFields.push_back(0);
	FreshRecord key(recordFieldManipulators, projectedFields);
	key.field(0).set_int(7);
	std::vector<Record> results = page.records_by_value(projectedFields, key);

	BOOST_CHECK_EQUAL(results.size(), 1);
	BOOST_CHECK_CLOSE(results[0].field(1).get_double(), 8.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(results[0].field(2).get_int(), 51);

	projectedFields.clear();
	projectedFields.push_back(1);
	key = FreshRecord(recordFieldManipulators, projectedFields);
	key.field(0).set_double(9.0);
	results = page.records_by_value(projectedFields, key);

	BOOST_CHECK_EQUAL(results.size(), 1);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 23);
	BOOST_CHECK_EQUAL(results[0].field(2).get_int(), 84);

	projectedFields.clear();
	projectedFields.push_back(2);
	key = FreshRecord(recordFieldManipulators, projectedFields);
	key.field(0).set_int(51);
	results = page.records_by_value(projectedFields, key);

	BOOST_CHECK_EQUAL(results.size(), 2);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 7);
	BOOST_CHECK_CLOSE(results[0].field(1).get_double(), 8.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(results[1].field(0).get_int(), 17);
	BOOST_CHECK_CLOSE(results[1].field(1).get_double(), 10.0, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()
