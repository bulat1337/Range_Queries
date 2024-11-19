#include <gtest/gtest.h> // for Test, TestInfo (ptr only), TEST

#include <string> // for basic_string

#include "test_utils.h" // for run_test

#include "RB_Tree.h"

class RBTreeTest : public ::testing::Test
{
  protected:
    RB::Tree<int> tree;
    size_t size = 7;
    int min = 3;
    int max = 18;

    void SetUp() override
    {
        tree.insert(10);
        tree.insert(5);
        tree.insert(15);
        tree.insert(3);
        tree.insert(7);
        tree.insert(12);
        tree.insert(18);
    }
};

TEST_F(RBTreeTest, InsertTest)
{
    ASSERT_NE(tree.lower_bound(10), nullptr);

    EXPECT_EQ(tree.lower_bound(10)->value, 10);
    EXPECT_EQ(tree.lower_bound(5)->value, 5);
    EXPECT_EQ(tree.lower_bound(15)->value, 15);
}

TEST_F(RBTreeTest, LowerBoundTest)
{
    auto it = tree.lower_bound(6);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(it->value, 7);

    it = tree.lower_bound(10);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(it->value, 10);

    it = tree.lower_bound(20);
    EXPECT_EQ(it, nullptr);
}

TEST_F(RBTreeTest, UpperBoundTest)
{
    auto it = tree.upper_bound(10);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(it->value, 12);

    it = tree.upper_bound(18);
    EXPECT_EQ(it, nullptr);
}

TEST_F(RBTreeTest, DistanceTest)
{
    auto it1 = tree.lower_bound(5);
    auto it2 = tree.lower_bound(12);
    EXPECT_EQ(tree.distance(it1, it2), 3);

    EXPECT_EQ(tree.distance(it1, it1), 0);

    it1 = tree.lower_bound(12);
    it2 = tree.lower_bound(5);
    EXPECT_EQ(tree.distance(it1, it2), 0);
}

TEST_F(RBTreeTest, CopyConstructorTest)
{
    RB::Tree<int> copy_tree(tree);

    auto it = copy_tree.lower_bound(10);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(it->value, 10);
}

TEST_F(RBTreeTest, MoveConstructorTest)
{
    RB::Tree<int> moved_tree(std::move(tree));

    auto it = moved_tree.lower_bound(10);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(it->value, 10);

    EXPECT_EQ(tree.lower_bound(10), nullptr);
}

TEST_F(RBTreeTest, CopyAssignmentTest)
{
    RB::Tree<int> copy_tree;
    copy_tree = tree;

    auto it = copy_tree.lower_bound(10);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(it->value, 10);
}

TEST_F(RBTreeTest, MoveAssignmentTest)
{
    RB::Tree<int> moved_tree;
    moved_tree = std::move(tree);

    auto it = moved_tree.lower_bound(10);
    ASSERT_NE(it, nullptr);
    EXPECT_EQ(it->value, 10);

    EXPECT_EQ(tree.lower_bound(10), nullptr);
}

TEST_F(RBTreeTest, copy_ctor)
{
    RB::Tree<int> tree_copy(tree);

    auto lb = tree_copy.lower_bound(min);
    auto ub = tree_copy.upper_bound(max);

    EXPECT_EQ(tree_copy.distance(lb, ub), size);
}

TEST_F(RBTreeTest, move_ctor)
{
    RB::Tree<int> tree_copy(std::move(tree));

    auto lb = tree_copy.lower_bound(min);
    auto ub = tree_copy.upper_bound(max);

    EXPECT_EQ(tree_copy.distance(lb, ub), size);
}

TEST_F(RBTreeTest, copy_assign)
{
    RB::Tree<int> tree_copy = tree;

    auto lb = tree_copy.lower_bound(min);
    auto ub = tree_copy.upper_bound(max);

    EXPECT_EQ(tree_copy.distance(lb, ub), size);
}

TEST_F(RBTreeTest, move_assign)
{
    RB::Tree<int> tree_copy = std::move(tree);

    auto lb = tree_copy.lower_bound(min);
    auto ub = tree_copy.upper_bound(max);

    EXPECT_EQ(tree_copy.distance(lb, ub), size);
}

TEST(range_queries, basic_1)
{
    test_utils::run_test<int, test_utils::detail::Start_Wrapper>(
        "/common/basic_1");
}

TEST(range_queries, basic_2)
{
    test_utils::run_test<int, test_utils::detail::Start_Wrapper>(
        "/common/basic_2");
}

TEST(range_queries, basic_3)
{
    test_utils::run_test<int, test_utils::detail::Start_Wrapper>(
        "/common/basic_3");
}

TEST(range_queries, basic_4)
{
    test_utils::run_test<int, test_utils::detail::Start_Wrapper>(
        "/common/basic_4");
}

TEST(range_queries, basic_5)
{
    test_utils::run_test<int, test_utils::detail::Start_Wrapper>(
        "/common/basic_5");
}

// ------- ref_range_queries -------

TEST(ref_range_queries, basic_1)
{
    test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>(
        "/common/basic_1");
}

TEST(ref_range_queries, basic_2)
{
    test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>(
        "/common/basic_2");
}

TEST(ref_range_queries, basic_3)
{
    test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>(
        "/common/basic_3");
}

TEST(ref_range_queries, basic_4)
{
    test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>(
        "/common/basic_4");
}

TEST(ref_range_queries, basic_5)
{
    test_utils::run_test<int, test_utils::detail::Ref_Start_Wrapper>(
        "/common/basic_5");
}

#ifdef ENABLE_BD_TESTS

#endif
