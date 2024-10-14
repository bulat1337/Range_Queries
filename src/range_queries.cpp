#include "range_queries.h"

#include <stddef.h>   // for size_t
#include <iostream>   // for char_traits, basic_istream, basic_ostream, oper...

#include "rb_tree.h"  // for RB_Tree

void range_queries::start(std::istream& in, std::ostream& out)
{
	RB_Tree<int> tree;
	char option = 0;

	while (in >> option)
	{
		switch (option)
		{
			case 'k':
			{
				int key = 0;
				in >> key;

				tree.insert(key);

				break;
			}
			case 'q':
			{
				int left_b  = 0;
				int right_b = 0;

				in >> left_b >> right_b;

				RB_Tree<int>::iterator lower_b = tree.lower_bound(left_b);
				RB_Tree<int>::iterator upper_b = tree.upper_bound(right_b);

				size_t distance = tree.distance(lower_b, upper_b);

				out << distance << '\n';

				break;
			}
			default:
				std::cerr   << "Invalid option.\n"
							<< "Usage:\n"
							<< "\tk key_value\n"
							<< "\tq left_boundary right_right_boundary\n";
		}
	}
}
