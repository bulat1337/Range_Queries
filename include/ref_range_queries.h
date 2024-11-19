#ifndef REF_RANGE_QUERIES_H
#define REF_RANGE_QUERIES_H

#include <iostream> // for char_traits, basic_istream, basic_ostream, oper...
#include <set>
#include <stddef.h> // for size_t

#include "log.h"

namespace ref_range_queries
{
template <typename T> inline void start(std::istream &in, std::ostream &out)
{
    std::set<T> tree;
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
                LOG("query from {} to {}\n", left_b, right_b);

                if (left_b > right_b)
                {
                    out << 0 << ' ';
                    break;
                }

                auto lower_b = tree.lower_bound(left_b);
                auto upper_b = tree.upper_bound(right_b);

                auto distance = std::distance(lower_b, upper_b);

                out << distance << ' ';

                break;
            }
            default:
                std::cerr << "Invalid option.\n"
                          << "Usage:\n"
                          << "\tk key_value\n"
                          << "\tq left_boundary right_right_boundary\n";
        }
    }

    out << std::endl;
}

} // namespace ref_range_queries

#endif // REF_RANGE_QUERIES_H
