/**
 * test-db: DataPageTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost::assign;

#include "whery/db/base/DataPage.h"
#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/RangeKey.h"
#include "whery/db/base/ValueKey.h"
using namespace whery;

#include "Constants.h"

//#################### TYPEDEFS ####################

typedef boost::shared_ptr<DataPage> DataPage_Ptr;

//#################### HELPER FUNCTIONS ####################

namespace {

DataPage_Ptr make_big_data_page()
{
	const unsigned int TUPLE_COUNT = 20;

	TupleManipulator tupleManipulator(list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
	);

	DataPage_Ptr page(new DataPage(tupleManipulator.size() * TUPLE_COUNT, tupleManipulator));

	for(unsigned int i = 0; i < TUPLE_COUNT; ++i)
	{
		BackedTuple tuple = page->add_tuple();
		tuple.field(0).set_int(i);
	}

	return page;
}

DataPage_Ptr make_small_data_page()
{
	const int PAGE_BUFFER_SIZE = 1024;

	DataPage_Ptr page(new DataPage(list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&DoubleFieldManipulator::instance())
		(&IntFieldManipulator::instance()),
		PAGE_BUFFER_SIZE
	));

	BackedTuple tuple = page->add_tuple();
	tuple.field(0).set_int(23);
	tuple.field(1).set_double(9.0);
	tuple.field(2).set_int(84);

	tuple = page->add_tuple();
	tuple.field(0).set_int(7);
	tuple.field(1).set_double(8.0);
	tuple.field(2).set_int(51);

	tuple = page->add_tuple();
	tuple.field(0).set_int(17);
	tuple.field(1).set_double(10.0);
	tuple.field(2).set_int(51);

	return page;
}

}

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(DataPageTest)

BOOST_AUTO_TEST_CASE(delete_tuple)
{
	DataPage_Ptr page = make_small_data_page();
	std::vector<BackedTuple> tuples = page->tuples();

	// Check that the page has the right number of tuples to start with.
	BOOST_CHECK_EQUAL(page->tuple_count(), 3);

	page->delete_tuple(tuples[1]);

	// Check that deleting a tuples decreases the tuple count of the page
	// and leaves the other tuples unaffected.
	BOOST_CHECK_EQUAL(page->tuple_count(), 2);
	BOOST_CHECK_EQUAL(tuples[0].field(0).get_int(), 23);
	BOOST_CHECK_EQUAL(tuples[2].field(0).get_int(), 17);

	BackedTuple t = page->add_tuple();

	// Check that adding a tuple when there is a tuple on the free list
	// increases the tuple count of the page and reuses the tuple on
	// the free list (note that this is a white-box test that relies on
	// knowing details of the implementation).
	BOOST_CHECK_EQUAL(page->tuple_count(), 3);
	BOOST_CHECK_EQUAL(t.location(), tuples[1].location());

	t = page->add_tuple();

	// Check that adding a tuple when there is no tuple on the free list
	// increases the tuple count and allocates a new tuple.
	BOOST_CHECK_EQUAL(page->tuple_count(), 4);
	for(int i = 0; i < 3; ++i)
	{
		BOOST_CHECK_NE(t.location(), tuples[i].location());
	}
}

BOOST_AUTO_TEST_CASE(tuples_by_range)
{
	DataPage_Ptr page = make_big_data_page();

	// Check a [) range.
	RangeKey key(page->field_manipulators(), list_of(0));
	key.low_kind() = CLOSED;
	key.low_value().field(0).set_int(7);
	key.high_kind() = OPEN;
	key.high_value().field(0).set_int(9);
	std::vector<BackedTuple> results = page->tuples_by_range(key);

	BOOST_CHECK_EQUAL(results.size(), 2);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 7);
	BOOST_CHECK_EQUAL(results[1].field(0).get_int(), 8);

	// Check a [] range.
	key.high_kind() = CLOSED;
	results = page->tuples_by_range(key);

	BOOST_CHECK_EQUAL(results.size(), 3);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 7);
	BOOST_CHECK_EQUAL(results[1].field(0).get_int(), 8);
	BOOST_CHECK_EQUAL(results[2].field(0).get_int(), 9);

	// Check a (] range.
	key.low_kind() = OPEN;
	results = page->tuples_by_range(key);

	BOOST_CHECK_EQUAL(results.size(), 2);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 8);
	BOOST_CHECK_EQUAL(results[1].field(0).get_int(), 9);

	// Check a () range.
	key.high_kind() = OPEN;
	results = page->tuples_by_range(key);

	BOOST_CHECK_EQUAL(results.size(), 1);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 8);

	// Check a half-bounded range.
	key.clear_low_endpoint();
	results = page->tuples_by_range(key);

	BOOST_CHECK_EQUAL(results.size(), 9);
	BOOST_CHECK_EQUAL(results.front().field(0).get_int(), 0);
	BOOST_CHECK_EQUAL(results.back().field(0).get_int(), 8);

	// Check an unbounded range.
	key.clear_high_endpoint();
	results = page->tuples_by_range(key);

	BOOST_CHECK_EQUAL(results.size(), page->tuple_count());
	BOOST_CHECK_EQUAL(results.front().field(0).get_int(), 0);
	BOOST_CHECK_EQUAL(results.back().field(0).get_int(), page->tuple_count() - 1);
}

BOOST_AUTO_TEST_CASE(tuples_by_value)
{
	DataPage_Ptr page = make_small_data_page();

	ValueKey key(page->field_manipulators(), list_of(0));
	key.field(0).set_int(7);
	std::vector<BackedTuple> results = page->tuples_by_value(key);

	BOOST_CHECK_EQUAL(results.size(), 1);
	BOOST_CHECK_CLOSE(results[0].field(1).get_double(), 8.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(results[0].field(2).get_int(), 51);

	key = ValueKey(page->field_manipulators(), list_of(1));
	key.field(0).set_double(9.0);
	results = page->tuples_by_value(key);

	BOOST_CHECK_EQUAL(results.size(), 1);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 23);
	BOOST_CHECK_EQUAL(results[0].field(2).get_int(), 84);

	key = ValueKey(page->field_manipulators(), list_of(2));
	key.field(0).set_int(51);
	results = page->tuples_by_value(key);

	BOOST_CHECK_EQUAL(results.size(), 2);
	BOOST_CHECK_EQUAL(results[0].field(0).get_int(), 7);
	BOOST_CHECK_CLOSE(results[0].field(1).get_double(), 8.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(results[1].field(0).get_int(), 17);
	BOOST_CHECK_CLOSE(results[1].field(1).get_double(), 10.0, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()
