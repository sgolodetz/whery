/**
 * whery: BTree.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREE
#define H_WHERY_BTREE

#include "BTreeDataPage.h"

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
		/** The node's children in the B+-tree (if any). */
		std::vector<Node*> m_children;

		/** The page used to store the tuple data for the node. */
		BTreeDataPage m_page;

		/** The node's parent in the B+-tree (if any). */
		Node *m_parent;

		/** The node's left sibling in the B+-tree (if any). */
		Node *m_siblingLeft;

		/** The node's right sibling in the B+-tree (if any). */
		Node *m_siblingRight;
	};

public:
	class Iterator
	{
	private:
		Node *m_node;
		BTreeDataPage::TupleSetCIter m_it;
	public:
		// TODO
	};

	//#################### PRIVATE VARIABLES ####################
private:
	// TODO: Set of B+-tree nodes, root node.

	//#################### PUBLIC METHODS ####################
public:
	void bulk_load(const std::vector<BTreeDataPage>& pages);
	void clear();

	//#################### PRIVATE METHODS ####################
private:
	// TODO
};

}

#endif
