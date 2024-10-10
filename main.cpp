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

	tree.dump_tree();

	return 0;
}
