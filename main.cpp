#include <iostream> // for cin, cout

#include <set>

#include "range_queries.h" // for start

#include "RB_Tree.h"

int main()
{
    range_queries::start<RB::Tree<int>, int>(std::cin, std::cout);

    return 0;
}
