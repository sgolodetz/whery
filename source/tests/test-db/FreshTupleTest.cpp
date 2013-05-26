/**
 * test-db: FreshTupleTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/FreshTuple.h"
#include "whery/db/base/IntFieldManipulator.h"
using namespace whery;

#include "Constants.h"

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(FreshTupleTest)

BOOST_AUTO_TEST_CASE(field)
{
	std::vector<const FieldManipulator*> fms = list_of<const FieldManipulator*>
		(&IntFieldManipulator::instance())
		(&DoubleFieldManipulator::instance())
		(&IntFieldManipulator::instance());

	FreshTuple tuple(fms);
	tuple.field(0).set_int(23);
	tuple.field(1).set_double(9.0);
	tuple.field(2).set_int(84);

	// Check that we can retrieve the field values from the tuple correctly.
	BOOST_CHECK_EQUAL(tuple.field(0).get_int(), 23);
	BOOST_CHECK_CLOSE(tuple.field(1).get_double(), 9.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(tuple.field(2).get_int(), 84);
}

BOOST_AUTO_TEST_SUITE_END()
