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
#include "whery/db/base/RangeKey.h"
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

#if 0
BOOST_AUTO_TEST_CASE(equal_range_rangekey)
{
	BTree tree(BTreePageController_CPtr(new TestPageController));
	FreshTuple tuple(tree.leaf_tuple_manipulator());

	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			tuple.field(0).set_int(i);
			tuple.field(1).set_int(j);
			tuple.field(2).set_int(j);
			tree.insert_tuple(tuple);
		}
	}

	tree.print(std::cout);

	RangeKey key(tree.leaf_tuple_manipulator().field_manipulators(), list_of(0));
	key.low_kind() = OPEN;
	key.high_kind() = CLOSED;
	key.low_value().field(0).set_int(0);
	key.high_value().field(0).set_int(2);
	BTree::EqualRangeResult er = tree.equal_range(key);
	for(BTree::ConstIterator it = er.first; it != er.second; ++it)
	{
		std::cout << it->field(0).get_int() << ' ' << it->field(1).get_int() << '\n';
	}
}
#endif

BOOST_AUTO_TEST_CASE(erase_tuple)
{
	BTree tree(BTreePageController_CPtr(new TestPageController));
	FreshTuple tuple(tree.leaf_tuple_manipulator());

	int arr[] = {0,1,2,4,5,3};
	int size = sizeof(arr) / sizeof(int);
	for(int i = 0; i < size; ++i)
	{
		tuple.field(0).set_int(arr[i]);
		tuple.field(1).set_double(arr[i] * arr[i]);
		tuple.field(2).set_double(arr[i] * arr[i] * arr[i]);
		tree.insert_tuple(tuple);
		tree.print(std::cout);
		std::cout << '\n';
	}

	ValueKey key(tree.leaf_tuple_manipulator().field_manipulators(), list_of(0));
	key.field(0).set_int(2);
	tree.erase_tuple(key);

	tree.print(std::cout);

	key.field(0).set_int(3);
	tree.erase_tuple(key);

	tree.print(std::cout);

	key.field(0).set_int(1);
	tree.erase_tuple(key);

	tree.print(std::cout);

#if 0
	tuple.field(0).set_int(1);
	tuple.field(1).set_double(0.0);
	tuple.field(2).set_double(0.0);
	tree.insert_tuple(tuple);

	tree.print(std::cout);
#endif
}

#if 0
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

	//int arr[] = {0,1,4,2,3};
	//int arr[] = {0,1,2,3,4,5,6};
	int arr[] = {23,9,84,7,8,51,17,10,24,12,18};
	int size = sizeof(arr) / sizeof(int);
	for(int i = 0; i < size; ++i)
	{
		tuple.field(0).set_int(arr[i]);
		tuple.field(1).set_double(arr[i] * arr[i]);
		tuple.field(2).set_double(arr[i] * arr[i] * arr[i]);
		tree.insert_tuple(tuple);
		tree.print(std::cout);
		std::cout << '\n';
	}

	for(BTree::ConstIterator it = tree.begin(), iend = tree.end(); it != iend; ++it)
	{
		std::cout << it->field(0).get_int() << '\n';
	}
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(upper_bound_valuekey)
{
	BTree tree(BTreePageController_CPtr(new TestPageController));
	FreshTuple tuple(tree.leaf_tuple_manipulator());

	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			tuple.field(0).set_int(i);
			tuple.field(1).set_int(j);
			tuple.field(2).set_int(j);
			tree.insert_tuple(tuple);
		}
	}

	tree.print(std::cout);

	ValueKey key(tree.leaf_tuple_manipulator().field_manipulators(), list_of(0));
	for(int i = -1; i <= 3; ++i)
	{
		key.field(0).set_int(i);
		BTree::ConstIterator it = tree.upper_bound(key);
		if(it != tree.end())
		{
			std::cout << i << ' ' << it->field(0).get_int() << ' ' << it->field(1).get_int() << '\n';
		}
		else
		{
			std::cout << i << " End\n";
		}
	}
}
#endif

BOOST_AUTO_TEST_SUITE_END()
