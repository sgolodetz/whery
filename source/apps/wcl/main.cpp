/**
 * wcl: main.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include <algorithm>
#include <iostream>
#include <utility>

#include "whery/db/DataPage.h"
#include "whery/db/DoubleFieldManipulator.h"
#include "whery/db/FieldTupleComparator.h"
#include "whery/db/IntFieldManipulator.h"
using namespace whery;

void output(const DataPage& page, std::vector<std::pair<unsigned int,SortDirection> > *fieldIndices = NULL)
{
	std::cout << page.record_count() << ' ' << page.percentage_full() << '\n';

	std::vector<Record> records = page.records();
	if(fieldIndices != NULL)
	{
		std::sort(records.begin(), records.end(), FieldTupleComparator(*fieldIndices));
	}

	std::cout << "---\n";
	for(std::vector<Record>::const_iterator it = records.begin(), iend = records.end(); it != iend; ++it)
	{
		const Record& record = *it;
		for(size_t j = 0; j < record.arity(); ++j)
		{
			std::cout << record.field(j).get_string() << '\t';
		}
		std::cout << '\n';
	}
	std::cout << "---\n\n";
}

int main()
{
	std::vector<const FieldManipulator*> fms;
	fms.push_back(&IntFieldManipulator::instance());
	fms.push_back(&DoubleFieldManipulator::instance());
	fms.push_back(&IntFieldManipulator::instance());
	DataPage page(fms);

	output(page);

	Record record = page.add_record();
	record.field(0).set_int(0);
	record.field(1).set_double(23.0);
	record.field(2).set_int(9);

	output(page);

	record = page.add_record();
	record.field(0).set_int(1);
	record.field(1).set_double(7.0);
	record.field(2).set_int(8);

	output(page);

	record = page.add_record();
	record.field(0).set_int(2);
	record.field(1).set_double(17.0);
	record.field(2).set_int(51);

	output(page);

	record = page.add_record();
	record.field(0).set_int(3);
	record.field(1).set_double(24.0);
	record.field(2).set_int(12);

	output(page);

	page.delete_record(page.records()[2]);

	output(page);

	record = page.add_record();
	record.field(0).set_int(2);
	record.field(1).set_double(17.0);
	record.field(2).set_int(51);

	output(page);

	std::vector<std::pair<unsigned int,SortDirection> > fieldIndices;
	fieldIndices.push_back(std::make_pair(0, ASC));
	output(page, &fieldIndices);

	return 0;
}
