/**
 * whery: BTree.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/btrees/BTree.h"

#include <cassert>

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
		id = child_node_id(*page_begin(id));
	}

	// Return an iterator pointing to the start of the set of tuples on the leaf's page.
	return ConstIterator(this, id, page_begin(id));
}

BTree::ConstIterator BTree::end() const
{
	int id = m_rootID;

	// Walk down the right side of the tree until we hit the right-most leaf.
	while(m_nodes[id].has_children())
	{
		id = m_nodes[id].m_lastChildID;
	}

	// Return an iterator pointing to the end of the set of tuples on the leaf's page.
	return ConstIterator(this, id, page_end(id));
}

void BTree::insert_tuple(const Tuple& tuple)
{
	insert_tuple_sub(tuple, m_rootID);
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

int BTree::child_node_id(const BackedTuple& branchTuple) const
{
	int id = branchTuple.field(branchTuple.arity() - 1).get_int();
	assert(0 <= id && static_cast<unsigned int>(id) < m_nodes.size() && m_nodes[id].m_page.get() != NULL);
	return id;
}

int BTree::insert_tuple_sub(const Tuple& tuple, int nodeID)
{
	Node& n = m_nodes[nodeID];
	if(n.has_children())
	{
		// TODO
		throw 23;
	}
	else
	{
		if(n.m_page->empty_tuple_count() > 0)
		{
			// If we reach a leaf page with spare capacity, simply insert the tuple into it.
			n.m_page->add_tuple(tuple);
			return -1;
		}
		else
		{
			// TODO
			throw 23;
		}
	}
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

}
