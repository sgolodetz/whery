/**
 * whery: TextUtil.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/util/TextUtil.h"

#include <string>

namespace whery {

void write_tabbed_text(std::ostream& os, unsigned int tabCount, const std::string& text)
{
	for(unsigned int i = 0; i < tabCount; ++i)
	{
		os << '\t';
	}
	os << text << '\n';
}

}
