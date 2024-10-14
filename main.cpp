#include "range_queries.h"
#include "rb_tree.h"

int main()
{
	RB_Tree<int> tree;

	tree.insert(8);
	tree.insert(5);
	tree.insert(15);
	tree.insert(12);
	tree.insert(19);
	tree.insert(9);
	tree.insert(13);
	tree.insert(23);
	tree.insert(10);
	tree.insert(11);
	tree.insert(61);
	tree.insert(31);
	tree.insert(46);
	tree.insert(43);
	tree.insert(5);
	tree.insert(58);
	tree.insert(35);

	RB_Tree<int> tree_2;

	tree_2.insert(666);
	tree_2.insert(777);
	tree_2.insert(111);
	tree_2.insert(333);

	tree_2 = std::move(tree);

	tree_2.dump_tree();

	int key = 0;
	std::cin >> key;
	RB_Tree<int>::iterator lb = tree_2.lower_bound(key);
	RB_Tree<int>::iterator ub = tree_2.upper_bound(key);

	std::cout << "lower_bound of " << key << " is " << lb->value << '\n';
	std::cout << "upper_bound of " << key << " is " << ub->value << '\n';

	return 0;
}
