#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>       // for string
#include <string_view>  // for string_view

namespace test_utils
{
	void run_test(const std::string& test_name);

	std::string get_result(std::string_view file_name);

	std::string get_answer(std::string_view file_name);
}

#endif
