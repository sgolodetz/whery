/**
 * test-db: InMemorySortedPageTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/RangeKey.h"
#include "whery/db/pages/InMemorySortedPage.h"
using namespace whery;

#include "Constants.h"

//#################### HELPER FUNCTIONS ####################

namespace {

void check_tuple(const BackedTuple& tuple, int i, int j, int k)
{
	BOOST_CHECK_EQUAL(tuple.field(0).get_int(), i);
	BOOST_CHECK_EQUAL(tuple.field(1).get_int(), j);
	BOOST_CHECK_EQUAL(tuple.field(2).get_int(), k);
}

InMemorySortedPage make_prefix_page()
{
	const unsigned int N = 5;

	TupleManipulator tupleManipulator(list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&IntFieldManipulator::instance())
		(&IntFieldManipulator::instance())
	);

	InMemorySortedPage page(tupleManipulator.size() * N * N * N, tupleManipulator);

	FreshTuple tuple(page.field_manipulators());
	for(unsigned int i = 0; i < N; ++i)
	{
		for(unsigned int j = 0; j < N; ++j)
		{
			for(unsigned int k = 0; k < N; ++k)
			{
				tuple.field(0).set_int(i);
				tuple.field(1).set_int(j);
				tuple.field(2).set_int(k);
				page.add_tuple(tuple);
			}
		}
	}

	return page;
}

InMemorySortedPage make_small_page()
{
	const int PAGE_BUFFER_SIZE = 1024;

	InMemorySortedPage page(list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&DoubleFieldManipulator::instance())
		(&IntFieldManipulator::instance()),
		PAGE_BUFFER_SIZE
	);

	FreshTuple tuple(page.field_manipulators());

	tuple.field(0).set_int(23);
	tuple.field(1).set_double(9.0);
	tuple.field(2).set_int(84);
	page.add_tuple(tuple);

	tuple.field(0).set_int(7);
	tuple.field(1).set_double(8.0);
	tuple.field(2).set_int(51);
	page.add_tuple(tuple);

	tuple.field(0).set_int(17);
	tuple.field(1).set_double(10.0);
	tuple.field(2).set_int(51);
	page.add_tuple(tuple);

	return page;
}

}

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(InMemorySortedPageTest)

BOOST_AUTO_TEST_CASE(equal_range_rangekey)
{
	InMemorySortedPage page = make_prefix_page();

	// Check a [] range.
	RangeKey key(page.field_manipulators(), list_of(0)(1));
	key.low_kind() = CLOSED;
	key.low_value().field(0).set_int(2);
	key.low_value().field(1).set_int(4);
	key.high_kind() = CLOSED;
	key.high_value().field(0).set_int(3);
	key.high_value().field(1).set_int(0);
	InMemorySortedPage::EqualRangeResult result = page.equal_range(key);
	std::vector<BackedTuple> tuples(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 10);
	check_tuple(tuples[0], 2, 4, 0);
	check_tuple(tuples[9], 3, 0, 4);

	// Check a [) range.
	key.high_kind() = OPEN;
	result = page.equal_range(key);
	tuples = std::vector<BackedTuple>(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 5);
	check_tuple(tuples[0], 2, 4, 0);
	check_tuple(tuples[4], 2, 4, 4);

	// Check a () range.
	key.low_kind() = OPEN;
	result = page.equal_range(key);
	tuples = std::vector<BackedTuple>(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 0);

	// Check a (] range.
	key.high_kind() = CLOSED;
	result = page.equal_range(key);
	tuples = std::vector<BackedTuple>(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 5);
	check_tuple(tuples[0], 3, 0, 0);
	check_tuple(tuples[4], 3, 0, 4);

	// Check a half-bounded range.
	key.clear_high_endpoint();
	result = page.equal_range(key);
	tuples = std::vector<BackedTuple>(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 50);
	check_tuple(tuples[0], 3, 0, 0);
	check_tuple(tuples[49], 4, 4, 4);

	// Check an unbounded range.
	key.clear_low_endpoint();
	result = page.equal_range(key);
	tuples = std::vector<BackedTuple>(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 125);
	check_tuple(tuples[0], 0, 0, 0);
	check_tuple(tuples[124], 4, 4, 4);
}

BOOST_AUTO_TEST_CASE(equal_range_valuekey)
{
	InMemorySortedPage page = make_prefix_page();

	ValueKey key(page.field_manipulators(), list_of(0));
	key.field(0).set_int(2);
	InMemorySortedPage::EqualRangeResult result = page.equal_range(key);
	std::vector<BackedTuple> tuples(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 25);
	check_tuple(tuples[0], 2, 0, 0);
	check_tuple(tuples[24], 2, 4, 4);

	key = ValueKey(page.field_manipulators(), list_of(0)(1));
	key.field(0).set_int(2);
	key.field(1).set_int(3);
	result = page.equal_range(key);
	tuples = std::vector<BackedTuple>(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 5);
	check_tuple(tuples[0], 2, 3, 0);
	check_tuple(tuples[4], 2, 3, 4);

	key = ValueKey(page.field_manipulators(), list_of(0)(1)(2));
	key.field(0).set_int(2);
	key.field(1).set_int(3);
	key.field(2).set_int(1);
	result = page.equal_range(key);
	tuples = std::vector<BackedTuple>(result.first, result.second);

	BOOST_CHECK_EQUAL(tuples.size(), 1);
	check_tuple(tuples[0], 2, 3, 1);
}

BOOST_AUTO_TEST_CASE(erase_tuple)
{
	InMemorySortedPage page = make_small_page();
	std::vector<BackedTuple> tuples(page.begin(), page.end());

	// Check that the page has the right number of tuples to start with.
	BOOST_CHECK_EQUAL(page.tuple_count(), 3);

	// Erase the second tuple in the page, i.e. (17,10.0,51) since the page is sorted.
	page.erase_tuple(tuples[1]);

	// Check that deleting a tuple decreases the tuple count of the page
	// and leaves the other tuples unaffected.
	BOOST_CHECK_EQUAL(page.tuple_count(), 2);
	BOOST_CHECK_EQUAL(tuples[0].field(0).get_int(), 7);
	BOOST_CHECK_EQUAL(tuples[2].field(0).get_int(), 23);

	FreshTuple t(page.field_manipulators());
	page.add_tuple(t);

	// Check that adding a tuple when there is a tuple on the free list
	// increases the tuple count of the page.
	BOOST_CHECK_EQUAL(page.tuple_count(), 3);

	page.add_tuple(t);

	// Check that adding a tuple when there is no tuple on the free list
	// increases the tuple count of the page.
	BOOST_CHECK_EQUAL(page.tuple_count(), 4);
}

BOOST_AUTO_TEST_SUITE_END()
