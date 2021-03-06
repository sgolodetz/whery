/**
 * whery: BTree.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREE
#define H_WHERY_BTREE

#include <boost/optional/optional.hpp>

#include "whery/db/base/FreshTuple.h"
#include "whery/db/pages/SortedPage.h"
#include "whery/util/IDAllocator.h"
#include "BTreePageController.h"

namespace whery {

/**
\brief An instance of this class represents a B+-tree.

Note that this implementation is designed to work with tuples that
incorporate a unique key, and as such does not support duplicates.
This turns out to be a minor restriction in practice.
*/
class BTree
{
	//#################### NESTED TYPES ####################
private:
	/**
	\brief An instance of this struct represents a node in a B+-tree.
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
	\brief An instance of this represents the merging of two B+-tree nodes into one, e.g. during an erase operation.
	*/
	struct Merge
	{
		/** The ID of the node resulting from the merge. */
		const int nodeID;

		/**
		Constructs a merge.

		\param nodeID	The ID of the node resulting from the merge.
		*/
		explicit Merge(int nodeID_)
		:	nodeID(nodeID_)
		{}
	};

	/**
	\brief An instance of this class represents the splitting of a B+-tree node in two, e.g. during an insert operaton.
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
	/**
	\brief An instance of this class can be used to traverse the leaf tuples in a B+-tree.

	The leaf tuples themselves cannot be modified through this interface.
	*/
	class ConstIterator : public std::iterator<std::bidirectional_iterator_tag, BackedTuple>
	{
		//#################### FRIENDS ####################
		friend class BTree;

		//#################### PRIVATE VARIABLES ####################
	private:
		/** The B+-tree for which this is an iterator. */
		const BTree *m_tree;

		/** The ID of the leaf node containing the currently-pointed-to tuple. */
		int m_nodeID;

		/** An iterator to the currently-pointed-to tuple within a leaf page. */
		SortedPage::TupleSetCIter m_it;

		//#################### CONSTRUCTORS ####################
	public:
		/**
		Constructs an invalid B+-tree iterator (it can be assigned something valid later).
		*/
		ConstIterator()
		:	m_tree(NULL), m_nodeID(-1)
		{}

	private:
		/**
		Constructs a B+-tree iterator.

		\param tree		The B+-tree for which this is an iterator.
		\param nodeID	The ID of the leaf node containing the initially-pointed-to tuple.
		\param it		An iterator to the initially-pointed-to tuple within the leaf page.
		*/
		ConstIterator(const BTree *tree, int nodeID, SortedPage::TupleSetCIter it)
		:	m_tree(tree), m_nodeID(nodeID), m_it(it)
		{}

		//#################### PUBLIC OPERATORS ####################
	public:
		const BackedTuple& operator*() const
		{
			return *m_it;
		}

		const BackedTuple *operator->() const
		{
			return m_it.operator->();
		}

		bool operator==(const ConstIterator& rhs) const
		{
			return m_tree == rhs.m_tree && m_nodeID == rhs.m_nodeID && m_it == rhs.m_it;
		}

		bool operator!=(const ConstIterator& rhs) const
		{
			return !(*this == rhs);
		}

		ConstIterator& operator++()
		{
			// Provided we're not at the end of the current page (which can only
			// happen if the page is an empty root page, in which case the whole
			// B+-tree must also be empty), increment the iterator.
			if(m_it != m_tree->page_end(m_nodeID))
			{
				++m_it;
			}

			// If we're at the end of the current page and there's a right sibling,
			// move the iterator to the start of the right sibling's page.
			if(m_it == m_tree->page_end(m_nodeID) && m_tree->m_nodes[m_nodeID].siblingRightID != -1)
			{
				m_nodeID = m_tree->m_nodes[m_nodeID].siblingRightID;
				m_it = m_tree->page_begin(m_nodeID);
			}

			return *this;
		}

