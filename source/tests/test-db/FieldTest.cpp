/**
 * test-db: FieldTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <vector>

#include "whery/db/DoubleFieldManipulator.h"
#include "whery/db/Field.h"
#include "whery/db/IntFieldManipulator.h"
using namespace whery;

#include "Constants.h"

//#################### GLOBAL VARIABLES ####################

std::vector<char> buffer1(IntFieldManipulator::instance().size());
std::vector<char> buffer2(DoubleFieldManipulator::instance().size());
Field f1(&buffer1[0], IntFieldManipulator::instance());
Field f2(&buffer2[0], DoubleFieldManipulator::instance());

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(FieldTest)

BOOST_AUTO_TEST_CASE(compare_to)
{
	f1.set_int(23);
	f2.set_double(9.0);
	BOOST_CHECK_EQUAL(f1.compare_to(f1), 0);
	BOOST_CHECK_EQUAL(f2.compare_to(f2), 0);
	BOOST_CHECK_EQUAL(f1.compare_to(f2), 1);
	BOOST_CHECK_EQUAL(f2.compare_to(f1), -1);

	f2.set_double(23.0);
	BOOST_CHECK_EQUAL(f1.compare_to(f2), 0);
	BOOST_CHECK_EQUAL(f2.compare_to(f1), 0);
}

BOOST_AUTO_TEST_CASE(set_from)
{
	f2.set_double(24.0);
	f1.set_from(f2);
	BOOST_CHECK_EQUAL(f1.get_int(), 24);

	f1.set_int(12);
	f2.set_from(f1);
	BOOST_CHECK_CLOSE(f2.get_double(), 12.0, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()
