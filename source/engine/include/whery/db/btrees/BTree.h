/**
 * whery: BTree.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREE
#define H_WHERY_BTREE

#include "whery/db/pages/SortedPage.h"

namespace whery {

//#################### FORWARD DECLARATIONS ####################
class BTreePageFactory;

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
		/** The node's children in the B+-tree (if any). */
		std::vector<Node*> m_children;

		/** The page used to store the tuple data for the node. */
		SortedPage_Ptr m_page;

		/** The node's parent in the B+-tree (if any). */
		Node *m_parent;

		/** The node's left sibling in the B+-tree (if any). */
		Node *m_siblingLeft;

		/** The node's right sibling in the B+-tree (if any). */
		Node *m_siblingRight;
	};

public:
	class ConstIterator
	{
	private:
		Node *m_node;
		SortedPage::TupleSetCIter m_it;
	public:
		// TODO
	};

	//#################### PRIVATE VARIABLES ####################
private:
	// TODO: Set of B+-tree nodes.

	/** The root node of the B+-tree. */
	Node *m_root;

	//#################### CONSTRUCTORS ####################
public:
	BTree(const BTreePageFactory& pageFactory);

	//#################### COPY CONSTRUCTOR & ASSIGNMENT OPERATOR ####################
private:
	/** Private and unimplemented - copying and assignment are potentially expensive for B+-trees. */
	BTree(const BTree&);
	BTree& operator=(const BTree&);

	//#################### PUBLIC METHODS ####################
public:
	ConstIterator begin() const;
	void bulk_load(const std::vector<SortedPage_Ptr>& pages);
	void clear();
	ConstIterator end() const;
	// TODO: equal_range
	void erase_tuple(const RangeKey& key);
	void erase_tuples(const ValueKey& key);
	void insert_tuple(const Tuple& tuple);
	ConstIterator lower_bound(const RangeKey& key) const;
	ConstIterator lower_bound(const ValueKey& key) const;
	unsigned int tuple_count();
	ConstIterator upper_bound(const RangeKey& key) const;
	ConstIterator upper_bound(const ValueKey& key) const;
};

}

#endif