		ConstIterator& operator--()
		{
			// If we're at the start of the current page and there's a left sibling,
			// move the iterator to the end of the left sibling's page.
			if(m_it == m_tree->page_begin(m_nodeID) && m_tree->m_nodes[m_nodeID].siblingLeftID != -1)
			{
				m_nodeID = m_tree->m_nodes[m_nodeID].siblingLeftID;
				m_it = m_tree->page_end(m_nodeID);
			}

			// Provided we're not at the start of the current page (which can only happen
			// here if we're at the start of the whole B+-tree), decrement the iterator.
			if(m_it != m_tree->page_begin(m_nodeID))
			{
				--m_it;
			}

			return *this;
		}
	};

	//#################### TYPEDEFS ####################
public:
	typedef std::pair<ConstIterator,ConstIterator> EqualRangeResult;

	//#################### PRIVATE VARIABLES ####################
private:
	/** The field indices to use when making branch keys. */
	std::vector<unsigned int> m_branchKeyFieldIndices;

	/** The ID of the first leaf node (used to optimise begin()). */
	int m_firstLeafID;

	/** The ID of the last leaf node (used to optimise end()). */
	int m_lastLeafID;

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
	Returns an iterator pointing to the start of the set of leaf (data) tuples in the B+-tree.

	\return	An iterator pointing to the start of the set of leaf (data) tuples in the B+-tree.
	*/
	ConstIterator begin() const;

	/**
	TODO: Loads a large number of tuples into an empty B+-tree to avoid the cost of repeated insertions.
	*/
	void bulk_load(const std::vector<SortedPage_Ptr>& pages);

	/**
	TODO: Clears the B+-tree. (This is deferred because of its interaction with entity management.)
	*/
	void clear();

	/**
	Returns an iterator pointing to the end of the set of leaf (data) tuples in the B+-tree.

	\return	An iterator pointing to the end of the set of leaf (data) tuples in the B+-tree.
	*/
	ConstIterator end() const;

	/**
	Calculates a pair of iterators that together bound those leaf (data) tuples in the B+-tree
	that are equivalent to the specified key. The pair returned will be equal to the pair
	[lower_bound(key), upper_bound(key)] unless the key is "invalid", in which case
	[lower_bound(key), lower_bound(key)] will be returned. This is essential, because in
	such cases the upper bound iterator would be strictly before the lower bound iterator.

	\return	A pair of iterators that together bound those leaf tuples in the B+-tree
			that are equivalent to the specified key.
	*/
	EqualRangeResult equal_range(const RangeKey& key) const;

	/**
	Returns the pair [lower_bound(key), upper_bound(key)].

	\return	The pair [lower_bound(key), upper_bound(key)].
	*/
	EqualRangeResult equal_range(const ValueKey& key) const;

	/**
	Erases the first leaf (data) tuple that matches the specified key from the B+-tree.
	Other leaf tuples that match the specified key remain in the tree.

	\param key	The key denoting the tuple to erase.
	*/
	void erase_tuple(const ValueKey& key);

	/**
	TODO: Erases any tuples that match the specified key from the B+-tree.

	\param key	The key denoting the tuples to erase.
	*/
	void erase_tuples(const RangeKey& key);

	/**
	TODO: Erases any tuples that match the specified key from the B+-tree.

	\param key	The key denoting the tuples to erase.
	*/
	void erase_tuples(const ValueKey& key);

	/**
	Returns an iterator pointing to the first leaf (data) tuple in the B+-tree
	that compares equal to key (if any), or end() otherwise.

	\param key	The search key.
	\return		An iterator pointing to the first leaf (data) tuple in the B+-tree
				that compares equal to key (if any), or end() otherwise.
	*/
	ConstIterator find(const ValueKey& key) const;

	/**
	Inserts a leaf (data) tuple into the B+-tree.

	\param tuple	The tuple to insert.
	*/
	void insert_tuple(const Tuple& tuple);

	/**
	Returns a tuple manipulator that can be used to interact with the B+-tree's leaf (data) tuples.

	\return	The tuple manipulator.
	*/
	TupleManipulator leaf_tuple_manipulator() const;

