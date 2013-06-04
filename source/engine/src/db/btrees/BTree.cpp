/**
 * whery: BTree.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/btrees/BTree.h"

namespace whery {

//#################### CONSTRUCTORS ####################

BTree::BTree(const BTreePageController_CPtr& pageController)
:	m_pageController(pageController)
{
	m_rootID = add_leaf_node();
}

//#################### PRIVATE METHODS ####################

int BTree::add_branch_node()
{
	int id = add_node();

	Node& n = m_nodes[id];
	n.m_isLeaf = false;
	n.m_page = m_pageController->make_btree_branch_page();

	return id;
}

int BTree::add_leaf_node()
{
	int id = add_node();

	Node& n = m_nodes[id];
	n.m_isLeaf = true;
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

}
