/**
 * whery: BTree.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/btrees/BTree.h"

#include <cassert>
#include <boost/lexical_cast.hpp>

#include "whery/db/base/RangeKey.h"
#include "whery/util/TextUtil.h"

namespace whery {

//#################### CONSTRUCTORS ####################

BTree::BTree(const BTreePageController_CPtr& pageController)
:	m_pageController(pageController), m_tupleCount(0)
{
	// Create the root node.
	m_rootID = m_firstLeafID = m_lastLeafID = add_leaf_node();

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
	return ConstIterator(this, m_firstLeafID, page_begin(m_firstLeafID));
}

BTree::ConstIterator BTree::end() const
{
	return ConstIterator(this, m_lastLeafID, page_end(m_lastLeafID));
}

BTree::EqualRangeResult BTree::equal_range(const RangeKey& key) const
{
	return std::make_pair(lower_bound(key), upper_bound(key));
}

BTree::EqualRangeResult BTree::equal_range(const ValueKey& key) const
{
	return std::make_pair(lower_bound(key), upper_bound(key));
}

void BTree::erase_tuple(const ValueKey& key)
{
	boost::optional<Merge> result = erase_tuple_from_subtree(key, m_rootID);
	assert(!result);
	--m_tupleCount;
}

BTree::ConstIterator BTree::find(const ValueKey& key) const
{
	ConstIterator it = lower_bound(key), iend = end();
	if(it != iend)
	{
		PrefixTupleComparator comp;
		if(comp.compare(*it, key) != 0)
		{
			it = iend;
		}
	}
	return it;
}

void BTree::insert_tuple(const Tuple& tuple)
{
	boost::optional<Split> result = insert_tuple_into_subtree(tuple, m_rootID);
	assert(!result);
	++m_tupleCount;
}

TupleManipulator BTree::leaf_tuple_manipulator() const
{
	return m_pageController->btree_leaf_tuple_manipulator();
}

BTree::ConstIterator BTree::lower_bound(const RangeKey& key) const
{
	if(key.has_low_endpoint())
	{
		ConstIterator it = lower_bound(key.low_value());
		if(key.low_kind() == OPEN)
		{
			ConstIterator iend = end();
			PrefixTupleComparator comp;
			while(it != iend && comp.compare(*it, key.low_value()) == 0) ++it;
		}
		return it;
	}
	else return begin();
}

BTree::ConstIterator BTree::lower_bound(const ValueKey& key) const
{
	int id = m_rootID;
	SortedPage::TupleSetCIter it = page(id)->lower_bound(key);

	// Walk down the B+-tree to find the lower bound. At the start of each iteration,
	// the iterator it points to the lower bound of the key in the current node.
	while(m_nodes[id].has_children())
	{
		id = left_child_of(it, id);
		it = page(id)->lower_bound(key);
	}

	// If the iterator points to the end of the leaf page, move it to the start
	// of the leaf page's right sibling (if any).
	if(it == page_end(id) && m_nodes[id].siblingRightID != -1)
	{
		id = m_nodes[id].siblingRightID;
		it = page_begin(id);
	}

	return ConstIterator(this, id, it);
}

void BTree::print(std::ostream& os) const
{
	print_subtree(os, m_rootID, 0);
}

unsigned int BTree::tuple_count()
{
	return m_tupleCount;
}

BTree::ConstIterator BTree::upper_bound(const RangeKey& key) const
{
	if(key.has_high_endpoint())
	{
		ConstIterator it = upper_bound(key.high_value());
		if(key.high_kind() == OPEN)
		{
			PrefixTupleComparator comp;
			ConstIterator test = it;
			--test;
			while(test != it && comp.compare(*test, key.high_value()) == 0)
			{
				it = test;
				--test;
			}
		}
		return it;
	}
	else return end();
}

BTree::ConstIterator BTree::upper_bound(const ValueKey& key) const
{
	int id = m_rootID;
	SortedPage::TupleSetCIter it = page(id)->upper_bound(key);

	// Walk down the B+-tree to find the upper bound. At the start of each iteration,
	// the iterator it points to the upper bound of the key in the current node.
	while(m_nodes[id].has_children())
	{
		id = left_child_of(it, id);
		it = page(id)->upper_bound(key);
	}

	// If the iterator points to the end of the leaf page, move it to the start
	// of the leaf page's right sibling (if any).
	if(it == page_end(id) && m_nodes[id].siblingRightID != -1)
	{
		id = m_nodes[id].siblingRightID;
		it = page_begin(id);
	}

	return ConstIterator(this, id, it);
}

//#################### PRIVATE METHODS ####################

int BTree::add_branch_node()
{
	int id = add_node();

	Node& n = m_nodes[id];
	n.page = m_pageController->make_btree_branch_page();

	return id;
}

void BTree::add_index_entry(int nodeID)
{
	assert(m_nodes[nodeID].parentID != -1);
	SortedPage_Ptr parentPage = page(m_nodes[nodeID].parentID);
	parentPage->add_tuple(make_branch_tuple(*page_begin(nodeID), nodeID));
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

boost::optional<BTree::Split> BTree::add_root_node(const Split& split)
{
	m_rootID = add_branch_node();
	m_nodes[split.leftNodeID].parentID = m_rootID;
	m_nodes[split.rightNodeID].parentID = m_rootID;
	m_nodes[m_rootID].firstChildID = split.leftNodeID;
	page(m_rootID)->add_tuple(make_branch_tuple(split.splitter, split.rightNodeID));
	return boost::none;
}

TupleManipulator BTree::branch_tuple_manipulator() const
{
	return m_pageController->btree_branch_tuple_manipulator();
}

int BTree::child_node_id(const BackedTuple& branchTuple) const
{
	int id = branchTuple.field(branchTuple.arity() - 1).get_int();
	assert(0 <= id && static_cast<unsigned int>(id) < m_nodes.size() && page(id).get() != NULL);
	return id;
}

void BTree::delete_node(int nodeID)
{
	m_nodeIDAllocator.deallocate(nodeID);

	Node& n = m_nodes[nodeID];
	n.page.reset();
	n.firstChildID = n.parentID = n.siblingLeftID = n.siblingRightID = -1;
}

void BTree::erase_index_entry(int nodeID)
{
	page(m_nodes[nodeID].parentID)->erase_tuple(find_index_entry(nodeID));
}

boost::optional<BTree::Merge> BTree::erase_tuple_from_branch(const ValueKey& key, int nodeID)
{
	// Find the child of this node below which tuples that match the specified key can be found,
	// and erase the tuple from the subtree below it.
	int childNodeID = left_child_of(page(nodeID)->lower_bound(key), nodeID);
	boost::optional<Merge> result = erase_tuple_from_subtree(key, childNodeID);

	if(!result)
	{
		// The erasure succeeded without any merge occurring on the level directly below this one.
		return result;
	}
	else
	{
		// A merge occurred in the level below this one, so check whether the minimum tuple invariant
		// for the node in this level has been violated and restore it if it has.

		// Get the ID of the relevant node at this level (the parent of the node resulting from the lower-level merge).
		int relevantNodeID = m_nodes[result->nodeID].parentID;

		if(relevantNodeID == m_rootID)
		{
			// If the relevant node is the root and the merge in the level below erased the last tuple in it,
			// decrease the height of the tree.
			if(page(relevantNodeID)->tuple_count() == 0)
			{
				int oldRootID = m_rootID;
				m_rootID = m_nodes[m_rootID].firstChildID;
				m_nodes[m_rootID].parentID = -1;
				delete_node(oldRootID);
			}

			return boost::none;
		}
		// FIXME: Factor this test into a predicate.
		else if(page(relevantNodeID)->tuple_count() >= page(relevantNodeID)->max_tuple_count() / 2)
		{
			// If the relevant node is not the root and it still satisfies its minimum tuple invariant,
			// then there is no need to do anything.
			return boost::none;
		}
		else
		{
			// If the relevant node is not the root and its minimum tuple invariant has been violated,
			// restore it by redistributing tuples from a sibling node or merging with a sibling node.

			// Check whether the node has a "useful" left or right sibling, i.e. one that has the same parent as it
			// (noting that we can't redistribute tuples between or merge nodes that do not have the same parent).
			// The node must have at least one useful sibling, since otherwise the B+-tree would be invalid (we know
			// that this node is the child of a branch node, and all branch nodes have at least two children).
			bool hasUsefulLeftSibling = is_useful_sibling(relevantNodeID, m_nodes[relevantNodeID].siblingLeftID);
			bool hasUsefulRightSibling = is_useful_sibling(relevantNodeID, m_nodes[relevantNodeID].siblingRightID);
			assert(hasUsefulLeftSibling || hasUsefulRightSibling);

			if(hasUsefulLeftSibling && has_more_than_min_tuples(m_nodes[relevantNodeID].siblingLeftID))
			{
				// TODO
				redistribute_from_left_branch(relevantNodeID);
				return boost::none;
			}
			else if(hasUsefulRightSibling && has_more_than_min_tuples(m_nodes[relevantNodeID].siblingRightID))
			{
				// TODO
				throw 23;
			}
			else if(hasUsefulLeftSibling)
			{
				// TODO
				return merge_branches(m_nodes[relevantNodeID].siblingLeftID, nodeID);
			}
			else
			{
				// TODO
				return merge_branches(nodeID, m_nodes[relevantNodeID].siblingRightID);
			}
		}
	}
}

boost::optional<BTree::Merge> BTree::erase_tuple_from_leaf(const ValueKey& key, int nodeID)
{
	SortedPage_Ptr nodePage = page(nodeID);

	// If all of the tuples in the node are less than the key,
	// and there is a right sibling, erase from that instead.
	SortedPage::TupleSetCIter it = nodePage->lower_bound(key);
	if(it == nodePage->end() && m_nodes[nodeID].siblingRightID != -1)
	{
		return erase_tuple_from_leaf(key, m_nodes[nodeID].siblingRightID);
	}

	// If the tuple pointed to by the iterator does not compare equal
	// to the key, then no tuple in the B+-tree does, so early out.
	PrefixTupleComparator comp;
	if(comp.compare(*it, key) != 0)
	{
		return boost::none;
	}

	if(nodeID == m_rootID || has_more_than_min_tuples(nodeID))
	{
		// Either this node is the root (in which case it has no minimum tuple requirement),
		// or it would still be at least half full after a deletion, so simply erase the
		// first tuple that matches the key.
		nodePage->erase_tuple(it);
		return boost::none;
	}
	else
	{
		// Check whether the node has a "useful" left or right sibling, i.e. one that has the same parent as it
		// (noting that we can't redistribute tuples between or merge nodes that do not have the same parent).
		// The node must have at least one useful sibling, since otherwise the B+-tree would be invalid (we know
		// that this node is the child of a branch node, and all branch nodes have at least two children).
		bool hasUsefulLeftSibling = is_useful_sibling(nodeID, m_nodes[nodeID].siblingLeftID);
		bool hasUsefulRightSibling = is_useful_sibling(nodeID, m_nodes[nodeID].siblingRightID);
		assert(hasUsefulLeftSibling || hasUsefulRightSibling);

		if(hasUsefulLeftSibling && has_more_than_min_tuples(m_nodes[nodeID].siblingLeftID))
		{
			// This node would be less than half full after a deletion, but its left sibling
			// has a tuple to spare, so we can avoid the need for a merge.
			redistribute_from_left_leaf_and_erase(nodeID, it);
			return boost::none;
		}
		else if(hasUsefulRightSibling && has_more_than_min_tuples(m_nodes[nodeID].siblingRightID))
		{
			// This node would be less than half full after a deletion, but its right sibling
			// has a tuple to spare, so we can avoid the need for a merge.
			redistribute_from_right_leaf_and_erase(nodeID, it);
			return boost::none;
		}
		else if(hasUsefulLeftSibling)
		{
			// This node would be less than half full after a deletion, and no redistribution from
			// a sibling is possible; it does however have a useful left sibling, so first erase
			// the tuple and then merge the two nodes.
			return merge_leaves_and_erase(nodeID, it, m_nodes[nodeID].siblingLeftID, nodeID);
		}
		else
		{
			// This node would be less than half full after a deletion, and no redistribution from
			// a sibling is possible; it does however have a useful right sibling, so first erase
			// the tuple and then merge the two nodes.
			return merge_leaves_and_erase(nodeID, it, nodeID, m_nodes[nodeID].siblingRightID);
		}
	}
}

boost::optional<BTree::Merge> BTree::erase_tuple_from_subtree(const ValueKey& key, int nodeID)
{
	if(m_nodes[nodeID].has_children())
	{
		return erase_tuple_from_branch(key, nodeID);
	}
	else
	{
		return erase_tuple_from_leaf(key, nodeID);
	}
}

SortedPage::TupleSetCIter BTree::find_index_entry(int nodeID) const
{
	assert(m_nodes[nodeID].parentID != -1);
	assert(m_nodes[m_nodes[nodeID].parentID].firstChildID != nodeID);

	SortedPage_Ptr parentPage = page(m_nodes[nodeID].parentID);

	SortedPage::TupleSetCIter it;
	if(page(nodeID)->tuple_count() > 0)
	{
		// Normally, the node itself will contain at least one tuple,
		// in which case we can use that to search for the entry in
		// the parent node. However, this may conceivably not be the
		// case during an erase operation, since the last tuple may
		// have been deleted by a lower-level merge.
		ValueKey key = make_branch_key(*page_begin(nodeID));
		it = parentPage->upper_bound(key);
		--it;
	}
	else
	{
		// If the node itself has no tuples due to an ongoing erase
		// operation, we instead search for the entry using a tuple
		// from its left sibling. This must exist and have the same
		// parent as the node, since it is a precondition of this
		// method that the node is not its parent's first child.
		assert(is_useful_sibling(nodeID, m_nodes[nodeID].siblingLeftID));
		ValueKey key = make_branch_key(*page_begin(m_nodes[nodeID].siblingLeftID));
		it = parentPage->upper_bound(key);
	}

	return it;
}

bool BTree::has_less_than_max_tuples(int nodeID) const
{
	return page(nodeID)->empty_tuple_count() > 0;
}

bool BTree::has_more_than_min_tuples(int nodeID) const
{
	return page(nodeID)->tuple_count() - 1 >= page(nodeID)->max_tuple_count() / 2;
}

void BTree::insert_node_as_right_sibling_of(int existingID, int freshID)
{
	m_nodes[freshID].parentID = m_nodes[existingID].parentID;
	m_nodes[freshID].siblingLeftID = existingID;
	m_nodes[freshID].siblingRightID = m_nodes[existingID].siblingRightID;
	m_nodes[existingID].siblingRightID = freshID;
	if(m_nodes[freshID].siblingRightID != -1) m_nodes[m_nodes[freshID].siblingRightID].siblingLeftID = freshID;
	if(m_lastLeafID == existingID) m_lastLeafID = freshID;
}

boost::optional<BTree::Split> BTree::insert_tuple_into_branch(const Tuple& tuple, int nodeID)
{
	// Find the child of this node below which the specified tuple should be inserted,
	// and insert the tuple into the subtree below it.
	int childNodeID = left_child_of(page(nodeID)->upper_bound(make_branch_key(tuple)), nodeID);
	boost::optional<Split> result = insert_tuple_into_subtree(tuple, childNodeID);

	if(!result)
	{
		// The insertion succeeded without needing to split the direct child of this node.
		return result;
	}
	else if(has_less_than_max_tuples(nodeID))
	{
		// The child of this node was split, and there's space in this node, so
		// insert an index entry for the right-hand node returned by the split.
		page(nodeID)->add_tuple(make_branch_tuple(result->splitter, result->rightNodeID));
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

boost::optional<BTree::Split> BTree::insert_tuple_into_leaf(const Tuple& tuple, int nodeID)
{
	if(has_less_than_max_tuples(nodeID))
	{
		// This node has spare capacity, so simply insert the tuple into it.
		page(nodeID)->add_tuple(tuple);
		return boost::none;
	}
	else if(m_nodes[nodeID].siblingLeftID != -1 &&
			m_nodes[m_nodes[nodeID].siblingLeftID].parentID == m_nodes[nodeID].parentID &&
			has_less_than_max_tuples(m_nodes[nodeID].siblingLeftID))
	{
		// This node is full, but its left sibling has the same parent and spare capacity,
		// so we can avoid the need for a split.
		redistribute_leaf_left_and_insert(nodeID, tuple);
		return boost::none;
	}
	else if(m_nodes[nodeID].siblingRightID != -1 &&
			m_nodes[m_nodes[nodeID].siblingRightID].parentID == m_nodes[nodeID].parentID &&
			has_less_than_max_tuples(m_nodes[nodeID].siblingRightID))
	{
		// This node is full, but its right sibling has the same parent and spare capacity,
		// so we can avoid the need for a split.
		redistribute_leaf_right_and_insert(nodeID, tuple);
		return boost::none;
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

boost::optional<BTree::Split> BTree::insert_tuple_into_subtree(const Tuple& tuple, int nodeID)
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

bool BTree::is_useful_sibling(int nodeID, int siblingID) const
{
	return siblingID != -1 && m_nodes[siblingID].parentID == m_nodes[nodeID].parentID;
}

int BTree::left_child_of(const SortedPage::TupleSetCIter& it, int branchNodeID) const
{
	if(it == page_begin(branchNodeID))
	{
		return m_nodes[branchNodeID].firstChildID;
	}
	else
	{
		SortedPage::TupleSetCIter jt = it;
		--jt;
		return child_node_id(*jt);
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
	assert(sourceTuple.arity() >= result.arity() - 1);
	for(unsigned int i = 0; i < result.arity() - 1; ++i)
	{
		result.field(i).set_from(sourceTuple.field(i));
	}

	// Set the last field of the branch tuple to the child node ID.
	result.field(result.arity() - 1).set_int(childNodeID);

	return result;
}

BTree::Merge BTree::merge_branches(int leftNodeID, int rightNodeID)
{
	// TODO: This is very similar to the code in merge_leaves_and_erase - factor out the commonality.

	// Pull down the index entry for the right-hand node from the parent page into the left-hand node.
	SortedPage::TupleSetCIter it = find_index_entry(rightNodeID);
	page(leftNodeID)->add_tuple(make_branch_tuple(*it, m_nodes[rightNodeID].firstChildID));

	// Erase the index entry for the right-hand node from the parent page.
	page(m_nodes[leftNodeID].parentID)->erase_tuple(it);

	// Update the parent pointers for the children of the right-hand node to make them point to the left-hand node.
	update_parent_pointers(rightNodeID, leftNodeID);

	// Transfer all tuples from the right-hand node to the left-hand node.
	assert(page(leftNodeID)->tuple_count() + page(rightNodeID)->tuple_count() <= page(leftNodeID)->max_tuple_count());
	transfer_leaf_tuples_left(rightNodeID, page(rightNodeID)->tuple_count());

	// Disconnect the right-hand node from the B+-tree and delete it.
	m_nodes[leftNodeID].siblingRightID = m_nodes[rightNodeID].siblingRightID;
	if(m_nodes[leftNodeID].siblingRightID != -1) m_nodes[m_nodes[leftNodeID].siblingRightID].siblingLeftID = leftNodeID;
	delete_node(rightNodeID);

	return Merge(leftNodeID);
}

BTree::Merge BTree::merge_leaves_and_erase(int nodeID, const SortedPage::TupleSetCIter& it, int leftNodeID, int rightNodeID)
{
	// Erase the index entry for the right-hand node from the parent page.
	erase_index_entry(rightNodeID);

	// Erase the tuple itself.
	page(nodeID)->erase_tuple(it);

	// Transfer all tuples from the right-hand node to the left-hand node.
	assert(page(leftNodeID)->tuple_count() + page(rightNodeID)->tuple_count() <= page(leftNodeID)->max_tuple_count());
	transfer_leaf_tuples_left(rightNodeID, page(rightNodeID)->tuple_count());

	// Disconnect the right-hand node from the B+-tree and delete it.
	m_nodes[leftNodeID].siblingRightID = m_nodes[rightNodeID].siblingRightID;
	if(m_nodes[leftNodeID].siblingRightID != -1) m_nodes[m_nodes[leftNodeID].siblingRightID].siblingLeftID = leftNodeID;
	delete_node(rightNodeID);

	return Merge(leftNodeID);
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

SortedPage::TupleSetCRIter BTree::page_rend(int nodeID) const
{
	SortedPage_Ptr p = page(nodeID);
	assert(p.get() != NULL);
	return p->rend();
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

void BTree::redistribute_from_left_branch(int nodeID)
{
	assert(m_nodes[nodeID].has_children());
	assert(is_useful_sibling(nodeID, m_nodes[nodeID].siblingLeftID));

	// Pull the index entry for the node down from its parent page.
	// TODO: This is similar to some code in merge_branches - factor out the commonality.
	SortedPage::TupleSetCIter it = find_index_entry(nodeID);
	page(nodeID)->add_tuple(make_branch_tuple(*it, m_nodes[nodeID].firstChildID));
	page(m_nodes[nodeID].parentID)->erase_tuple(it);

	// Make a note of the last child of the left sibling.
	SortedPage_Ptr leftPage = page(m_nodes[nodeID].siblingLeftID);
	int childID = child_node_id(*leftPage->rbegin());

	// Push the last index entry of the left sibling up to the parent node.
	page(m_nodes[nodeID].parentID)->add_tuple(make_branch_tuple(*leftPage->rbegin(), nodeID));
	leftPage->erase_tuple(leftPage->rbegin());

	// Update the first child of the node to be the last child of its left sibling.
	m_nodes[nodeID].firstChildID = childID;
	m_nodes[childID].parentID = nodeID;
}

void BTree::redistribute_from_left_leaf_and_erase(int nodeID, const SortedPage::TupleSetCIter& it)
{
	// Erase the index entry for this node from the parent page (an updated index
	// entry will be re-added below).
	erase_index_entry(nodeID);

	// Erase the tuple (temporarily breaking the minimum tuple invariant for the page).
	page(nodeID)->erase_tuple(it);

	// Restore the minimum tuple invariant by transferring a tuple across from the left sibling.
	transfer_leaf_tuples_right(m_nodes[nodeID].siblingLeftID, 1);

	// Re-add an index entry for this node to the parent page.
	add_index_entry(nodeID);
}

void BTree::redistribute_from_right_leaf_and_erase(int nodeID, const SortedPage::TupleSetCIter& it)
{
	// Erase the index entry for the right sibling from the parent page (an updated index
	// entry will be re-added below).
	erase_index_entry(m_nodes[nodeID].siblingRightID);

	// If this node is not its parent's first child, similarly erase its index entry.
	if(nodeID != m_nodes[m_nodes[nodeID].parentID].firstChildID)
	{
		erase_index_entry(nodeID);
	}

	// Erase the tuple (temporarily breaking the minimum tuple invariant for the page).
	page(nodeID)->erase_tuple(it);

	// Restore the minimum tuple invariant by transferring a tuple across from the right sibling.
	transfer_leaf_tuples_left(m_nodes[nodeID].siblingRightID, 1);

	// Re-add an index entry for the right sibling to the parent page.
	add_index_entry(m_nodes[nodeID].siblingRightID);

	// If this node is not its parent's first child, similarly re-add an index entry for it.
	if(nodeID != m_nodes[m_nodes[nodeID].parentID].firstChildID)
	{
		add_index_entry(nodeID);
	}
}

void BTree::redistribute_leaf_left_and_insert(int nodeID, const Tuple& tuple)
{
	// Erase the index entry for this node from the parent page (an updated index
	// entry will be re-added below).
	erase_index_entry(nodeID);

	// Noting that the tuple must be at least as large as the first tuple on this
	// page, or we wouldn't be trying to insert it here, we can redistribute the
	// first tuple across to the left sibling to make space, and then insert the
	// tuple into this page.
	transfer_leaf_tuples_left(nodeID, 1);
	page(nodeID)->add_tuple(tuple);

	// Re-add an index entry for this node to the parent page.
	add_index_entry(nodeID);
}

void BTree::redistribute_leaf_right_and_insert(int nodeID, const Tuple& tuple)
{
	// Erase the index entry for the right sibling from the parent page (an updated
	// index entry will be re-added below).
	erase_index_entry(m_nodes[nodeID].siblingRightID);

	PrefixTupleComparator comp;
	if(comp.compare(*page_rbegin(nodeID), tuple) == -1)
	{
		// If the tuple is greater than the last tuple on this page, it can be inserted
		// into the right sibling (which has space). Note that this is a valid thing to
		// do because the tuple must also be less than the first tuple on the right page
		// (or we wouldn't be trying to insert it here in the first place).
		page(m_nodes[nodeID].siblingRightID)->add_tuple(tuple);
	}
	else
	{
		// If the tuple is not greater than the last tuple on this page, we can redistribute
		// the last tuple across to the right sibling to make space, and then insert the tuple
		// into this page.
		transfer_leaf_tuples_right(nodeID, 1);
		page(nodeID)->add_tuple(tuple);
	}

	// Re-add an index entry for the right sibling to the parent page.
	add_index_entry(m_nodes[nodeID].siblingRightID);
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
	// Check the precondition.
	if(has_less_than_max_tuples(nodeID))
	{
		throw std::invalid_argument("Cannot split a non-full leaf node.");
	}

	// Create a fresh leaf node and connect it to the rest of the tree.
	int freshID = add_leaf_node();
	insert_node_as_right_sibling_of(nodeID, freshID);

	// Transfer half of the tuples across to the fresh node.
	transfer_leaf_tuples_right(nodeID, page(nodeID)->tuple_count() / 2);

	// Insert the original tuple into the appropriate node.
	selectively_insert_tuple(tuple, nodeID, freshID);

	// Construct and return the split result.
	Split split(nodeID, freshID, FreshTuple(leaf_tuple_manipulator()));
	split.splitter.copy_from(*page_begin(freshID));
	return split;
}

BTree::Split BTree::split_branch_and_insert(int nodeID, const FreshTuple& tuple)
{
	// Check the precondition.
	if(has_less_than_max_tuples(nodeID))
	{
		throw std::invalid_argument("Cannot split a non-full branch node.");
	}

	// Create a fresh branch node and connect it to the rest of the tree.
	int freshID = add_branch_node();
	insert_node_as_right_sibling_of(nodeID, freshID);

	// Make a tuple set containing fresh copies of all the tuples in the original page.
	typedef std::multiset<FreshTuple,PrefixTupleComparator> FreshTupleSet;
	FreshTupleSet tuples;
	TupleManipulator branchTupleManipulator = branch_tuple_manipulator();
	SortedPage_Ptr nodePage = page(nodeID);
	for(SortedPage::TupleSetCIter it = nodePage->begin(), iend = nodePage->end(); it != iend; ++it)
	{
		FreshTuple temp(branchTupleManipulator);
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
	FreshTuple splitter(branchTupleManipulator);
	splitter.copy_from(*it);
	++it;

	// Copy the second half of the tuple set across to the fresh page.
	SortedPage_Ptr freshPage = page(freshID);
	for(; it != iend; ++it)
	{
		freshPage->add_tuple(*it);
	}

	// Set the first child of the fresh node to be the child pointed to by the splitter.
	m_nodes[freshID].firstChildID = child_node_id(splitter);

	// Update the parent pointers of all the children of the fresh page.
	update_parent_pointers(freshID, freshID);

	return Split(nodeID, freshID, splitter);
}

void BTree::transfer_leaf_tuples(int sourceNodeID, int targetNodeID, const std::vector<BackedTuple>& tuples)
{
	// Check the preconditions.
	if(m_nodes[targetNodeID].parentID != m_nodes[sourceNodeID].parentID)
	{
		throw std::invalid_argument("Cannot transfer tuples to a node with a different parent.");
	}

	SortedPage_Ptr targetPage = page(targetNodeID);
	if(targetPage->empty_tuple_count() < tuples.size())
	{
		throw std::invalid_argument("Cannot transfer tuples to a page with insufficient space to hold them.");
	}

	// Transfer the tuples to the target page.
	SortedPage_Ptr sourcePage = page(sourceNodeID);
	for(std::vector<BackedTuple>::const_iterator it = tuples.begin(), iend = tuples.end(); it != iend; ++it)
	{
		targetPage->add_tuple(*it);
		sourcePage->erase_tuple(*it);
	}
}

void BTree::transfer_leaf_tuples_left(int sourceNodeID, unsigned int n)
{
	if(page(sourceNodeID)->tuple_count() < n)
	{
		throw std::invalid_argument("The specified page does not contain sufficient tuples to transfer.");
	}

	std::vector<BackedTuple> tuples;
	tuples.reserve(n);
	unsigned int i = 0;
	for(SortedPage::TupleSetCIter it = page_begin(sourceNodeID), iend = page_end(sourceNodeID); it != iend && i < n; ++it, ++i)
	{
		tuples.push_back(*it);
	}

	transfer_leaf_tuples(sourceNodeID, m_nodes[sourceNodeID].siblingLeftID, tuples);
}

void BTree::transfer_leaf_tuples_right(int sourceNodeID, unsigned int n)
{
	if(page(sourceNodeID)->tuple_count() < n)
	{
		throw std::invalid_argument("The specified page does not contain sufficient tuples to transfer.");
	}

	std::vector<BackedTuple> tuples;
	tuples.reserve(n);
	unsigned int i = 0;
	for(SortedPage::TupleSetCRIter it = page_rbegin(sourceNodeID), iend = page_rend(sourceNodeID); it != iend && i < n; ++it, ++i)
	{
		tuples.push_back(*it);
	}

	transfer_leaf_tuples(sourceNodeID, m_nodes[sourceNodeID].siblingRightID, tuples);
}

void BTree::update_parent_pointers(int oldParentID, int newParentID)
{
	m_nodes[m_nodes[oldParentID].firstChildID].parentID = newParentID;
	for(SortedPage::TupleSetCIter it = page_begin(oldParentID), iend = page_end(oldParentID); it != iend; ++it)
	{
		m_nodes[child_node_id(*it)].parentID = newParentID;
	}
}

}