	/**
	Returns an iterator pointing to the leaf (data) tuple at the lower end
	of the range specified by key.

	\param key	The search key.
	\return		An iterator pointing to the leaf (data) tuple at the lower
				end of the range specified by key.
	*/
	ConstIterator lower_bound(const RangeKey& key) const;

	/**
	Returns an iterator pointing to the first leaf (data) tuple in the B+-tree
	that is not ordered before the specified key (using prefix comparison).

	\param key	The search key.
	\return		An iterator pointing to the first leaf (data) tuple in the B+-tree
				that is not ordered before key, or end() if all tuples are ordered
				before key.
	*/
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

	/**
	Returns an iterator pointing one beyond the leaf (data) tuple at the higher end
	of the range specified by key.

	\param key	The search key.
	\return		An iterator pointing one beyond the leaf (data) tuple at the higher
				end of the range specified by key.
	*/
	ConstIterator upper_bound(const RangeKey& key) const;

	/**
	Returns an iterator pointing to the first leaf (data) tuple in the B+-tree
	that is ordered after the specified key (using prefix comparison).

	\param key	The search key.
	\return		An iterator pointing to the first leaf (data) tuple in the B+-tree
				that is ordered after key, or end() if no tuples are ordered after
				key.
	*/
	ConstIterator upper_bound(const ValueKey& key) const;

	//#################### PRIVATE METHODS ####################
private:
	/**
	Adds a branch (index) node that can be connected to the rest of the B+-tree.

	\return	The ID of the branch node.
	*/
	int add_branch_node();

	/**
	Adds an index entry for the specified node to its parent node.
	Evidently the node must have a parent for this to work.

	\param nodeID	The ID of the node for which to add an index entry.
	*/
	void add_index_entry(int nodeID);

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
	*/
	void add_root_node(const Split& split);

	/**
	Returns a tuple manipulator that can be used to interact with the B+-tree's branch (index) tuples.

	\return	The tuple manipulator.
	*/
	TupleManipulator branch_tuple_manipulator() const;

	/**
	Extracts the child node ID from a branch tuple of the form <key1,...,keyN,child node ID>.

	\param branchTuple	The tuple from which to extract the child node ID.
	\return				The child node ID from the tuple.
	*/
	int child_node_id(const BackedTuple& branchTuple) const;

	/**
	Connects a fresh node into the B+-tree as the right sibling of the specified node
	and with the same parent. Note that this function makes no attempt to update the
	parent of the two nodes, and should therefore only be used as part of a larger
	algorithm that does do so.

	\param freshID		The ID of the fresh node.
	\param existingID	The ID of the existing node whose right sibling the fresh node will become.
	*/
	void connect_node_as_right_sibling_of(int freshID, int existingID);

	/**
	Deletes the specified node from the B+-tree. Note that the caller is responsible
	for updating other nodes in the tree where necessary.

	\param nodeID	The ID of the node to delete.
	*/
	void delete_node(int nodeID);

	/**
	Disconnects the specified node from its siblings in the B+-tree. Note that this
	function makes no attempt to update the parent of the node, and should therefore
	only be used as part of a larger algorithm that does so.

	\param nodeID	The ID of the node to disconnect.
	*/
	void disconnect_node_from_siblings(int nodeID);

	/**
	Erases the index entry for the specified node from its parent node. The node
	must have a parent and must not be the parent's first child.

	\param nodeID	The ID of the node whose index entry should be erased.
	*/
	void erase_index_entry(int nodeID);

	/**
	Erases the first tuple that matches the specified key from the subtree rooted at
	the specified branch node. Other tuples that match the specified key remain in
	the subtree.

	\param key		The key denoting the tuple to erase.
	\param nodeID	The ID of the branch node at the root of the subtree from which to erase it.
	\return			The result of any merge that occurs, or boost::none otherwise.
	*/
	boost::optional<Merge> erase_tuple_from_branch(const ValueKey& key, int nodeID);

