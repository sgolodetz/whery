/**
 * test-db: FieldManipulatorTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <vector>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
using namespace boost::uuids;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/UuidFieldManipulator.h"
using namespace whery;

#include "Constants.h"

//#################### GLOBAL VARIABLES ####################

const DoubleFieldManipulator& dfm = DoubleFieldManipulator::instance();
std::vector<char> dbuffer(dfm.size());
char *dloc = &dbuffer[0];

const IntFieldManipulator& ifm = IntFieldManipulator::instance();
std::vector<char> ibuffer(ifm.size());
char *iloc = &ibuffer[0];

const UuidFieldManipulator& ufm = UuidFieldManipulator::instance();
std::vector<char> ubuffer(ufm.size());
char *uloc = &ubuffer[0];

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(FieldManipulatorTest)

BOOST_AUTO_TEST_CASE(dfm_setdouble_getdouble)
{
	dfm.set_double(dloc, 7.851);
	BOOST_CHECK_CLOSE(dfm.get_double(dloc), 7.851, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_CASE(dfm_setdouble_getint)
{
	dfm.set_double(dloc, 17.1051);
	BOOST_CHECK_EQUAL(dfm.get_int(dloc), 17);
}

BOOST_AUTO_TEST_CASE(dfm_setint_getdouble)
{
	dfm.set_int(dloc, 24);
	BOOST_CHECK_CLOSE(dfm.get_double(dloc), 24.0, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_CASE(ifm_setdouble_getint)
{
	ifm.set_double(iloc, 9.84);
	BOOST_CHECK_EQUAL(ifm.get_int(iloc), 9);
}

BOOST_AUTO_TEST_CASE(ifm_setint_getdouble)
{
	ifm.set_int(iloc, 23);
	BOOST_CHECK_CLOSE(ifm.get_double(iloc), 23.0, Constants::SMALL_EPSILON);
}

BOOST_AUTO_TEST_CASE(ifm_setint_getint)
{
	ifm.set_int(iloc, 84);
	BOOST_CHECK_EQUAL(ifm.get_int(iloc), 84);
}

BOOST_AUTO_TEST_CASE(ufm_setuuid_getstring)
{
	std::string s = "01234567-89ab-cdef-0123-456789abcdef";
	uuid u = string_generator()(s);
	ufm.set_uuid(uloc, u);
	BOOST_CHECK_EQUAL(ufm.get_string(uloc), s);
}

BOOST_AUTO_TEST_CASE(ufm_setuuid_getuuid)
{
	uuid u = string_generator()("01234567-89ab-cdef-0123-456789abcdef");
	ufm.set_uuid(uloc, u);
	BOOST_CHECK_EQUAL(ufm.get_uuid(uloc), u);
}

BOOST_AUTO_TEST_SUITE_END()
