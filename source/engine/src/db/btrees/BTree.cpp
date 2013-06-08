/**
 * whery: BTree.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/btrees/BTree.h"

#include <cassert>

#include <boost/lexical_cast.hpp>

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
	InsertResult result = insert_tuple_sub(tuple, m_rootID);
	assert(result.m_leftChildID == -1);
	++m_tupleCount;
}

TupleManipulator BTree::leaf_tuple_manipulator() const
{
	return m_pageController->btree_leaf_tuple_manipulator();
}

void BTree::print(std::ostream& os) const
{
	print_sub(os, m_rootID, 0);
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

void BTree::add_root_node(const InsertResult& insertResult)
{
	m_rootID = add_branch_node();
	m_nodes[insertResult.m_leftChildID].m_parentID = m_rootID;
	m_nodes[insertResult.m_rightChildID].m_parentID = m_rootID;
	m_nodes[m_rootID].m_firstChildID = insertResult.m_leftChildID;
	m_nodes[m_rootID].m_page->add_tuple(make_branch_tuple(*insertResult.m_splitter, insertResult.m_rightChildID));
}

int BTree::child_node_id(const BackedTuple& branchTuple) const
{
	int id = branchTuple.field(branchTuple.arity() - 1).get_int();
	assert(0 <= id && static_cast<unsigned int>(id) < m_nodes.size() && m_nodes[id].m_page.get() != NULL);
	return id;
}

void BTree::insert_node_as_right_sibling_of(int nodeID, int freshID)
{
	m_nodes[freshID].m_parentID = m_nodes[nodeID].m_parentID;
	m_nodes[freshID].m_siblingLeftID = nodeID;
	m_nodes[freshID].m_siblingRightID = m_nodes[nodeID].m_siblingRightID;
	m_nodes[nodeID].m_siblingRightID = freshID;
	if(m_nodes[freshID].m_siblingRightID != -1)
	{
		m_nodes[m_nodes[freshID].m_siblingRightID].m_siblingLeftID = freshID;
	}
}

BTree::InsertResult BTree::insert_tuple_branch(const Tuple& tuple, int nodeID)
{
	// Find the child of this node below which the specified tuple should be inserted,
	// and insert the tuple into the subtree below it.
	ValueKey key = make_branch_key(tuple);
	SortedPage::TupleSetCIter it = m_nodes[nodeID].m_page->upper_bound(key);
	int childID;
	if(it == page_begin(nodeID))
	{
		childID = m_nodes[nodeID].m_firstChildID;
	}
	else
	{
		--it;
		childID = child_node_id(*it);
	}
	InsertResult subResult = insert_tuple_sub(tuple, childID);

	if(subResult.m_leftChildID == -1)
	{
		// The insertion succeeded without needing to split the direct child of this node.
		return subResult;
	}
	else if(m_nodes[nodeID].m_page->empty_tuple_count() > 0)
	{
		// A child of this node was split, and there's space in this node, so
		// insert an index entry for the right-hand node returned by the split.
		m_nodes[nodeID].m_page->add_tuple(make_branch_tuple(*subResult.m_splitter, subResult.m_rightChildID));

		return InsertResult();
	}
	else
	{
		// The child of this node was split, but this node is full, so split it into two nodes
		// and update appropriately. (Don't forget to handle the root case.)

		// TODO: This is very similar to the code in insert_tuple_leaf - factor out the commonality.

		// Step 1:	Create a fresh branch node and connect it to the rest of the tree.
		int freshID = add_branch_node();
		insert_node_as_right_sibling_of(nodeID, freshID);

		// Step 2:	Make a tuple set containing fresh copies of all the tuples in the original page.
		SortedPage_Ptr nodePage = m_nodes[nodeID].m_page, freshPage = m_nodes[freshID].m_page;
		std::multiset<FreshTuple,PrefixTupleComparator> tuples;
		for(SortedPage::TupleSetCIter it = nodePage->begin(), iend = nodePage->end(); it != iend; ++it)
		{
			FreshTuple temp(branch_tuple_manipulator());
			temp.copy_from(*it);
			tuples.insert(temp);
		}

		// Step 3:	Add the splitter from the sub-split to this set.
		tuples.insert(make_branch_tuple(*subResult.m_splitter, subResult.m_rightChildID));

		// Step 4:	Clear the original page and copy the first half of the tuples across to it.
		nodePage->clear();
		unsigned int count = 0, size = tuples.size();
		std::multiset<FreshTuple,PrefixTupleComparator>::const_iterator it = tuples.begin(), iend = tuples.end();
		for(; count < size / 2; ++it, ++count)
		{
			nodePage->add_tuple(*it);
		}

		// Step 5:	Record the median as the new splitter.
		boost::shared_ptr<FreshTuple> splitter(new FreshTuple(branch_tuple_manipulator()));
		splitter->copy_from(*it);
		++it;

		// Step 6:	Copy the second half of the tuples across to the fresh page.
		for(; it != iend; ++it)
		{
			freshPage->add_tuple(*it);
		}

		// Step 7:	Set the first child of the fresh node to the child node ID of the splitter.
		m_nodes[freshID].m_firstChildID = child_node_id(*splitter);

		// Step 8:	Update the parent pointers of all children of the fresh page.
		m_nodes[m_nodes[freshID].m_firstChildID].m_parentID = freshID;
		for(SortedPage::TupleSetCIter jt = page_begin(freshID), jend = page_end(freshID); jt != jend; ++jt)
		{
			m_nodes[child_node_id(*jt)].m_parentID = freshID;
		}

		// Step 3:	Construct a triple indicating the result of the split.
		InsertResult result(nodeID, freshID, splitter);

		// Step 4:	If the node that was split is the root, create a new root node.
		//			If not, return the result as-is.
		if(nodeID == m_rootID)
		{
			add_root_node(result);
			return InsertResult();
		}
		else return result;
	}
}

BTree::InsertResult BTree::insert_tuple_leaf(const Tuple& tuple, int nodeID)
{
	if(m_nodes[nodeID].m_page->empty_tuple_count() > 0)
	{
		// This node is a leaf and has spare capacity, so simply insert the tuple into it.
		m_nodes[nodeID].m_page->add_tuple(tuple);
		return InsertResult();
	}
	else if(false)
	{
		// This node is full, but one of its siblings has the same parent and spare capacity,
		// so we can redistribute tuples and avoid the need for a split.
		// TODO (Optional)
	}
	else
	{
		// This node is full and redistribution is not possible, so split it into two nodes
		// and insert the tuple into the appropriate one of them.

		// Step 1:	Create a fresh leaf node and connect it to the rest of the tree.
		int freshID = add_leaf_node();
		insert_node_as_right_sibling_of(nodeID, freshID);

		// Step 2:	Transfer half of the tuples across to the fresh node.
		SortedPage_Ptr nodePage = m_nodes[nodeID].m_page, freshPage = m_nodes[freshID].m_page;
		transfer_leaf_tuples_right(nodeID, nodePage->tuple_count() / 2);

		// Step 3:	Insert the original tuple into the appropriate node.
		selectively_insert_tuple(tuple, nodeID, freshID);

		// Step 4:	Construct a triple indicating the result of the split.
		boost::shared_ptr<FreshTuple> splitter(new FreshTuple(leaf_tuple_manipulator()));
		splitter->copy_from(*freshPage->begin());
		InsertResult result(nodeID, freshID, splitter);

		// Step 5:	If the node that was split is the root, create a new root node.
		//			If not, return the result as-is.
		if(nodeID == m_rootID)
		{
			add_root_node(result);
			return InsertResult();
		}
		else return result;
	}
}

BTree::InsertResult BTree::insert_tuple_sub(const Tuple& tuple, int nodeID)
{
	if(m_nodes[nodeID].has_children())
		return insert_tuple_branch(tuple, nodeID);
	else
		return insert_tuple_leaf(tuple, nodeID);
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

SortedPage_Ptr BTree::page(int nodeID) const
{
	return m_nodes[nodeID].m_page;
}

SortedPage::TupleSetCIter BTree::page_begin(int nodeID) const
{
	SortedPage_Ptr p = page(nodeID);
	assert(p.get() != NULL);
	return p->begin();
}

SortedPage::TupleSetCIter BTree::page_end(int nodeID) const
{
	SortedPage_Ptr p = page(nodeID);
	assert(p.get() != NULL);
	return p->end();
}

SortedPage::TupleSetCRIter BTree::page_rbegin(int nodeID) const
{
	SortedPage_Ptr p = page(nodeID);
	assert(p.get() != NULL);
	return p->rbegin();
}

void BTree::print_sub(std::ostream& os, int nodeID, unsigned int depth) const
{
	// Print the basic details of the node.
	write_tabbed_text(os, depth, "Node: " + boost::lexical_cast<std::string>(nodeID));
	write_tabbed_text(os, depth, "Parent: " + boost::lexical_cast<std::string>(m_nodes[nodeID].m_parentID));
	write_tabbed_text(os, depth, "Left Sibling: " + boost::lexical_cast<std::string>(m_nodes[nodeID].m_siblingLeftID));
	write_tabbed_text(os, depth, "Right Sibling: " + boost::lexical_cast<std::string>(m_nodes[nodeID].m_siblingRightID));

	// Print the tuples held by the node.
	for(SortedPage::TupleSetCIter it = page_begin(nodeID), iend = page_end(nodeID); it != iend; ++it)
	{
		std::stringstream ss;
		ss << "Tuple(";
		for(unsigned int j = 0, arity = it->arity(); j < arity; ++j)
		{
			ss << it->field(j).get_string();
			if(j != arity - 1) ss << ',';
		}
		ss << ')';
		write_tabbed_text(os, depth, ss.str());
	}

	// Recursively print the children of the node (if any).
	if(m_nodes[nodeID].m_firstChildID != -1)
	{
		print_sub(os, m_nodes[nodeID].m_firstChildID, depth + 1);
		for(SortedPage::TupleSetCIter it = page_begin(nodeID), iend = page_end(nodeID); it != iend; ++it)
		{
			print_sub(os, child_node_id(*it), depth + 1);
		}
	}
}

void BTree::selectively_insert_tuple(const Tuple& tuple, int leftNodeID, int rightNodeID)
{
	SortedPage_Ptr leftPage = page(leftNodeID), rightPage = page(rightNodeID);

	// Check the precondition.
	if(leftPage->empty_tuple_count() == 0 || rightPage->empty_tuple_count() == 0)
	{
		throw std::invalid_argument("Cannot selectively insert a tuple unless both potential pages have space.");
	}

	// Compare the tuple to be inserted against the first tuple on the right-hand page.
	// If it's strictly before that tuple in the ordering, insert it into the left-hand
	// page; if not, insert it into the right-hand page.
	PrefixTupleComparator comp;
	if(comp.compare(tuple, *rightPage->begin()) == -1)
	{
		leftPage->add_tuple(tuple);
	}
	else
	{
		rightPage->add_tuple(tuple);
	}
}

void BTree::transfer_leaf_tuples_right(int sourceNodeID, unsigned int n)
{
	// Check the preconditions.
	int targetNodeID = m_nodes[sourceNodeID].m_siblingRightID;
	if(m_nodes[targetNodeID].m_parentID != m_nodes[sourceNodeID].m_parentID)
	{
		throw std::invalid_argument("Cannot transfer tuples to a page with a different parent.");
	}

	SortedPage_Ptr targetPage = page(targetNodeID);
	if(targetPage->empty_tuple_count() < n)
	{
		throw std::invalid_argument("Cannot transfer tuples to a page with insufficient space to hold them.");
	}

	// Transfer n tuples from the source page to the target page. Note that copying the tuples
	// into a separate container is needed to prevent a "modification during iteration" issue.
	SortedPage_Ptr sourcePage = page(sourceNodeID);
	std::vector<BackedTuple> ts(sourcePage->rbegin(), sourcePage->rend());
	unsigned int i = 0;
	for(std::vector<BackedTuple>::const_iterator it = ts.begin(), iend = ts.end(); it != iend && i < n; ++it, ++i)
	{
		targetPage->add_tuple(*it);
		sourcePage->delete_tuple(*it);
	}
}

void BTree::write_tabbed_text(std::ostream& os, unsigned int tabCount, const std::string& text) const
{
	for(unsigned int i = 0; i < tabCount; ++i)
	{
		os << '\t';
	}
	os << text << '\n';
}

}
