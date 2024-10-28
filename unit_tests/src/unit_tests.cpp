#include <gtest/gtest.h>  // for Test, TestInfo (ptr only), TEST

#include <string>         // for basic_string

#include "test_utils.h"   // for run_test

TEST(range_queries, basic_1)
{
	test_utils::run_test<int>("/common/basic_1");
}

TEST(range_queries, basic_2)
{
	test_utils::run_test<int>("/common/basic_2");
}

#ifdef BD_TESTS


#endif

