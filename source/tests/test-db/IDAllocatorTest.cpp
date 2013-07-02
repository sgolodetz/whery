/**
 * test-db: IDAllocatorTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include "whery/util/IDAllocator.h"
using namespace whery;

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(IDAllocatorTest)

BOOST_AUTO_TEST_CASE(deallocate)
{
	IDAllocator a;

	for(int i = 0; i <= 10; ++i)
	{
		BOOST_CHECK_EQUAL(a.allocate(), i);
	}

	a.deallocate(7);
	a.deallocate(3);
	a.deallocate(5);
	a.deallocate(2);

	BOOST_CHECK_EQUAL(a.allocate(), 2);
	BOOST_CHECK_EQUAL(a.allocate(), 3);
	BOOST_CHECK_EQUAL(a.allocate(), 5);
	BOOST_CHECK_EQUAL(a.allocate(), 7);
	BOOST_CHECK_EQUAL(a.allocate(), 11);
}

BOOST_AUTO_TEST_CASE(reset)
{
	IDAllocator a;

	for(int i = 0; i <= 10; ++i)
	{
		if(i % 3 == 0)
		{
			a.reset();
		}

		BOOST_CHECK_EQUAL(a.allocate(), i % 3);
	}
}

BOOST_AUTO_TEST_SUITE_END()
