/**
 * test-db: DataPageTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/shared_ptr.hpp>

#include "whery/db/DataPage.h"
#include "whery/db/DoubleFieldManipulator.h"
#include "whery/db/FreshRecord.h"
#include "whery/db/IntFieldManipulator.h"
using namespace whery;

#include "Constants.h"

//#################### TYPEDEFS ####################

typedef boost::shared_ptr<DataPage> DataPage_Ptr;

//#################### HELPER FUNCTIONS ####################

namespace {

DataPage_Ptr make_data_page()
{
	const int PAGE_BUFFER_SIZE = 1024;

	std::vector<const FieldManipulator*> recordFieldManipulators;
	recordFieldManipulators.push_back(&IntFieldManipulator::instance());
	recordFieldManipulators.push_back(&DoubleFieldManipulator::instance());
	recordFieldManipulators.push_back(&IntFieldManipulator::instance());
	DataPage_Ptr page(new DataPage(recordFieldManipulators, PAGE_BUFFER_SIZE));

	Record record = page->add_record();
	record.field(0).set_int(23);
	record.field(1).set_double(9.0);
	record.field(2).set_int(84);

	record = page->add_record();
	record.field(0).set_int(7);
	record.field(1).set_double(8.0);
	record.field(2).set_int(51);

	record = page->add_record();
	record.field(0).set_int(17);
	record.field(1).set_double(10.0);
	record.field(2).set_int(51);

	return page;
}

}

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(DataPageTest)

BOOST_AUTO_TEST_CASE(delete_record)
{
	DataPage_Ptr page = make_data_page();
	std::vector<Record> records = page->records();

	// Check that the page has the right number of records to start with.
	BOOST_CHECK_EQUAL(page->record_count(), 3);

	page->delete_record(records[1]);

	// Check that deleting a record decreases the record count of the page
	// and leaves the other records unaffected.
	BOOST_CHECK_EQUAL(page->record_count(), 2);
	BOOST_CHECK_EQUAL(records[0].field(0).get_int(), 23);
	BOOST_CHECK_EQUAL(records[2].field(0).get_int(), 17);

	Record r = page->add_record();

	// Check that adding a record when there is a record on the free list
	// increases the record count of the page and reuses the record on
	// the free list (note that this is a white-box test that relies on
	// knowing details of the implementation).
	BOOST_CHECK_EQUAL(page->record_count(), 3);
	BOOST_CHECK_EQUAL(r.location(), records[1].location());

	r = page->add_record();

	// Check that adding a record when there is no record on the free list
	// increases the record count and allocates a new record.
	BOOST_CHECK_EQUAL(page->record_count(), 4);
	for(int i = 0; i < 3; ++i)
	{
		BOOST_CHECK_NE(r.location(), records[i].location());
	}
}

BOOST_AUTO_TEST_CASE(records_by_value)
{
	DataPage_Ptr page = make_data_page();

	std::vector<unsigned int> projectedFields;
	projectedFields.push_back(0);
	FreshRecord key(page->field_manipulators(), projectedFields);
	key.field(0).set_int(7);
	std::vector<Record> results = page->records_by_value(projectedFields, key);

	BOOST_CHECK_EQUAL(results.size(), 1);
	BOOST_CHECK_CLOSE(results[0].field(1).get_double(), 8.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(results[0].field(2).get_int(), 51);

	projectedFields.clear();
	projectedFields.push_back(1);
	key = FreshRecord(page->field_manipulators(), projectedFields);
	key.field(0).set_double(9.0);
	results = page->records_by_value(projectedFields, key);

	BOOST_CHECK_EQUAL(results.size(), 1);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 23);
	BOOST_CHECK_EQUAL(results[0].field(2).get_int(), 84);

	projectedFields.clear();
	projectedFields.push_back(2);
	key = FreshRecord(page->field_manipulators(), projectedFields);
	key.field(0).set_int(51);
	results = page->records_by_value(projectedFields, key);

	BOOST_CHECK_EQUAL(results.size(), 2);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 7);
	BOOST_CHECK_CLOSE(results[0].field(1).get_double(), 8.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(results[1].field(0).get_int(), 17);
	BOOST_CHECK_CLOSE(results[1].field(1).get_double(), 10.0, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()
