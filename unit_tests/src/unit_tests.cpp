#include <gtest/gtest.h>  // for Test, TestInfo (ptr only), TEST

#include <string>         // for basic_string

#include "test_utils.h"   // for run_test

TEST(range_queries, basic_1)
{
	test_utils::run_test<int, test_utils::detail::Start_Wrapper>("/common/basic_1");
}

TEST(range_queries, basic_2)
{
	test_utils::run_test<int, test_utils::detail::Start_Wrapper>("/common/basic_2");
}

TEST(range_queries, basic_3)
{
	test_utils::run_test<int, test_utils::detail::Start_Wrapper>("/common/basic_3");
}

TEST(range_queries, basic_4)
{
	test_utils::run_test<int, test_utils::detail::Start_Wrapper>("/common/basic_4");
}

TEST(range_queries, basic_5)
{
	test_utils::run_test<int, test_utils::detail::Start_Wrapper>("/common/basic_5");
}

// ------- ref_range_queries -------

TEST(ref_range_queries, basic_1)
{
	test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>("/common/basic_1");
}

TEST(ref_range_queries, basic_2)
{
	test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>("/common/basic_2");
}

TEST(ref_range_queries, basic_3)
{
	test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>("/common/basic_3");
}

TEST(ref_range_queries, basic_4)
{
	test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>("/common/basic_4");
}

TEST(ref_range_queries, basic_5)
{
	test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>("/common/basic_5");
}

#ifdef ENABLE_BD_TESTS

#endif

