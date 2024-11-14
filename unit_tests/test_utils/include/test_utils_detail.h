#ifndef TEST_UTILS_DETAIL
#define TEST_UTILS_DETAIL

#include "RB_Tree.h"
#include "range_queries.h"
#include "ref_range_queries.h"

namespace test_utils
{

namespace detail
{

struct Start_Wrapper
{
    template <typename T>
    void operator()(std::istream &in, std::ostream &out) const
    {
        range_queries::start<T>(in, out);
    }
};

struct Ref_Start_Wrapper
{
    template <typename T>
    void operator()(std::istream &in, std::ostream &out) const
    {
        ref_range_queries::start<T>(in, out);
    }
};

template <typename T, typename Start_Wrapper>
std::string get_result(std::string_view file_name)
{
    std::ifstream test_data(file_name.data());
    if (!test_data.is_open())
    {
        LOG("Can't open {}\n", file_name);
        throw std::runtime_error("Failed to open test data file");
    }

    std::stringstream result;
    Start_Wrapper{}.template operator()<T>(test_data,
                                           result); // Specify T explicitly
    return result.str();
}

inline std::string get_answer(std::string_view file_name)
{
    std::ifstream answer_file;

    answer_file.open(std::string(file_name));

    if (!answer_file.is_open())
    {
        LOG("Can't open {}\n", file_name);
        throw;
    }

    std::string answer((std::istreambuf_iterator<char>(answer_file)),
                       std::istreambuf_iterator<char>());

    // answer.erase(std::remove(answer.begin(), answer.end(), '\n'), answer.end());

    return answer;
}

}; // namespace detail

}; // namespace test_utils

#endif // TEST_UTILS_DETAILS
