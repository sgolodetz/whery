/**
 * whery: BTree.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/btrees/BTree.h"

#include <cassert>

#include "whery/db/base/ValueKey.h"

namespace whery {

//#################### CONSTRUCTORS ####################

BTree::BTree(const BTreePageController_CPtr& pageController)
:	m_pageController(pageController), m_tupleCount(0)
{
	m_rootID = add_leaf_node();
}

//#################### PUBLIC METHODS ####################

BTree::ConstIterator BTree::begin() const
{
	int id = m_rootID;

	// Walk down the left side of the tree until we hit the left-most leaf.
	while(m_nodes[id].has_children())
	{
		id = m_nodes[id].m_firstChildID;
	}

	// Return an iterator pointing to the start of the set of tuples on the leaf's page.
	return ConstIterator(this, id, page_begin(id));
}

TupleManipulator BTree::branch_tuple_manipulator() const
{
	return m_pageController->btree_branch_tuple_manipulator();
}

BTree::ConstIterator BTree::end() const
{
	int id = m_rootID;

	// Walk down the right side of the tree until we hit the right-most leaf.
	while(m_nodes[id].has_children())
	{
		id = child_node_id(*page_rbegin(id));
	}

	// Return an iterator pointing to the end of the set of tuples on the leaf's page.
	return ConstIterator(this, id, page_end(id));
}

void BTree::insert_tuple(const Tuple& tuple)
{
	std::pair<int,int> result = insert_tuple_sub(tuple, m_rootID);
	assert(result.first == -1 && result.second == -1);
	++m_tupleCount;
}

TupleManipulator BTree::leaf_tuple_manipulator() const
{
	return m_pageController->btree_leaf_tuple_manipulator();
}

unsigned int BTree::tuple_count()
{
	return m_tupleCount;
}

//#################### PRIVATE METHODS ####################

int BTree::add_branch_node()
{
	int id = add_node();

	Node& n = m_nodes[id];
	n.m_page = m_pageController->make_btree_branch_page();

	return id;
}

int BTree::add_leaf_node()
{
	int id = add_node();

	Node& n = m_nodes[id];
	n.m_page = m_pageController->make_btree_leaf_page();

	return id;
}

int BTree::add_node()
{
	int id = m_nodeIDAllocator.allocate();

	if(static_cast<unsigned int>(id) >= m_nodes.size())
	{
		m_nodes.resize(id + 1);
	}

	Node& n = m_nodes[id];
	n.m_parentID = n.m_siblingLeftID = n.m_siblingRightID = -1;

	return id;
}

void BTree::add_root_node(int leftChildID, int rightChildID)
{
	m_rootID = add_branch_node();
	m_nodes[m_rootID].m_firstChildID = leftChildID;
	m_nodes[m_rootID].m_page->add_tuple(make_branch_tuple(*page_begin(rightChildID), rightChildID));
}

int BTree::child_node_id(const BackedTuple& branchTuple) const
{
	int id = branchTuple.field(branchTuple.arity() - 1).get_int();
	assert(0 <= id && static_cast<unsigned int>(id) < m_nodes.size() && m_nodes[id].m_page.get() != NULL);
	return id;
}

int BTree::find_child(int nodeID, const Tuple& tuple) const
{
	ValueKey key = make_branch_key(tuple);
	SortedPage::TupleSetCIter it = m_nodes[nodeID].m_page->upper_bound(key);

	if(it == page_begin(nodeID))
	{
		return m_nodes[nodeID].m_firstChildID;
	}
	else
	{
		--it;
		return child_node_id(*it);
	}
}

std::pair<int,int> BTree::insert_tuple_sub(const Tuple& tuple, int nodeID)
{
	if(m_nodes[nodeID].has_children())
	{
		// Find the subtree below which the specified tuple should be located.
		int childID = find_child(nodeID, tuple);

		// Insert the tuple into this subtree.
		std::pair<int,int> result = insert_tuple_sub(tuple, childID);

		// If the insertion succeeded without needing to split the direct child
		// of this node, early out.
		if(result.first == -1) return result;

		// Otherwise:

		// Option 1:	The branch node has spare capacity, so update it to record
		//				the split of its child.
		if(m_nodes[nodeID].m_page->empty_tuple_count() > 0)
		{
			// TODO
			throw 23;
		}

		// Option 2:	The branch node is full, so split it into two nodes and update appropriately.
		// TODO

		// TODO
		throw 23;
	}
	else
	{
		// Option 1:	We have reached a leaf node with spare capacity, so simply insert the tuple into it.
		if(m_nodes[nodeID].m_page->empty_tuple_count() > 0)
		{
			m_nodes[nodeID].m_page->add_tuple(tuple);
			return std::make_pair(-1, -1);
		}

		// Option 2:	The leaf node is full, but one of its siblings has the same parent and spare capacity,
		//				so we can redistribute tuples and avoid the need for a split.
		// TODO (Optional)

		// Option 3:	The leaf node is full and redistribution is not possible, so split it
		//				into two nodes and insert the tuple into the appropriate one of them.

		// Step 1:	Create a fresh leaf node and connect it to the rest of the tree.
		int freshID = add_leaf_node();
		m_nodes[freshID].m_parentID = m_nodes[nodeID].m_parentID;
		m_nodes[freshID].m_siblingLeftID = nodeID;
		m_nodes[freshID].m_siblingRightID = m_nodes[nodeID].m_siblingRightID;
		m_nodes[nodeID].m_siblingRightID = freshID;

		// Step 2:	Starting from the last tuple in the original node's tuple set,
		//			transfer tuples across to the fresh node until the point at
		//			which it would have more tuples than the existing node.
		SortedPage_Ptr nodePage = m_nodes[nodeID].m_page, freshPage = m_nodes[freshID].m_page;
		nodePage->transfer_high_tuples(*freshPage, nodePage->tuple_count() / 2);

		// Step 3:	Insert the tuple into one of the two nodes, based on a comparison
		//			against the first tuple in the new node.
		PrefixTupleComparator comp;
		if(comp.compare(tuple, *freshPage->begin()) == -1) nodePage->add_tuple(tuple);
		else freshPage->add_tuple(tuple);

		// Step 4:	Construct a pair indicating the result of the split.
		std::pair<int,int> result = std::make_pair(nodeID, freshID);

		// Step 5:	If the node that was split is the root, create a new root node.
		//			If not, return the result as-is.
		if(nodeID == m_rootID)
		{
			add_root_node(nodeID, freshID);
			return std::make_pair(-1, -1);
		}
		else return result;
	}
}

ValueKey BTree::make_branch_key(const Tuple& tuple) const
{
	TupleManipulator branchTupleManipulator = branch_tuple_manipulator();
	unsigned int arity = branchTupleManipulator.arity() - 1;

	std::vector<unsigned int> fieldIndices;
	fieldIndices.reserve(arity);
	for(unsigned int i = 0; i < arity; ++i) fieldIndices.push_back(i);

	ValueKey result(branchTupleManipulator.field_manipulators(), fieldIndices);
	for(unsigned int i = 0; i < arity; ++i)
	{
		result.field(i).set_from(tuple.field(i));
	}
	return result;
}

FreshTuple BTree::make_branch_tuple(const Tuple& tuple, int nodeID) const
{
	FreshTuple result(branch_tuple_manipulator());
	for(unsigned int i = 0; i < result.arity() - 1; ++i)
	{
		result.field(i).set_from(tuple.field(i));
	}
	result.field(result.arity() - 1).set_int(nodeID);
	return result;
}

SortedPage::TupleSetCIter BTree::page_begin(int nodeID) const
{
	SortedPage_Ptr page = m_nodes[nodeID].m_page;
	assert(page.get() != NULL);
	return page->begin();
}

SortedPage::TupleSetCIter BTree::page_end(int nodeID) const
{
	SortedPage_Ptr page = m_nodes[nodeID].m_page;
	assert(page.get() != NULL);
	return page->end();
}

SortedPage::TupleSetCRIter BTree::page_rbegin(int nodeID) const
{
	SortedPage_Ptr page = m_nodes[nodeID].m_page;
	assert(page.get() != NULL);
	return page->rbegin();
}

}
