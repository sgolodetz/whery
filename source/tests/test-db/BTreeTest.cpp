/**
 * test-db: BTreeTest.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <boost/test/unit_test.hpp>

#include <boost/algorithm/clamp.hpp>
#include <boost/assign/list_of.hpp>
using namespace boost::assign;

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/FreshTuple.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/RangeKey.h"
#include "whery/db/btrees/BTree.h"
#include "whery/db/pages/InMemorySortedPage.h"
using namespace whery;

#include "Constants.h"

//#################### HELPER CLASSES ####################

class TestPageController : public BTreePageController
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The number of tuples that should fit on a B+-tree branch page. */
	const int m_tuplesPerBranch;

	/** The number of tuples that should fit on a B+-tree leaf page. */
	const int m_tuplesPerLeaf;

	//#################### CONSTRUCTORS ####################
public:
	TestPageController(int tuplesPerBranch, int tuplesPerLeaf)
	:	m_tuplesPerBranch(tuplesPerBranch), m_tuplesPerLeaf(tuplesPerLeaf)
	{}

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual SortedPage_Ptr make_btree_branch_page() const
	{
		TupleManipulator tupleManipulator = btree_branch_tuple_manipulator();
		return SortedPage_Ptr(new InMemorySortedPage(tupleManipulator.size() * m_tuplesPerBranch, tupleManipulator));
	}

	virtual SortedPage_Ptr make_btree_leaf_page() const
	{
		TupleManipulator tupleManipulator = btree_leaf_tuple_manipulator();
		return SortedPage_Ptr(new InMemorySortedPage(tupleManipulator.size() * m_tuplesPerLeaf, tupleManipulator));
	}
};

/**
An instance of this class provides page support to a primary B+-tree
with leaf tuples of the form <tuple ID,x,y> and branch tuples of the
form <tuple ID,child node ID>. Since all tuple IDs are unique, the
branch tuple IDs do not need to store any of the other fields.
*/
class PrimaryTestPageController : public TestPageController
{
	//#################### CONSTRUCTORS ####################
public:
	PrimaryTestPageController(int tuplesPerBranch, int tuplesPerLeaf)
	:	TestPageController(tuplesPerBranch, tuplesPerLeaf)
	{}

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual TupleManipulator btree_branch_tuple_manipulator() const
	{
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
};

/**
An instance of this class provides page support to a secondary B+-tree
with leaf tuples of the form <y,tuple ID> and branch tuples of the form
<y,tuple ID,child node ID>. The branch keys are clearly unique in this
case, since they contain the tuple ID. If y were a unique key on its own,
including the tuple ID would not be necessary, but there is no attempt
made to guarantee that for these tests.
*/
class SecondaryTestPageController : public TestPageController
{
	//#################### CONSTRUCTORS ####################
public:
	SecondaryTestPageController(int tuplesPerBranch, int tuplesPerLeaf)
	:	TestPageController(tuplesPerBranch, tuplesPerLeaf)
	{}

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual TupleManipulator btree_branch_tuple_manipulator() const
	{
		return TupleManipulator(list_of<const FieldManipulator*>
			(&DoubleFieldManipulator::instance())
			(&IntFieldManipulator::instance())
			(&IntFieldManipulator::instance())
		);
	}