	/**
	Erases the first tuple that matches the specified key from the specified leaf node.
	Other tuples that match the specified key remain in the node.

	\param key		The key denoting the tuple to erase.
	\param nodeID	The ID of the leaf node from which to erase it.
	\return			The result of any merge that occurs, or boost::none otherwise.
	*/
	boost::optional<Merge> erase_tuple_from_leaf(const ValueKey& key, int nodeID);

	/**
	Erases the first tuple that matches the specified key from the subtree rooted at the
	specified node. Other tuples that match the specified key remain in the subtree.

	\param key		The key denoting the tuple to erase.
	\param nodeID	The ID of the node at the root of the subtree from which to erase it.
	\return			The result of any merge that occurs, or boost::none otherwise.
	*/
	boost::optional<Merge> erase_tuple_from_subtree(const ValueKey& key, int nodeID);

	/**
	Finds the index entry for the specified node in its parent node. The node must
	have a parent and must not be the parent's first child.

	\param nodeID	The ID of the node whose index entry should be found.
	\return			An iterator pointing to the index entry for the specified node in its parent.
	*/
	SortedPage::TupleSetCIter find_index_entry(int nodeID) const;

	/**
	Checks whether or not the specified node satisfies its minimum tuple invariant, possibly
	after changing its tuple count by the specified offset. For example, specifying an offset
	of -1 allows us to check whether the node will still satisfy its minimum tuple invariant
	after a tuple has been erased.

	\param nodeID	The node for which we want to check the minimum tuple invariant.
	\param offset	An optional offset to apply to the node's tuple count when performing the check.
	\return			true, if the node satisfies its minimum tuple invariant after changing
					its tuple count by the specified offset, or false otherwise.
	*/
	bool has_at_least_min_tuples(int nodeID, int offset = 0) const;

	/**
	Checks whether or not the specified node contains less than the maximum number
	of tuples that can be stored in a node.

	\param nodeID	The ID of the node to check.
	\return			true, if the specified node has less than the maximum number of
					tuples that can be stored in a node, or false otherwise.
	*/
	bool has_less_than_max_tuples(int nodeID) const;

	/**
	Inserts a tuple into the subtree rooted at the specified branch node. This may cause
	the node to be split, in which case a split result will be returned.

	\param tuple	The tuple to insert.
	\param nodeID	The ID of the branch node at the root of the subtree into which to insert it.
	\return			The result of any split that occurs, or boost::none otherwise.
	*/
	boost::optional<Split> insert_tuple_into_branch(const Tuple& tuple, int nodeID);

	/**
	Inserts a tuple into the specified leaf node. This may cause the node to be split,
	in which case a split result will be returned.

	\param tuple	The tuple to insert.
	\param nodeID	The ID of the leaf node into which to insert it.
	\return			The result of any split that occurs, or boost::none otherwise.
	*/
	boost::optional<Split> insert_tuple_into_leaf(const Tuple& tuple, int nodeID);

	/**
	Inserts a tuple into the subtree rooted at the specified node. This may cause
	the node to be split, in which case a split result will be returned.

	\param tuple	The tuple to insert.
	\param nodeID	The ID of the node at the root of the subtree into which to insert it.
	\return			The result of any split that occurs, or boost::none otherwise.
	*/
	boost::optional<Split> insert_tuple_into_subtree(const Tuple& tuple, int nodeID);

	/**
	Checks whether or not the specified sibling of the specified node is "useful" for a
	redistribution or a merge, in the sense that it both exists and has the same parent.

	\param nodeID		The ID of the node whose sibling we want to check.
	\param siblingID	The ID of the left or right sibling of the node (may be -1 if there isn't one).
	\return				true, if the specified sibling exists and has the same parent as the node, or
						false otherwise.
	*/
	bool is_useful_sibling(int nodeID, int siblingID) const;

