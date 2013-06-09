/**
 * whery: BTree.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREE
#define H_WHERY_BTREE

#include <iostream>

#include <boost/optional/optional.hpp>

#include "whery/db/base/FreshTuple.h"
#include "whery/db/pages/SortedPage.h"
#include "whery/util/IDAllocator.h"
#include "BTreePageController.h"

namespace whery {

/**
An instance of this class represents a B+-tree.
*/
class BTree
{
	//#################### NESTED TYPES ####################
private:
	/**
	An instance of this struct represents a node in a B+-tree.
	*/
	struct Node
	{
		/**
		The ID of the node's first child, if it has one. The IDs of any
		other children are stored in the tuples on the data page.
		*/
		int firstChildID;

		/** The page used to store the tuple data for the node. */
		SortedPage_Ptr page;

		/** The ID of the node's parent in the B+-tree (if any). */
		int parentID;

		/** The ID of the node's left sibling in the B+-tree (if any). */
		int siblingLeftID;

		/** The ID of the node's right sibling in the B+-tree (if any). */
		int siblingRightID;

		/**
		Constructs a node.
		*/
		Node()
		:	firstChildID(-1), parentID(-1), siblingLeftID(-1), siblingRightID(-1)
		{}

		/**
		Constructs a node.

		\param page	The page to be used to store the tuple data for the node.
		*/
		explicit Node(const SortedPage_Ptr& page_)
		:	firstChildID(-1), page(page_), parentID(-1), siblingLeftID(-1), siblingRightID(-1)
		{}

		/**
		Returns whether or not the node has any children.

		\return	true, if the node has children, or false otherwise.
		*/
		bool has_children() const
		{
			return firstChildID != -1;
		}
	};

	/**
	An instance of this class represents the splitting of a B+-tree node in two, e.g. during an insert operaton.
	*/
	struct Split
	{
		/** The ID of the left-hand node resulting from the split. */
		const int leftNodeID;

		/** The ID of the right-hand node resulting from the split. */
		const int rightNodeID;

		/** A key that can be used to separate the tuples in the two halves of the split. */
		FreshTuple splitter;

		/**
		Constructs a split.

		\param leftNodeID	The ID of the left-hand node resulting from the split.
		\param rightNodeID	The ID of the right-hand node resulting from the split.
		\param splitter		A key that can be used to separate the tuples in the two halves of the split.
		*/
		Split(int leftNodeID_, int rightNodeID_, const FreshTuple& splitter_)
		:	leftNodeID(leftNodeID_), rightNodeID(rightNodeID_), splitter(splitter_)
		{}
	};

public:
	class ConstIterator
	{
	private:
		const BTree *m_tree;
		int m_nodeID;
		SortedPage::TupleSetCIter m_it;
	public:
		ConstIterator(const BTree *tree, int nodeID, SortedPage::TupleSetCIter it)
		:	m_tree(tree), m_nodeID(nodeID), m_it(it)
		{}

		bool operator==(const ConstIterator& rhs) const
		{
			return m_tree == rhs.m_tree && m_nodeID == rhs.m_nodeID && m_it == rhs.m_it;
		}
	};

	//#################### TYPEDEFS ####################
private:
	typedef boost::optional<Split> OptionalSplit;

	//#################### PRIVATE VARIABLES ####################
private:
	/** The field indices to use when making branch keys. */
	std::vector<unsigned int> m_branchKeyFieldIndices;

	/** An ID allocator used to allocate IDs for the nodes. */
	IDAllocator m_nodeIDAllocator;

	/** The nodes in the B+-tree. */
	std::vector<Node> m_nodes;

	/** The page controller used to construct/destroy pages for the B+-tree. */
	BTreePageController_CPtr m_pageController;

	/** The ID of the root node. */
	int m_rootID;

	/** The number of tuples currently stored in the leaf nodes. */
	unsigned int m_tupleCount;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a B+-tree whose pages are to be constructed/destroyed using the specified controller.

	\param pageController	The page controller to be used to construct/destroy pages for the B+-tree.
	*/
	explicit BTree(const BTreePageController_CPtr& pageController);

	//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################
private:
	/** Private and unimplemented - copying and assignment are potentially expensive for B+-trees. */
	BTree(const BTree&);
	BTree& operator=(const BTree&);

	//#################### PUBLIC METHODS ####################
public:
	/**
	Returns an iterator pointing to the start of the set of tuples in the B+-tree.

	\return	An iterator pointing to the start of the set of tuples in the B+-tree.
	*/
	ConstIterator begin() const;

