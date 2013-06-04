/**
 * test-db: BTreeTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/btrees/BTree.h"
#include "whery/db/pages/InMemorySortedPage.h"
using namespace whery;

//#################### HELPER CLASSES ####################

class TestPageController : public BTreePageController
{
	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual SortedPage_Ptr make_btree_branch_page() const
	{
		const unsigned int TUPLES_PER_PAGE = 10;

		// For a primary B+-tree index, tuples in branch nodes are pairs:
		// - The first component contains (unique) values from the first column of the relation.
		// - The second component stores the ID of a child node.
		TupleManipulator tupleManipulator(list_of<const FieldManipulator*>
			(&IntFieldManipulator::instance())
			(&IntFieldManipulator::instance())
		);

		return SortedPage_Ptr(new InMemorySortedPage(tupleManipulator.size() * TUPLES_PER_PAGE, tupleManipulator));
	}

	virtual SortedPage_Ptr make_btree_leaf_page() const
	{
		const unsigned int PAGE_BUFFER_SIZE = 128;

		return SortedPage_Ptr(new InMemorySortedPage(list_of<const FieldManipulator*>
			(&IntFieldManipulator::instance())
			(&DoubleFieldManipulator::instance())
			(&DoubleFieldManipulator::instance()),
			PAGE_BUFFER_SIZE
		));
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
	// TODO
}

BOOST_AUTO_TEST_SUITE_END()
