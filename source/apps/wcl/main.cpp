/**
 * wcl: main.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <algorithm>
#include <iostream>
#include <utility>

#include <boost/utility.hpp>

#include "whery/db/base/DoubleFieldManipulator.h"
#include "whery/db/base/FreshTuple.h"
#include "whery/db/base/IntFieldManipulator.h"
#include "whery/db/base/TupleComparator.h"
#include "whery/db/pages/SortedPage.h"
using namespace whery;

void output(const SortedPage& page, std::vector<std::pair<unsigned int,SortDirection> > *fieldIndices = NULL)
{
	std::cout << page.tuple_count() << ' ' << page.percentage_full() << '\n';

	std::vector<BackedTuple> tuples(page.begin(), page.end());
	if(fieldIndices != NULL)
	{
		std::sort(tuples.begin(), tuples.end(), TupleComparator(*fieldIndices));
	}

	std::cout << "---\n";
	for(std::vector<BackedTuple>::const_iterator it = tuples.begin(), iend = tuples.end(); it != iend; ++it)
	{
		const Tuple& tuple = *it;
		for(size_t j = 0; j < tuple.arity(); ++j)
		{
			std::cout << tuple.field(j).get_string() << '\t';
		}
		std::cout << '\n';
	}
	std::cout << "---\n\n";
}

int main()
try
{
	const int PAGE_BUFFER_SIZE = 1024;

	std::vector<const FieldManipulator*> fms;
	fms.push_back(&IntFieldManipulator::instance());
	fms.push_back(&DoubleFieldManipulator::instance());
	fms.push_back(&IntFieldManipulator::instance());
	SortedPage page(fms, PAGE_BUFFER_SIZE);

	output(page);

	FreshTuple tuple(page.field_manipulators());
	tuple.field(0).set_int(0);
	tuple.field(1).set_double(23.0);
	tuple.field(2).set_int(9);
	page.add_tuple(tuple);

	output(page);

	tuple.field(0).set_int(1);
	tuple.field(1).set_double(7.0);
	tuple.field(2).set_int(8);
	page.add_tuple(tuple);

	output(page);

	tuple.field(0).set_int(2);
	tuple.field(1).set_double(17.0);
	tuple.field(2).set_int(10);
	page.add_tuple(tuple);

	output(page);

	tuple.field(0).set_int(3);
	tuple.field(1).set_double(24.0);
	tuple.field(2).set_int(12);
	page.add_tuple(tuple);

	output(page);

	page.delete_tuple(*boost::next(page.begin(), 2));

	output(page);

	tuple.field(0).set_int(2);
	tuple.field(1).set_double(17.0);
	tuple.field(2).set_int(51);
	page.add_tuple(tuple);

	output(page);

	std::vector<std::pair<unsigned int,SortDirection> > fieldIndices;
	fieldIndices.push_back(std::make_pair(1, DESC));
	output(page, &fieldIndices);

	return 0;
}
catch(std::exception& e)
{
	std::cout << "ERROR: " << e.what() << '\n';
}
