/**
 * test-db: BTreeDataPageTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost::assign;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/RangeKey.h"
#include "whery/db/base/ValueKey.h"
#include "whery/db/btrees/BTreeDataPage.h"
using namespace whery;

#include "Constants.h"

//#################### TYPEDEFS ####################

typedef boost::shared_ptr<BTreeDataPage> BTreeDataPage_Ptr;

//#################### HELPER FUNCTIONS ####################

namespace {

BTreeDataPage_Ptr make_big_data_page()
{
	const unsigned int TUPLE_COUNT = 20;

	TupleManipulator tupleManipulator(list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
	);

	BTreeDataPage_Ptr page(new BTreeDataPage(tupleManipulator.size() * TUPLE_COUNT, tupleManipulator));

	FreshTuple tuple(page->field_manipulators());
	for(unsigned int i = 0; i < TUPLE_COUNT; ++i)
	{
		tuple.field(0).set_int(i);
		page->add_tuple(tuple);
	}

	return page;
}

BTreeDataPage_Ptr make_small_data_page()
{
	const int PAGE_BUFFER_SIZE = 1024;

	BTreeDataPage_Ptr page(new BTreeDataPage(list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&DoubleFieldManipulator::instance())
		(&IntFieldManipulator::instance()),
		PAGE_BUFFER_SIZE
	));

	FreshTuple tuple(page->field_manipulators());

	tuple.field(0).set_int(23);
	tuple.field(1).set_double(9.0);
	tuple.field(2).set_int(84);
	page->add_tuple(tuple);

	tuple.field(0).set_int(7);
	tuple.field(1).set_double(8.0);
	tuple.field(2).set_int(51);
	page->add_tuple(tuple);

	tuple.field(0).set_int(17);
	tuple.field(1).set_double(10.0);
	tuple.field(2).set_int(51);
	page->add_tuple(tuple);

	return page;
}

}

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(BTreeDataPageTest)

BOOST_AUTO_TEST_CASE(delete_tuple)
{
	BTreeDataPage_Ptr page = make_small_data_page();
	std::vector<BackedTuple> tuples(page->tuples().begin(), page->tuples().end());

	// Check that the page has the right number of tuples to start with.
	BOOST_CHECK_EQUAL(page->tuple_count(), 3);

	// Delete the second tuple in the page, i.e. (17,10.0,51) since the page is sorted.
	page->delete_tuple(tuples[1]);

	// Check that deleting a tuples decreases the tuple count of the page
	// and leaves the other tuples unaffected.
	BOOST_CHECK_EQUAL(page->tuple_count(), 2);
	BOOST_CHECK_EQUAL(tuples[0].field(0).get_int(), 7);
	BOOST_CHECK_EQUAL(tuples[2].field(0).get_int(), 23);

	FreshTuple t(page->field_manipulators());
	page->add_tuple(t);

	// Check that adding a tuple when there is a tuple on the free list
	// increases the tuple count of the page.
	BOOST_CHECK_EQUAL(page->tuple_count(), 3);

	page->add_tuple(t);

	// Check that adding a tuple when there is no tuple on the free list
	// increases the tuple count of the page.
	BOOST_CHECK_EQUAL(page->tuple_count(), 4);
}

BOOST_AUTO_TEST_CASE(tuples_by_range)
{
	BTreeDataPage_Ptr page = make_big_data_page();

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
	BTreeDataPage_Ptr page = make_small_data_page();

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
