/**
 * whery: BTreePageController.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREEPAGECONTROLLER
#define H_WHERY_BTREEPAGECONTROLLER

#include "whery/db/pages/SortedPage.h"

namespace whery {

/**
\brief An instance of a class deriving from this one controls the construction and destruction of a B+-tree's pages.

This interface exists because the B+-tree itself should not need to care about how pages are constructed and destroyed
(which generally involves specifying details of how the pages are persisted, and interaction with the cache).
*/
class BTreePageController
{
	//#################### DESTRUCTOR ####################
public:
	/**
	Destroys the page controller.
	*/
	virtual ~BTreePageController() {}

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	/**
	Returns a tuple manipulator that can be used to interact with the B+-tree's branch (index) tuples.

	\return	The tuple manipulator.
	*/
	virtual TupleManipulator btree_branch_tuple_manipulator() const = 0;

	/**
	Returns a tuple manipulator that can be used to interact with the B+-tree's leaf (data) tuples.

	\return	The tuple manipulator.
	*/
	virtual TupleManipulator btree_leaf_tuple_manipulator() const = 0;

	/**
	Makes a B+-tree branch (index) page.

	\return	The branch page.
	*/
	virtual SortedPage_Ptr make_btree_branch_page() const = 0;

	/**
	Makes a B+-tree leaf (data) page.

	\return	The leaf page.
	*/
	virtual SortedPage_Ptr make_btree_leaf_page() const = 0;
};

typedef boost::shared_ptr<const BTreePageController> BTreePageController_CPtr;

}

#endif
