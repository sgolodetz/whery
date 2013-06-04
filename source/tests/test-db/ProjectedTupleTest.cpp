/**
 * test-db: ProjectedTupleTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/FreshTuple.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/ProjectedTuple.h"
using namespace whery;

#include "Constants.h"

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(ProjectedTupleTest)

BOOST_AUTO_TEST_CASE(field)
{
	std::vector<const FieldManipulator*> fieldManipulators = list_of<const FieldManipulator*>
		(&DoubleFieldManipulator::instance())
		(&IntFieldManipulator::instance());

	FreshTuple tuple(fieldManipulators);
	tuple.field(0).set_double(7.0);
	tuple.field(1).set_int(8);

	ProjectedTuple projected(tuple, list_of(1)(0)(0)(1));

	// Check that we can retrieve the field values from the projection correctly.
	BOOST_CHECK_EQUAL(projected.field(0).get_int(), 8);
	BOOST_CHECK_CLOSE(projected.field(1).get_double(), 7.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_CLOSE(projected.field(2).get_double(), 7.0, Constants::SMALL_EPSILON);
	BOOST_CHECK_EQUAL(projected.field(3).get_int(), 8);
}

BOOST_AUTO_TEST_SUITE_END()
