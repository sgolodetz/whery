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
	// Create the root node.
	m_rootID = add_leaf_node();

	// Pre-calculate the field indices to use when constructing branch keys.
	TupleManipulator branchTupleManipulator = branch_tuple_manipulator();
	unsigned int branchKeyArity = branchTupleManipulator.arity() - 1;
	m_branchKeyFieldIndices.reserve(branchKeyArity);
	for(unsigned int i = 0; i < branchKeyArity; ++i)
	{
		m_branchKeyFieldIndices.push_back(i);
	}
}

//#################### PUBLIC METHODS ####################

BTree::ConstIterator BTree::begin() const
{
	int id = m_rootID;

	// Walk down the left side of the tree until we hit the left-most leaf.
	while(m_nodes[id].has_children())
	{
		id = m_nodes[id].firstChildID;
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
	OptionalSplit result = insert_tuple_into_subtree(tuple, m_rootID);
	assert(!result);
	++m_tupleCount;
}

TupleManipulator BTree::leaf_tuple_manipulator() const
{
	return m_pageController->btree_leaf_tuple_manipulator();
}

void BTree::print(std::ostream& os) const
{
	print_subtree(os, m_rootID, 0);
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
	n.page = m_pageController->make_btree_branch_page();

	return id;
}

int BTree::add_leaf_node()
{
	int id = add_node();

	Node& n = m_nodes[id];
	n.page = m_pageController->make_btree_leaf_page();

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
	n.parentID = n.siblingLeftID = n.siblingRightID = -1;

	return id;
}

BTree::OptionalSplit BTree::add_root_node(const Split& split)
{
	m_rootID = add_branch_node();
	m_nodes[split.leftNodeID].parentID = m_rootID;
	m_nodes[split.rightNodeID].parentID = m_rootID;
	m_nodes[m_rootID].firstChildID = split.leftNodeID;
	m_nodes[m_rootID].page->add_tuple(make_branch_tuple(split.splitter, split.rightNodeID));
	return boost::none;
}

int BTree::child_node_id(const BackedTuple& branchTuple) const
{
	int id = branchTuple.field(branchTuple.arity() - 1).get_int();
	assert(0 <= id && static_cast<unsigned int>(id) < m_nodes.size() && page(id).get() != NULL);
	return id;
}

void BTree::insert_node_as_right_sibling_of(int nodeID, int freshID)
{
	m_nodes[freshID].parentID = m_nodes[nodeID].parentID;
	m_nodes[freshID].siblingLeftID = nodeID;
	m_nodes[freshID].siblingRightID = m_nodes[nodeID].siblingRightID;
	m_nodes[nodeID].siblingRightID = freshID;
	if(m_nodes[freshID].siblingRightID != -1)
	{
		m_nodes[m_nodes[freshID].siblingRightID].siblingLeftID = freshID;
	}
}

int BTree::find_child(const Tuple& leafTuple, int branchNodeID) const
{
	ValueKey key = make_branch_key(leafTuple);
	SortedPage::TupleSetCIter it = m_nodes[branchNodeID].page->upper_bound(key);

	int result;
	if(it == page_begin(branchNodeID))
	{
		// If the key is less than the first key in the branch node,
		// then the leaf tuple might be within the first child node.
		result = m_nodes[branchNodeID].firstChildID;
	}
	else
	{
		// Otherwise, return the child node ID from the relevant index entry.
		--it;
		result = child_node_id(*it);
	}

	return result;
}

BTree::OptionalSplit BTree::insert_tuple_into_branch(const Tuple& tuple, int nodeID)
{
	// Find the child of this node below which the specified tuple should be inserted,
	// and insert the tuple into the subtree below it.
	int childNodeID = find_child(tuple, nodeID);
	OptionalSplit result = insert_tuple_into_subtree(tuple, childNodeID);

	if(!result)
	{
		// The insertion succeeded without needing to split the direct child of this node.
		return result;
	}
	else if(m_nodes[nodeID].page->empty_tuple_count() > 0)
	{
		// The child of this node was split, and there's space in this node, so
		// insert an index entry for the right-hand node returned by the split.
		m_nodes[nodeID].page->add_tuple(make_branch_tuple(result->splitter, result->rightNodeID));
		return boost::none;
	}
	else
	{
		// The child of this node was split, but this node is full, so split it into two nodes,
		// inserting the new tuple and then pushing the median tuple upwards. If the node being
		// split is also the root node, add a new root above both it and the fresh node.
		Split split = split_branch_and_insert(nodeID, make_branch_tuple(result->splitter, result->rightNodeID));
		return nodeID == m_rootID ? add_root_node(split) : split;
	}
}

BTree::OptionalSplit BTree::insert_tuple_into_leaf(const Tuple& tuple, int nodeID)
{
	if(m_nodes[nodeID].page->empty_tuple_count() > 0)
	{
		// This node is a leaf and has spare capacity, so simply insert the tuple into it.
		m_nodes[nodeID].page->add_tuple(tuple);
		return boost::none;
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
		// and insert the tuple into the appropriate one of them. If the node being split
		// is also the root node, add a new root above both it and the fresh node.
		Split split = split_leaf_and_insert(nodeID, tuple);
		return nodeID == m_rootID ? add_root_node(split) : split;
	}
}

BTree::OptionalSplit BTree::insert_tuple_into_subtree(const Tuple& tuple, int nodeID)
{
	if(m_nodes[nodeID].has_children())
	{
		return insert_tuple_into_branch(tuple, nodeID);
	}
	else
	{
		return insert_tuple_into_leaf(tuple, nodeID);
	}
}

ValueKey BTree::make_branch_key(const Tuple& sourceTuple) const
{
	TupleManipulator branchTupleManipulator = branch_tuple_manipulator();
	unsigned int branchKeyArity = m_branchKeyFieldIndices.size();
	ValueKey result(branchTupleManipulator.field_manipulators(), m_branchKeyFieldIndices);
	for(unsigned int i = 0; i < branchKeyArity; ++i)
	{
		result.field(i).set_from(sourceTuple.field(i));
	}
	return result;
}

FreshTuple BTree::make_branch_tuple(const Tuple& sourceTuple, int childNodeID) const
{
	// A branch tuple has fields of the same type as some of the initial fields of a leaf tuple, plus a
	// child node ID (of type int). For example, a B+-tree with leaf tuples of type <int,double,double>
	// might have branch tuples of type <int,int>.
	FreshTuple result(branch_tuple_manipulator());

	// Copy fields across from the source tuple to fill up all but one of the fields of the branch tuple.
	// Note that not every field of the source tuple has to be used, but that the source tuple must have
	// at least the required number of fields.
	for(unsigned int i = 0; i < result.arity() - 1; ++i)
	{
		result.field(i).set_from(sourceTuple.field(i));
	}

	// Set the last field of the branch tuple to the child node ID.
	result.field(result.arity() - 1).set_int(childNodeID);

	return result;
}

SortedPage_Ptr BTree::page(int nodeID) const
{
	return m_nodes[nodeID].page;
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

void BTree::print_subtree(std::ostream& os, int nodeID, unsigned int depth) const
{
	// Print the basic details of the node.
	write_tabbed_text(os, depth, "Node: " + boost::lexical_cast<std::string>(nodeID));
	write_tabbed_text(os, depth, "Parent: " + boost::lexical_cast<std::string>(m_nodes[nodeID].parentID));
	write_tabbed_text(os, depth, "Left Sibling: " + boost::lexical_cast<std::string>(m_nodes[nodeID].siblingLeftID));
	write_tabbed_text(os, depth, "Right Sibling: " + boost::lexical_cast<std::string>(m_nodes[nodeID].siblingRightID));

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
	if(m_nodes[nodeID].has_children())
	{
		print_subtree(os, m_nodes[nodeID].firstChildID, depth + 1);
		for(SortedPage::TupleSetCIter it = page_begin(nodeID), iend = page_end(nodeID); it != iend; ++it)
		{
			print_subtree(os, child_node_id(*it), depth + 1);
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

BTree::Split BTree::split_leaf_and_insert(int nodeID, const Tuple& tuple)
{
	// Create a fresh leaf node and connect it to the rest of the tree.
	int freshID = add_leaf_node();
	insert_node_as_right_sibling_of(nodeID, freshID);

	// Transfer half of the tuples across to the fresh node.
	SortedPage_Ptr nodePage = m_nodes[nodeID].page, freshPage = m_nodes[freshID].page;
	transfer_leaf_tuples_right(nodeID, nodePage->tuple_count() / 2);

	// Insert the original tuple into the appropriate node.
	selectively_insert_tuple(tuple, nodeID, freshID);

	// Construct and return the split result.
	Split split(nodeID, freshID, FreshTuple(leaf_tuple_manipulator()));
	split.splitter.copy_from(*freshPage->begin());
	return split;
}

BTree::Split BTree::split_branch_and_insert(int nodeID, const FreshTuple& tuple)
{
	// Check the precondition.
	SortedPage_Ptr nodePage = m_nodes[nodeID].page;
	if(nodePage->empty_tuple_count() > 0)
	{
		throw std::invalid_argument("Cannot split a non-full branch node.");
	}

	// Create a fresh branch node and connect it to the rest of the tree.
	int freshID = add_branch_node();
	insert_node_as_right_sibling_of(nodeID, freshID);

	// Make a tuple set containing fresh copies of all the tuples in the original page.
	typedef std::multiset<FreshTuple,PrefixTupleComparator> FreshTupleSet;
	FreshTupleSet tuples;
	for(SortedPage::TupleSetCIter it = nodePage->begin(), iend = nodePage->end(); it != iend; ++it)
	{
		FreshTuple temp(branch_tuple_manipulator());
		temp.copy_from(*it);
		tuples.insert(temp);
	}

	// Add the tuple to be inserted to the set.
	tuples.insert(tuple);

	// Clear the original page and copy the first half of the tuple set across to it.
	nodePage->clear();
	FreshTupleSet::const_iterator it = tuples.begin(), iend = tuples.end();
	for(unsigned int i = 0, size = tuples.size(); i < size / 2; ++it, ++i)
	{
		nodePage->add_tuple(*it);
	}

	// Record the median as the splitter.
	FreshTuple splitter(branch_tuple_manipulator());
	splitter.copy_from(*it);
	++it;

	// Copy the second half of the tuple set across to the fresh page.
	SortedPage_Ptr freshPage = m_nodes[freshID].page;
	for(; it != iend; ++it)
	{
		freshPage->add_tuple(*it);
	}

	// Set the first child of the fresh node to be the child pointed to by the splitter.
	m_nodes[freshID].firstChildID = child_node_id(splitter);

	// Update the parent pointers of all the children of the fresh page.
	m_nodes[m_nodes[freshID].firstChildID].parentID = freshID;
	for(SortedPage::TupleSetCIter jt = page_begin(freshID), jend = page_end(freshID); jt != jend; ++jt)
	{
		m_nodes[child_node_id(*jt)].parentID = freshID;
	}

	return Split(nodeID, freshID, splitter);
}

void BTree::transfer_leaf_tuples_right(int sourceNodeID, unsigned int n)
{
	// Check the preconditions.
	int targetNodeID = m_nodes[sourceNodeID].siblingRightID;
	if(m_nodes[targetNodeID].parentID != m_nodes[sourceNodeID].parentID)
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
