/**
 * whery: BTree.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREE
#define H_WHERY_BTREE

#include "whery/db/pages/SortedPage.h"
#include "whery/util/IDAllocator.h"
#include "BTreePageController.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class FreshTuple;

/**
An instance of this class represents a B+-tree.
*/
class BTree
{
	//#################### NESTED TYPES ####################
private:
	struct InsertResult
	{
		int m_leftChildID;
		int m_middleChildID;
		int m_rightChildID;

		InsertResult()
		:	m_leftChildID(-1), m_middleChildID(-1), m_rightChildID(-1)
		{}

		InsertResult(int leftChildID, int middleChildID, int rightChildID)
		:	m_leftChildID(leftChildID), m_middleChildID(middleChildID), m_rightChildID(rightChildID)
		{}
	};

	/**
	An instance of this struct represents a node in a B+-tree.
	*/
	struct Node
	{
		/**
		The ID of the node's first child, if it has one. The IDs of any
		other children are stored in the tuples on the data page.
		*/
		int m_firstChildID;

		/** The page used to store the tuple data for the node. */
		SortedPage_Ptr m_page;

		/** The ID of the node's parent in the B+-tree (if any). */
		int m_parentID;

		/** The ID of the node's left sibling in the B+-tree (if any). */
		int m_siblingLeftID;

		/** The ID of the node's right sibling in the B+-tree (if any). */
		int m_siblingRightID;

		/**
		Constructs a node.
		*/
		Node()
		:	m_firstChildID(-1), m_parentID(-1), m_siblingLeftID(-1), m_siblingRightID(-1)
		{}

		/**
		Constructs a node.

		\param page	The page to be used to store the tuple data for the node.
		*/
		explicit Node(const SortedPage_Ptr& page)
		:	m_firstChildID(-1), m_page(page), m_parentID(-1), m_siblingLeftID(-1), m_siblingRightID(-1)
		{}

		/**
		Returns whether or not the node has any children.

		\return	true, if the node has children, or false otherwise.
		*/
		bool has_children() const
		{
			return m_firstChildID != -1;
		}
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

	//#################### PRIVATE VARIABLES ####################
private:
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

	\param result	TODO
	*/
	void add_root_node(const InsertResult& insertResult);

	/**
	Extracts the child node ID from a branch tuple of the form <key1,...,keyN,child node ID>.

	\param branchTuple	The tuple from which to extract the child node ID.
	\return				The child node ID from the tuple.
	*/
	int child_node_id(const BackedTuple& branchTuple) const;

	InsertResult insert_tuple_branch(const Tuple& tuple, int nodeID);
	InsertResult insert_tuple_leaf(const Tuple& tuple, int nodeID);

	/**
	Inserts a tuple into the subtree rooted at the specified node. This may ultimately cause
	the specified node to be split.

	\param tuple	The tuple to insert.
	\param nodeID	The ID of the node at the root of the subtree into which to insert it.
	\return			TODO
	*/
	InsertResult insert_tuple_sub(const Tuple& tuple, int nodeID);

	ValueKey make_branch_key(const Tuple& tuple) const;

	FreshTuple make_branch_tuple(const Tuple& tuple, int nodeID) const;

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
};

}

#endif