	/**
	Returns the ID of the child to the left of the pointed-to index entry in the
	specified branch node. (If the iterator points to the end of the index entries,
	this will return the right child of the last index entry.)

	\param it			An iterator pointing either to an index entry in the branch node
						or to the end of those index entries.
	\param branchNodeID	The ID of the branch node containing the index entries.
	\return				The ID of the child to the left of the pointed-to index entry.
	*/
	int left_child_of(const SortedPage::TupleSetCIter& it, int branchNodeID) const;

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
	Merges two branch nodes together (by merging the right-hand node into the left-hand node).

	\param leftNodeID	The left-hand operand of the merge.
	\param rightNodeID	The right-operand of the merge.
	\return				The result of the merge.
	*/
	Merge merge_branches(int leftNodeID, int rightNodeID);

	/**
	Merges two leaf nodes together, erasing a tuple from one of them in the process.

	\param nodeID		The ID of the node containing the tuple to erase (must be either leftNodeID or rightNodeID).
	\param it			An iterator pointing to the tuple to erase.
	\param leftNodeID	The left-hand operand of the merge.
	\param rightNodeID	The right-hand operand of the merge.
	\return				The result of the merge.
	*/
	Merge merge_leaves_and_erase(int nodeID, const SortedPage::TupleSetCIter& it, int leftNodeID, int rightNodeID);

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
	Returns an iterator pointing to the end of the reversed set of tuples on the specified node's page.

	\param nodeID	The ID of a node in the B+-tree.
	\return			An iterator pointing to the end of the reversed set of tuples on the specified node's page.
	*/
	SortedPage::TupleSetCRIter page_rend(int nodeID) const;

	/**
	Prints the subtree rooted at the specified node to an output stream (for debugging purposes).

	\param os		The output stream.
	\param nodeID	The ID of the node whose subtree should be printed.
	\param depth	The depth of the node in the tree (starting from 0 at the overall root).
	*/
	void print_subtree(std::ostream& os, int nodeID, unsigned int depth) const;

	/**
	Pulls the index entry of the source node down from its parent into the specified
	target node, replacing the child node ID it contains with the one specified.

	\param sourceNodeID	The ID of the node whose index entry should be pulled down.
	\param targetNodeID	The ID of the node into which to pull the index entry down.
	\param childNodeID	The child node ID to use for the new index entry.
	*/
	void pull_down_index_entry(int sourceNodeID, int targetNodeID, int childNodeID);

	/**
	Moves the last tuple across from the left sibling of the specified branch node so as to restore
	the specified node's minimum tuple invariant. The left sibling must have the same parent as the
	specified branch node and a tuple to spare. Note that this function appropriately updates the
	parent of the two nodes.

	\param nodeID	The ID of the branch node whose minimum tuple invariant has been broken.
	*/
	void redistribute_from_left_branch(int nodeID);

	/**
	Erases the tuple pointed to by the iterator from the specified leaf, and then moves
	the last tuple across from its left sibling (with the same parent) to ensure that the
	minimum tuple requirement is still met. The left sibling must itself have a tuple to
	spare to ensure that moving one does not break its own tuple requirements. Note that
	this function appropriately updates the parent of the two nodes.

	\param nodeID	The ID of the node containing the tuple being erased.
	\param it		An iterator pointing to the tuple to be erased.
	*/
	void redistribute_from_left_leaf_and_erase(int nodeID, const SortedPage::TupleSetCIter& it);

	/**
	Moves the first tuple across from the right sibling of the specified branch node so as to restore
	the specified node's minimum tuple invariant. The right sibling must have the same parent as the
	specified branch node and a tuple to spare. Note that this function appropriately updates the
	parent of the two nodes.

	\param nodeID	The ID of the branch node whose minimum tuple invariant has been broken.
	*/
	void redistribute_from_right_branch(int nodeID);

