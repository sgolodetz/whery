/**
 * whery: BTreePageFactory.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_BTREEPAGEFACTORY
#define H_WHERY_BTREEPAGEFACTORY

#include "whery/db/pages/SortedPage.h"

namespace whery {

/**
\brief An instance of a class deriving from this one can be used to make pages for a B+-tree.

This interface exists because the B+-tree itself should not need to care about how the pages are
constructed (page construction generally involves specifying details of how the pages are to be
persisted, and interaction with the cache).
*/
class BTreePageFactory
{
	//#################### DESTRUCTOR ####################
public:
	/**
	Destroys the page factory.
	*/
	virtual ~BTreePageFactory() {}

	//#################### PUBLIC ABSTRACT METHODS ####################
public:
	/**
	Makes a B+-tree branch (index) page.

	\return	The branch page.
	*/
	virtual SortedPage_Ptr make_branch_page() const = 0;

	/**
	Makes a B+-tree leaf (data) page.

	\return	The leaf page.
	*/
	virtual SortedPage_Ptr make_leaf_page() const = 0;
};

typedef boost::shared_ptr<const BTreePageFactory> BTreePageFactory_CPtr;

}

#endif
