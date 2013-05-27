/**
 * test-db: PrefixTupleComparatorTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <sstream>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/FreshTuple.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/PrefixTupleComparator.h"
using namespace whery;

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(PrefixTupleComparatorTest)

BOOST_AUTO_TEST_CASE(compare)
{
	std::vector<const FieldManipulator*> fms1 = list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance());

	std::vector<const FieldManipulator*> fms2 = list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&IntFieldManipulator::instance());

	std::vector<const FieldManipulator*> fms3 = list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&IntFieldManipulator::instance())
		(&IntFieldManipulator::instance());

	const int TUPLE_COUNT = 6;

	FreshTuple t[TUPLE_COUNT] = {
		FreshTuple(fms2),
		FreshTuple(fms3),
		FreshTuple(fms1),
		FreshTuple(fms2),
		FreshTuple(fms2),
		FreshTuple(fms3)
	};

	t[0].field(0).set_int(7);	t[0].field(1).set_int(8);
	t[1].field(0).set_int(7);	t[1].field(1).set_int(8);	t[1].field(2).set_int(51);
	t[2].field(0).set_int(17);
	t[3].field(0).set_int(17);	t[3].field(1).set_int(10);
	t[4].field(0).set_int(23);	t[4].field(1).set_int(9);
	t[5].field(0).set_int(23);	t[5].field(1).set_int(9);	t[5].field(2).set_int(84);

	PrefixTupleComparator c;
	for(int i = 0; i < TUPLE_COUNT; ++i)
	{
		for(int j = 0; j < TUPLE_COUNT; ++j)
		{
			int actual = c.compare(t[i], t[j]);

			int expected;
			if(i < j) expected = -1;
			else if(i > j) expected = 1;
			else expected = 0;

			std::stringstream ss;
			ss << '(' << i << ',' << j << ") check actual == " << expected << " failed [" << actual << " != " << expected << ']';

			BOOST_CHECK_MESSAGE(actual == expected, ss.str());
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
