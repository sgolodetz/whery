/**
 * test-db: BTreeTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/FreshTuple.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/btrees/BTree.h"
#include "whery/db/pages/InMemorySortedPage.h"
using namespace whery;

//#################### HELPER CLASSES ####################

class TestPageController : public BTreePageController
{
	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual TupleManipulator btree_branch_tuple_manipulator() const
	{
		// For a primary B+-tree index, tuples in branch nodes are pairs:
		// - The first component contains (unique) values from the first column of the relation.
		// - The second component stores the ID of a child node.
		return TupleManipulator(list_of<const FieldManipulator*>
			(&IntFieldManipulator::instance())
			(&IntFieldManipulator::instance())
		);
	}

	virtual TupleManipulator btree_leaf_tuple_manipulator() const
	{
		return TupleManipulator(list_of<const FieldManipulator*>
			(&IntFieldManipulator::instance())
			(&DoubleFieldManipulator::instance())
			(&DoubleFieldManipulator::instance())
		);
	}

	virtual SortedPage_Ptr make_btree_branch_page() const
	{
		const unsigned int TUPLES_PER_PAGE = 2;
		TupleManipulator tupleManipulator = btree_branch_tuple_manipulator();
		return SortedPage_Ptr(new InMemorySortedPage(tupleManipulator.size() * TUPLES_PER_PAGE, tupleManipulator));
	}

	virtual SortedPage_Ptr make_btree_leaf_page() const
	{
		const unsigned int TUPLES_PER_PAGE = 2;
		TupleManipulator tupleManipulator = btree_leaf_tuple_manipulator();
		return SortedPage_Ptr(new InMemorySortedPage(tupleManipulator.size() * TUPLES_PER_PAGE, tupleManipulator));
	}
};

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(BTreeTest)

BOOST_AUTO_TEST_CASE(constructor)
{
	BTree tree(BTreePageController_CPtr(new TestPageController));
	BOOST_CHECK_EQUAL(tree.tuple_count(), 0);
	BOOST_CHECK(tree.begin() == tree.end());
}

BOOST_AUTO_TEST_CASE(insert_tuple)
{
	BTree tree(BTreePageController_CPtr(new TestPageController));
	FreshTuple tuple(tree.leaf_tuple_manipulator());

	/*tuple.field(0).set_int(0);
	tuple.field(1).set_double(23.0);
	tuple.field(2).set_double(9.0);
	tree.insert_tuple(tuple);

	tuple.field(0).set_int(1);
	tuple.field(1).set_double(7.0);
	tuple.field(2).set_double(8.0);
	tree.insert_tuple(tuple);

	tuple.field(0).set_int(2);
	tuple.field(1).set_double(17.0);
	tuple.field(2).set_double(10.0);
	tree.insert_tuple(tuple);*/

	for(int i = 0; i < 7; ++i)
	{
		tuple.field(0).set_int(i);
		tuple.field(1).set_double(i * i);
		tuple.field(2).set_double(i * i * i);
		tree.insert_tuple(tuple);
	}
}

BOOST_AUTO_TEST_SUITE_END()
