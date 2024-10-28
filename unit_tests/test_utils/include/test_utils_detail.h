#ifndef TEST_UTILS_DETAIL
#define TEST_UTILS_DETAIL

#include "range_queries.h"
#include "RB_Tree.h"

namespace test_utils
{

namespace detail
{

template <typename T>
std::string get_result(std::string_view file_name)
{
	std::ifstream test_data;

	test_data.open(std::string(file_name));

	if (!test_data.is_open())
	{
		LOG("Can't open {}\n", file_name);
		throw;
	}

	std::stringstream result;

	range_queries::start<T>(test_data, result);

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

	return answer;
}

};

};

#endif // TEST_UTILS_DETAILS
