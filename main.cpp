#include "range_queries.h"
#include "rb_tree.h"

int main()
{
	RB_Tree<int> tree;

	tree.insert(3);
	tree.insert(7);
	tree.insert(9);
	tree.insert(94);
	tree.insert(19);
	tree.insert(46);
	tree.insert(0);
	tree.insert(-45);
	tree.insert(25);
	tree.insert(83);
	tree.insert(48);
	tree.insert(52);

	tree.dump_tree();

	return 0;
}
