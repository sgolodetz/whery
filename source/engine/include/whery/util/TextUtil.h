/**
 * whery: TextUtil.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_TEXTUTIL
#define H_WHERY_TEXTUTIL

#include <ostream>

namespace whery {

//#################### GLOBAL FUNCTIONS ####################

/**
Writes some text to the specified output stream, prefixed by the specified number of tabs.

\param os		The output stream.
\param tabCount	The number of tabs with which to prefix the text.
\param text		The text to write.
*/
void write_tabbed_text(std::ostream& os, unsigned int tabCount, const std::string& text);

}

#endif
