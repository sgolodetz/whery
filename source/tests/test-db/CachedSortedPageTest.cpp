/**
 * test-db: CachedSortedPageTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/FreshTuple.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/pages/CachedSortedPage.h"
#include "whery/db/pages/InMemorySortedPage.h"
using namespace whery;

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(CachedSortedPageTest)

BOOST_AUTO_TEST_CASE(simple)
{
	const int MAX_CACHE_BYTES = 32768;
	const int PAGE_BUFFER_SIZE = 1024;

	PageCache_Ptr cache(new PageCache(MAX_CACHE_BYTES));
	PageCacheID id = cache->add_page(InMemorySortedPage_Ptr(new InMemorySortedPage(list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&IntFieldManipulator::instance()),
		PAGE_BUFFER_SIZE
	)));
	CachedSortedPage page(id, cache);
	FreshTuple tuple(page.field_manipulators());

	tuple.field(0).set_int(23);
	tuple.field(1).set_int(9);
	page.add_tuple(tuple);

	tuple.field(0).set_int(7);
	tuple.field(1).set_int(8);
	page.add_tuple(tuple);

	std::vector<BackedTuple> tuples(page.begin(), page.end());

	BOOST_CHECK_EQUAL(tuples.size(), 2);
	BOOST_CHECK_EQUAL(tuples[0].field(0).get_int(), 7);
	BOOST_CHECK_EQUAL(tuples[0].field(1).get_int(), 8);
	BOOST_CHECK_EQUAL(tuples[1].field(0).get_int(), 23);
	BOOST_CHECK_EQUAL(tuples[1].field(1).get_int(), 9);
}

BOOST_AUTO_TEST_SUITE_END()
