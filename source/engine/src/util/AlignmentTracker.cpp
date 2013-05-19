/**
 * whery: AlignmentTracker.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/util/AlignmentTracker.h"

namespace whery {

//#################### CONSTRUCTORS ####################

AlignmentTracker::AlignmentTracker()
:	m_offset(0)
{}

//#################### PUBLIC METHODS ####################

void AlignmentTracker::advance(unsigned int n)
{
	m_offset += n;
}

void AlignmentTracker::advance_to_boundary(unsigned int alignment)
{
	if(m_offset % alignment != 0)
	{
		m_offset = (m_offset / alignment + 1) * alignment;
	}
}

unsigned int AlignmentTracker::max_alignment() const
{
	// Note: This is an egregious hack - I feel slightly unclean.
	union helper
	{
		long double ld;
		long long ll;
		void *vp;
	};

	return sizeof(helper);
}

unsigned int AlignmentTracker::offset() const
{
	return m_offset;
}

}
