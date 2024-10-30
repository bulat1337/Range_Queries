#ifndef RANGE_QUERIES_H
#define RANGE_QUERIES_H

#include <stddef.h>   // for size_t
#include <iostream>   // for char_traits, basic_istream, basic_ostream, oper...

#include "RB_Tree.h"  // for RB_Tree
#include "log.h"

namespace range_queries
{

template <typename T>
inline void start(std::istream& in, std::ostream& out)
{
	RB_Tree<T> tree;
	char option = 0;

	while (in >> option)
	{
		switch (option)
		{
			case 'k':
			{
				T key = 0;
				in >> key;

				tree.insert(key);

				break;
			}
			case 'q':
			{
				T left_b{};
				T right_b{};

				in >> left_b >> right_b;

				auto lower_b = tree.lower_bound(left_b);
				auto upper_b = tree.upper_bound(right_b);

				#ifdef ENABLE_LOGGING
				std::clog << lower_b << '\n';
				std::clog << upper_b << '\n';
				#endif

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

};

#endif
