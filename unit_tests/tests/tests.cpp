#include <gtest/gtest.h>  // for Test, TestInfo (ptr only), TEST

#include "test_utils.h"   // for run_test, Cache_type

TEST(range_queries, basic_1)
{
	test_utils::run_test("range_queries/basic_1");
}

TEST(range_queries, basic_2)
{
	test_utils::run_test("range_queries/basic_2");
}

#ifdef BD_TESTS


#endif