	/**
	Erases the tuple pointed to by the iterator from the specified leaf, and then moves the
	first tuple across from its right sibling (with the same parent) to ensure that the
	minimum tuple requirement is still met. The right sibling must itself have a tuple to
	spare to ensure that moving one does not break its own tuple requirements. Note that
	this function appropriately updates the parent of the two nodes.

	\param nodeID	The ID of the node containing the tuple being erased.
	\param it		An iterator pointing to the tuple to be erased.
	*/
	void redistribute_from_right_leaf_and_erase(int nodeID, const SortedPage::TupleSetCIter& it);

	/**
	Moves the first tuple from a full leaf to its non-full left sibling (with the same parent)
	to make space to insert the specified tuple, and then inserts it. Note that this function
	appropriately updates the parent of the two nodes.

	\param nodeID	The ID of the full leaf node.
	\param tuple	The tuple to insert into the leaf once there is space.
	*/
	void redistribute_leaf_left_and_insert(int nodeID, const Tuple& tuple);

	/**
	In most cases, moves the last tuple from a full leaf to its non-full right sibling (with the
	same parent) to make space to insert the specified tuple, and then inserts it. However, when
	the tuple being inserted is greater than the last tuple in the full leaf, it is itself inserted
	into the right sibling, leaving the last tuple of the full leaf where it is. Note that this
	function appropriately updates the parent of the two nodes.

	\param nodeID	The ID of the full leaf node.
	\param tuple	The tuple to insert into the leaf once there is space.
	*/
	void redistribute_leaf_right_and_insert(int nodeID, const Tuple& tuple);

	/**
	Splits a full branch node into two half-full branch nodes and inserts the specified tuple.

	\param nodeID					The ID of the branch node to split.
	\param tuple					The tuple to insert.
	\return							The result of the split.
	*/
	Split split_branch_and_insert(int nodeID, const FreshTuple& tuple);

	/**
	Splits a full leaf node into two half-full leaf nodes and inserts the specified tuple.

	\param nodeID					The ID of the leaf node to split.
	\param tuple					The tuple to insert.
	\return							The result of the split.
	*/
	Split split_leaf_and_insert(int nodeID, const Tuple& tuple);

	/**
	Transfers tuples from the specified leaf node to one of its siblings (which must have
	the same parent and enough space for the extra tuples). Note that this function makes
	no attempt to update the parent of the two nodes, and should therefore only be used as
	part of a larger algorithm that does so.

	\param sourceNodeID				The ID of the node from which the tuples should be transferred.
	\param targetNodeID				The ID of the node to which the tuples should be transferred.
	\param tuples					The tuples to transfer.
	*/
	void transfer_leaf_tuples(int sourceNodeID, int targetNodeID, const std::vector<BackedTuple>& tuples);

	/**
	Transfers n tuples from the specified leaf node to its left sibling.
	Note that this function makes no attempt to update the parent of the
	two nodes, and should therefore only be used as part of a larger
	algorithm that does do so.

	\param sourceNodeID				The ID of the node from which the tuples should be transferred.
	\param n						The number of tuples to transfer.
	*/
	void transfer_leaf_tuples_left(int sourceNodeID, unsigned int n);

	/**
	Transfers n tuples from the specified leaf node to its right sibling.
	Note that this function makes no attempt to update the parent of the
	two nodes, and should therefore only be used as part of a larger
	algorithm that does do so.

	\param sourceNodeID				The ID of the node from which the tuples should be transferred.
	\param n						The number of tuples to transfer.
	*/
	void transfer_leaf_tuples_right(int sourceNodeID, unsigned int n);

	/**
	Updates the parent pointers in the children of the old parent node to
	point to the new parent node.

	\param oldParentID	The ID of the old parent node.
	\param newParentID	The ID of the new parent node.
	*/
	void update_parent_pointers(int oldParentID, int newParentID);
};

//#################### GLOBAL FUNCTIONS ####################

std::ostream& operator<<(std::ostream& os, const BTree& rhs);

//#################### TYPEDEFS ####################

typedef boost::shared_ptr<BTree> BTree_Ptr;

}

#endif