	/**
	Returns a tuple manipulator that can be used to interact with the B+-tree's branch (index) tuples.

	\return	The tuple manipulator.
	*/
	TupleManipulator branch_tuple_manipulator() const;

	void bulk_load(const std::vector<SortedPage_Ptr>& pages);
	void clear();

	/**
	Returns an iterator pointing to the end of the set of tuples in the B+-tree.

	\return	An iterator pointing to the end of the set of tuples in the B+-tree.
	*/
	ConstIterator end() const;

	// TODO: equal_range
	void erase_tuple(const RangeKey& key);
	void erase_tuples(const ValueKey& key);

	/**
	Inserts a tuple into the B+-tree.

	\param tuple	The tuple to insert.
	*/
	void insert_tuple(const Tuple& tuple);

	/**
	Returns a tuple manipulator that can be used to interact with the B+-tree's leaf (data) tuples.

	\return	The tuple manipulator.
	*/
	TupleManipulator leaf_tuple_manipulator() const;

	ConstIterator lower_bound(const RangeKey& key) const;
	ConstIterator lower_bound(const ValueKey& key) const;

	/**
	Prints the B+-tree to an output stream (for debugging purposes).

	\param os	The output stream.
	*/
	void print(std::ostream& os) const;

	/**
	Gets the number of tuples currently stored in the B+-tree's leaf nodes.

	\return	The number of tuples currently stored in the B+-tree's leaf nodes.
	*/
	unsigned int tuple_count();

	ConstIterator upper_bound(const RangeKey& key) const;
	ConstIterator upper_bound(const ValueKey& key) const;

	//#################### PRIVATE METHODS ####################
private:
	/**
	Adds a branch (index) node that can be connected to the rest of the B+-tree.

	\return	The ID of the branch node.
	*/
	int add_branch_node();

	/**
	Adds a leaf (data) node that can be connected to the rest of the B+-tree.

	\return	The ID of the leaf node.
	*/
	int add_leaf_node();

	/**
	Adds a node that can be connected to the rest of the B+-tree. The internals of the node will
	be set by add_branch_node() or add_leaf_node(), which use this function as a helper.

	\return	The ID of the node.
	*/
	int add_node();

	/**
	Adds a new root node to the B+-tree. New root nodes are added in response to a split of the
	old root during an insert operation, so there will be two children of the new root, one of
	which must be the old root.

	\param split	The split that created the need for a new root.
	\return			boost::none (for convenience).
	*/
	OptionalSplit add_root_node(const Split& split);

	/**
	Extracts the child node ID from a branch tuple of the form <key1,...,keyN,child node ID>.

	\param branchTuple	The tuple from which to extract the child node ID.
	\return				The child node ID from the tuple.
	*/
	int child_node_id(const BackedTuple& branchTuple) const;

	/**
	Finds the ID of the child node of the specified branch node whose subtree might contain
	the specified leaf tuple.

	\param tuple		The leaf tuple for which to find the child node.
	\param branchNodeID	The ID of the branch node in which to search.
	\return				The ID of the child node of the branch node whose subtree might
						contain the leaf tuple.
	*/
	int find_child(const Tuple& leafTuple, int branchNodeID) const;

	/**
	Inserts a fresh node into the B+-tree as the right sibling of the specified node
	and with the same parent.

	\param nodeID	The ID of the node whose right sibling the fresh node will become.
	\param freshID	The ID of the fresh node.
	*/
	void insert_node_as_right_sibling_of(int nodeID, int freshID);

	/**
	Inserts a tuple into the subtree rooted at the specified branch node. This may cause
	the node to be split, in which case a split result will be returned.

	\param tuple	The tuple to insert.
	\param nodeID	The ID of the branch node at the root of the subtree into which to insert it.
	\return			The result of any split that occurs, or boost::none otherwise.
	*/
	OptionalSplit insert_tuple_into_branch(const Tuple& tuple, int nodeID);

	/**
	Inserts a tuple into the specified leaf node. This may cause the node to be split,
	in which case a split result will be returned.

	\param tuple	The tuple to insert.
	\param nodeID	The ID of the leaf node into which to insert it.
	\return			The result of any split that occurs, or boost::none otherwise.
	*/
	OptionalSplit insert_tuple_into_leaf(const Tuple& tuple, int nodeID);

	/**
	Inserts a tuple into the subtree rooted at the specified node. This may cause
	the node to be split, in which case a split result will be returned.

	\param tuple	The tuple to insert.
	\param nodeID	The ID of the node at the root of the subtree into which to insert it.
	\return			The result of any split that occurs, or boost::none otherwise.
	*/
	OptionalSplit insert_tuple_into_subtree(const Tuple& tuple, int nodeID);

