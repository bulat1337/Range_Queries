#include "range_queries.h"
#include "rb_tree.h"

int main()
{
	RB_Tree<int> tree;

	while (true)
	{
		char option = 0;

		std::cin >> option;

		switch (option)
		{
			case 'k':
			{
				int key = 0;
				std::cin >> key;

				tree.insert(key);

				break;
			}
			case 'q':
			{
				int left_b  = 0;
				int right_b = 0;

				std::cin >> left_b >> right_b;

				RB_Tree<int>::iterator lower_b = tree.lower_bound(left_b);
				RB_Tree<int>::iterator upper_b = tree.upper_bound(right_b);

				size_t distance = tree.distance(lower_b, upper_b);

				std::cout << distance << ' ';

				break;
			}
			default:
				std::cout   << "Invalid option.\n"
							<< "Usage:\n"
							<< "\tk key_value\n"
							<< "\tq left_boundary right_right_boundary\n";
		}
	}

	return 0;
}
