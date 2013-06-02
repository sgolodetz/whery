/**
 * whery: IDAllocator.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_IDALLOCATOR
#define H_WHERY_IDALLOCATOR

#include <set>

namespace whery {

/**
\brief An instance of this class is used to manage the allocation of unique integer IDs.

This is useful in a variety of contexts, e.g. allocating IDs to pages in a cache.
*/
class IDAllocator
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** A free list of IDs that have been deallocated - these can be reallocated by allocate(). */
	std::set<int> m_free;

	/** The set of IDs that are currently in use. */
	std::set<int> m_used;

	//#################### PUBLIC METHODS ####################
public:
	/**
	Allocates a fresh ID.

	\return	The ID allocated.
	*/
	int allocate();

	/**
	Deallocates the specified ID.

	\param n						The ID to deallocate.
	\throw std::invalid_argument	If the specified ID is not currently in use.
	*/
	void deallocate(int n);

	/**
	Resets the ID allocator (equivalent to deallocating all allocated IDs).
	*/
	void reset();

	//#################### PRIVATE METHODS ####################
private:
	/**
	Gets the maximum ID currently in use.

	\return	The maximum ID currently in use, if any, or -1 otherwise.
	*/
	int max_used() const;
};

}

#endif