	/**
	Makes a value key that can be used to search for tuples within a branch node. This has
	the same format as a branch tuple without the child node ID, since branch nodes are
	intended to store a mapping from branch keys to child node IDs. The fields of the key
	are taken from the specified source tuple (which can be either a leaf or a branch tuple).

	\param sourceTuple	The tuple from which to copy the branch key's fields.
	\return				The branch key.
	*/
	ValueKey make_branch_key(const Tuple& sourceTuple) const;

	/**
	Makes a branch tuple by copying the appropriate number of fields from a source tuple
	(which can be either a leaf or a branch tuple) and filling in the child node ID.

	\param sourceTuple	The tuple from which to copy all but one of the branch tuple's fields.
	\param childNodeID	The ID of the child node that will be stored in the last field of the
						branch tuple.
	\return				The branch tuple.
	*/
	FreshTuple make_branch_tuple(const Tuple& sourceTuple, int childNodeID) const;

	/**
	Returns the page of the specified node.

	\param nodeID	The node whose page we want to get.
	\return			The page of the specified node.
	*/
	SortedPage_Ptr page(int nodeID) const;

	/**
	Returns an iterator pointing to the start of the set of tuples on the specified node's page.

	\param nodeID	The ID of a node in the B+-tree.
	\return			An iterator pointing to the start of the set of tuples on the specified node's page.
	*/
	SortedPage::TupleSetCIter page_begin(int nodeID) const;

	/**
	Returns an iterator pointing to the end of the set of tuples on the specified node's page.

	\param nodeID	The ID of a node in the B+-tree.
	\return			An iterator pointing to the end of the set of tuples on the specified node's page.
	*/
	SortedPage::TupleSetCIter page_end(int nodeID) const;

	/**
	Returns an iterator pointing to the start of the reversed set of tuples on the specified node's page.

	\param nodeID	The ID of a node in the B+-tree.
	\return			An iterator pointing to the start of the reversed set of tuples on the specified node's page.
	*/
	SortedPage::TupleSetCRIter page_rbegin(int nodeID) const;

	/**
	Prints the subtree rooted at the specified node to an output stream (for debugging purposes).

	\param os		The output stream.
	\param nodeID	The ID of the node whose subtree should be printed.
	\param depth	The depth of the node in the tree (starting from 0 at the overall root).
	*/
	void print_subtree(std::ostream& os, int nodeID, unsigned int depth) const;

	/**
	Selectively inserts the specified tuple into one of two adjacent nodes,
	based on a comparison against the first tuple of the right-hand node.
	Both nodes must have space for an extra tuple.

	\param tuple					The tuple to insert.
	\param leftNodeID				The ID of the left-hand node.
	\param rightNodeID				The ID of the right-hand node.
	\throw std::invalid_argument	If at least one of the two nodes does
									not have space for an extra tuple.
	*/
	void selectively_insert_tuple(const Tuple& tuple, int leftNodeID, int rightNodeID);

	/**
	Splits a full branch node into two half-full branch nodes and inserts the specified tuple.

	\param nodeID					The ID of the branch node to split.
	\param tuple					The tuple to insert.
	\return							The result of the split.
	\throw std::invalid_argument	If the branch node is not full.
	*/
	Split split_branch_and_insert(int nodeID, const FreshTuple& tuple);

	/**
	Splits a full leaf node into two half-full leaf nodes and inserts the specified tuple.

	\param nodeID					The ID of the leaf node to split.
	\param tuple					The tuple to insert.
	\return							The result of the split.
	\throw std::invalid_argument	If the leaf node is not full.
	*/
	Split split_leaf_and_insert(int nodeID, const Tuple& tuple);

	/**
	Transfers n tuples from the specified leaf node to its right sibling.
	Note that this function makes no attempt to update the parent of the two
	nodes, and should therefore only be used as part of a larger algorithm
	that does do so.

	\param sourceNodeID				The ID of the node from which the tuples should be transferred.
	\param n						The number of tuples to transfer.
	\throw std::invalid_argument	If the right sibling does not have the same parent,
									or does not have space for an extra n tuples.
	*/
	void transfer_leaf_tuples_right(int sourceNodeID, unsigned int n);

	/**
	Writes some text to the specified output stream, prefixed by the specified number of tabs.

	\param os		The output stream.
	\param tabCount	The number of tabs with which to prefix the text.
	\param text		The text to write.
	*/
	void write_tabbed_text(std::ostream& os, unsigned int tabCount, const std::string& text) const;
};

}

#endif
