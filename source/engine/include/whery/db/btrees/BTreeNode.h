/**
 * whery: BTreeNode.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREENODE
#define H_WHERY_BTREENODE

#include "whery/db/entities/Entity.h"
#include "whery/db/pages/SortedPage.h"

namespace whery {

/**
\brief An instance of this class represents a node in a B+-tree.
*/
class BTreeNode : public Entity
{
	//#################### PRIVATE VARIABLES ####################
private:
	/**
	The ID of the node's first child, if it has one. The IDs of any
	other children are stored in the tuples on the data page.
	*/
	uuid firstChildID;

	/** The page used to store the tuple data for the node. */
	SortedPage_Ptr page;

	/** The ID of the node's parent in the B+-tree (if any). */
	uuid parentID;

	/** The ID of the node's left sibling in the B+-tree (if any). */
	uuid siblingLeftID;

	/** The ID of the node's right sibling in the B+-tree (if any). */
	uuid siblingRightID;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a B+-tree node from the specified path on disk.

	\param p	The path to a file containing the information for a B+-tree node.
	*/
	explicit BTreeNode(const boost::filesystem::path& p);

	//#################### PUBLIC INHERITED METHODS ####################
public:
	virtual void save(const boost::filesystem::path& p) const;

	//#################### PUBLIC METHODS ####################
public:
	/**
	Returns whether or not the node has any children.

	\return	true, if the node has children, or false otherwise.
	*/
	bool has_children() const;
};

}

#endif