	virtual TupleManipulator btree_leaf_tuple_manipulator() const
	{
		return TupleManipulator(list_of<const FieldManipulator*>
			(&DoubleFieldManipulator::instance())
			(&IntFieldManipulator::instance())
		);
	}
};

//#################### GLOBAL VARIABLES ####################

BTreePageController_CPtr primaryController_2_2(new PrimaryTestPageController(2, 2));
BTreePageController_CPtr secondaryController_2_2(new SecondaryTestPageController(2, 2));

//#################### HELPER FUNCTIONS ####################

std::pair<BTree_Ptr,BTree_Ptr> make_trees()
{
	BTree_Ptr primaryTree(new BTree(primaryController_2_2));
	BTree_Ptr secondaryTree(new BTree(secondaryController_2_2));

	FreshTuple primaryTuple(primaryTree->leaf_tuple_manipulator());
	FreshTuple secondaryTuple(secondaryTree->leaf_tuple_manipulator());

	for(int tupleID = 0; tupleID < 9; ++tupleID)
	{
		const double x = tupleID / 3;
		const double y = tupleID % 3;

		primaryTuple.field(0).set_int(tupleID);
		primaryTuple.field(1).set_double(x);
		primaryTuple.field(2).set_double(y);
		primaryTree->insert_tuple(primaryTuple);

		secondaryTuple.field(0).set_double(y);
		secondaryTuple.field(1).set_int(tupleID);
		secondaryTree->insert_tuple(secondaryTuple);
	}

	return std::make_pair(primaryTree, secondaryTree);
}

//#################### TESTS ####################

BOOST_AUTO_TEST_SUITE(BTreeTest)

BOOST_AUTO_TEST_CASE(begin_end)
{
	BTree tree(primaryController_2_2);
	BOOST_REQUIRE(tree.begin() == tree.end());

	// Insert a few tuples into the B+-tree and check begin() and end() after each insertion.
	FreshTuple tuple(tree.leaf_tuple_manipulator());
	for(int i = 0; i < 5; ++i)
	{
		tuple.field(0).set_int(i);
		tuple.field(1).set_double(i * i);
		tuple.field(2).set_double(i * i * i);
		tree.insert_tuple(tuple);
		BOOST_REQUIRE(tree.begin() != tree.end());
		BOOST_CHECK_EQUAL(tree.begin()->field(0).get_int(), 0);
		BOOST_CHECK_EQUAL((--tree.end())->field(0).get_int(), i);
	}

	// Erase the tuples again (in reverse order of insertion) and check begin() and end() after each erasure.
	ValueKey key(tree.leaf_tuple_manipulator(), list_of(0));
	for(int i = 0; i < 5; ++i)
	{
		BOOST_REQUIRE(tree.begin() != tree.end());
		BOOST_CHECK_EQUAL(tree.begin()->field(0).get_int(), i);
		BOOST_CHECK_EQUAL((--tree.end())->field(0).get_int(), 4);
		key.field(0).set_int(i);
		tree.erase_tuple(key);
	}

	BOOST_REQUIRE(tree.begin() == tree.end());
}

BOOST_AUTO_TEST_CASE(constructor)
{
	BTree tree(primaryController_2_2);
	BOOST_CHECK_EQUAL(tree.tuple_count(), 0);
}

BOOST_AUTO_TEST_CASE(equal_range_rangekey)
{
	BTree_Ptr primaryTree, secondaryTree;
	boost::tie(primaryTree, secondaryTree) = make_trees();

	// Check some ranges in the primary B+-tree.
	for(int i = -1; i <= 9; ++i)
	{
		for(int j = i; j <= 9; ++j)
		{
			RangeKey key(primaryTree->leaf_tuple_manipulator().field_manipulators(), list_of(0));
			key.low_value().field(0).set_int(i);
			key.high_value().field(0).set_int(j);

			// Check the range [i,j].
			key.low_kind() = CLOSED;
			key.high_kind() = CLOSED;
			BTree::EqualRangeResult result = primaryTree->equal_range(key);
			std::vector<BackedTuple> tuples = std::vector<BackedTuple>(result.first, result.second);

			int low = boost::algorithm::clamp(i, 0, 9);
			int high = boost::algorithm::clamp(j + 1, 0, 9);
			BOOST_CHECK_EQUAL(tuples.size(), high - low);
			if(!tuples.empty())
			{
				BOOST_CHECK_EQUAL(tuples.front().field(0).get_int(), low);
				BOOST_CHECK_EQUAL(tuples.back().field(0).get_int(), high - 1);
			}

			// Check the range [i,j).
			key.high_kind() = OPEN;
			result = primaryTree->equal_range(key);
			tuples = std::vector<BackedTuple>(result.first, result.second);

			high = boost::algorithm::clamp(j, 0, 9);
			BOOST_CHECK_EQUAL(tuples.size(), high - low);
			if(!tuples.empty())
			{
				BOOST_CHECK_EQUAL(tuples.front().field(0).get_int(), low);
				BOOST_CHECK_EQUAL(tuples.back().field(0).get_int(), high - 1);
			}

			// Check the range (i,j).
			key.low_kind() = OPEN;
			result = primaryTree->equal_range(key);
			tuples = std::vector<BackedTuple>(result.first, result.second);

			low = boost::algorithm::clamp(i + 1, 0, 9);
			if(i == j) high = low;
			BOOST_CHECK_EQUAL(tuples.size(), high - low);
			if(!tuples.empty())
			{
				BOOST_CHECK_EQUAL(tuples.front().field(0).get_int(), low);
				BOOST_CHECK_EQUAL(tuples.back().field(0).get_int(), high - 1);
			}

			// Check the range (i,j].
			key.high_kind() = CLOSED;
			result = primaryTree->equal_range(key);
			tuples = std::vector<BackedTuple>(result.first, result.second);

			high = boost::algorithm::clamp(j + 1, 0, 9);
			BOOST_CHECK_EQUAL(tuples.size(), high - low);
			if(!tuples.empty())
			{
				BOOST_CHECK_EQUAL(tuples.front().field(0).get_int(), low);
				BOOST_CHECK_EQUAL(tuples.back().field(0).get_int(), high - 1);
			}
		}
	}

	// TODO: Check some ranges in the secondary B+-tree.
}

BOOST_AUTO_TEST_CASE(equal_range_valuekey)
{
	BTree_Ptr primaryTree, secondaryTree;
	boost::tie(primaryTree, secondaryTree) = make_trees();

	// Check that the primary B+-tree contains a single tuple for each tuple ID in [0,8], and no tuple for -1 or 9.
	for(int i = -1; i <= 9; ++i)
	{
		ValueKey key(primaryTree->leaf_tuple_manipulator(), list_of(0));
		key.field(0).set_int(i);
		BTree::EqualRangeResult result = primaryTree->equal_range(key);
		std::vector<BackedTuple> tuples = std::vector<BackedTuple>(result.first, result.second);
		BOOST_CHECK_EQUAL(tuples.size(), i >= 0 && i <= 8 ? 1 : 0);

		if(!tuples.empty())
		{
			BOOST_CHECK_EQUAL(tuples[0].field(0).get_int(), i);
		}
	}

	// Check that the secondary B+-tree contains three tuples for each y in [0,2], and no tuple for -1 or 3.
	for(int i = -1; i <= 3; ++i)
	{
		ValueKey key(secondaryTree->leaf_tuple_manipulator(), list_of(0));
		key.field(0).set_double(i);
		BTree::EqualRangeResult result = secondaryTree->equal_range(key);
		std::vector<BackedTuple> tuples = std::vector<BackedTuple>(result.first, result.second);
		BOOST_CHECK_EQUAL(tuples.size(), i >= 0 && i <= 2 ? 3 : 0);

		for(unsigned int j = 0, size = tuples.size(); j < size; ++j)
		{
			BOOST_CHECK_CLOSE(tuples[j].field(0).get_double(), i, Constants::SMALL_EPSILON);
			BOOST_CHECK_EQUAL(tuples[j].field(1).get_int(), i + j * 3);
		}
	}
}

BOOST_AUTO_TEST_CASE(insert_erase)
{
	BTree tree(primaryController_2_2);

	// Insert tuples in an order designed to exercise the various different insert cases.
	// Check that the set of tuples is as expected after each insertion.
	int insertArray[] = {3,4,1,2,6,5,7,8,9,10,11};
	int insertSize = sizeof(insertArray) / sizeof(int);

	FreshTuple tuple(tree.leaf_tuple_manipulator());
	for(int i = 0; i < insertSize; ++i)
	{
		const int x = insertArray[i];
		tuple.field(0).set_int(x);
		tuple.field(1).set_double(x * x);
		tuple.field(2).set_double(x * x * x);
		tree.insert_tuple(tuple);

		std::vector<int> sortedInsertArray(insertArray, insertArray + i + 1);
		std::sort(sortedInsertArray.begin(), sortedInsertArray.end());

		int k = 0;
		for(BTree::ConstIterator jt = tree.begin(), jend = tree.end(); jt != jend; ++jt)
		{
			BOOST_CHECK_EQUAL(jt->field(0).get_int(), sortedInsertArray[k]);
			++k;
		}
	}

	// Erase tuples in an order designed to exercise the various different erase cases.
	// Check that the set of tuples is as expected after each erasure.
	// TODO
}

#if 0
BOOST_AUTO_TEST_CASE(erase_tuple)
{
	BTree tree(primaryController_2_2);
	FreshTuple tuple(tree.leaf_tuple_manipulator());

	//int arr[] = {0,1,2,4,5,3};
	int arr[] = {0,1,2,4,5,3,7,8,6,9};
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

	int darr[] = {7,8,0,1,2};
	size = sizeof(darr) / sizeof(int);
	ValueKey key(tree.leaf_tuple_manipulator(), list_of(0));
	for(int i = 0; i < size; ++i)
	{
		key.field(0).set_int(darr[i]);
		tree.erase_tuple(key);
		tree.print(std::cout);
		std::cout << '\n';
	}

#if 0
	key.field(0).set_int(2);
	tree.erase_tuple(key);

	tree.print(std::cout);
	std::cout << '\n';

	key.field(0).set_int(3);
	tree.erase_tuple(key);

	tree.print(std::cout);
	std::cout << '\n';

	key.field(0).set_int(1);
	tree.erase_tuple(key);

	tree.print(std::cout);
	std::cout << '\n';

	key.field(0).set_int(0);
	tree.erase_tuple(key);

	tree.print(std::cout);
	std::cout << '\n';

	key.field(0).set_int(4);
	tree.erase_tuple(key);

	tree.print(std::cout);
	std::cout << '\n';

	key.field(0).set_int(5);
	tree.erase_tuple(key);

	tree.print(std::cout);
	std::cout << '\n';
#endif

#if 0
	tuple.field(0).set_int(1);
	tuple.field(1).set_double(0.0);
	tuple.field(2).set_double(0.0);
	tree.insert_tuple(tuple);

	tree.print(std::cout);
#endif
}
#endif

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

	ValueKey key(tree.leaf_tuple_manipulator(), list_of(0));
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
