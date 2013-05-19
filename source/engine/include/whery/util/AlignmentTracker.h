/**
 * whery: AlignmentTracker.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_ALIGNMENTTRACKER
#define H_WHERY_ALIGNMENTTRACKER

namespace whery {

/**
\brief An instance of this class is used to keep track of the current alignment
when manually laying out objects in buffers.

This is important in order to ensure that all such objects are suitably-aligned.
*/
class AlignmentTracker
{
	//#################### PRIVATE VARIABLES ####################
private:
	/** The number of bytes we have advanced from a maximum-alignment boundary. */
	unsigned int m_offset;

	//#################### CONSTRUCTORS ####################
public:
	/**
	Constructs a tracker whose current alignment is initially on
	a maximum-alignment boundary.
	*/
	AlignmentTracker();

	//#################### PUBLIC METHODS ####################
public:
	/**
	Advances the tracking offset by the specified number of bytes.

	\param n	The number of bytes by which to advance the offset.
	*/
	void advance(unsigned int n);

	/**
	Advances the tracking offset to a boundary for the specified alignment.
	If it is currently on such a boundary, the offset remains unchanged.

	\param alignment	The alignment for which we want to find a boundary.
	*/
	void advance_to_boundary(unsigned int alignment);

	/**
	Returns the maximum alignment (in bytes) required for any data type.
	*/
	unsigned int max_alignment() const;

	/**
	Returns the tracking offset.

	\return	The tracking offset.
	*/
	unsigned int offset() const;
};

}

#endif
