#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <fstream>
#include <iostream>
#include <string>      // for string
#include <string_view> // for string_view

#include "log.h"
#include "range_queries.h"
#include "test_utils_detail.h"

namespace test_utils
{

template <typename Tree, typename T>
void run_test(const std::string &test_name)
{
    std::string test_folder = "data";

    std::string test_path =
        std::string(TEST_DATA_DIR) + test_folder + test_name;

    std::string result =
        detail::get_result<Tree, T>(test_path + ".dat");
    std::string answer = detail::get_answer(test_path + ".ans");

    EXPECT_EQ(result, answer);
}

} // namespace test_utils

#endif
