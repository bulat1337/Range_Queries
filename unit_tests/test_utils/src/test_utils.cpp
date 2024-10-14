#include "test_utils.h"

#include "range_queries.h"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <format>
#include <sstream>

void test_utils::run_test(const std::string& test_name)
{
	std::filesystem::path test_dir  = TEST_DATA_DIR;

	std::filesystem::path test_path = test_dir / test_name;

	test_path.replace_extension(".dat");

	std::string result = get_result(test_path.string());

	test_path.replace_extension(".ans");

	std::string answer = get_answer(test_path.string());

	EXPECT_EQ(result, answer);
}

std::string test_utils::get_result(std::string_view file_name)
{
	std::ifstream test_data;

	test_data.open(file_name);

	if (!test_data.is_open())
		throw std::runtime_error(std::format("Can't open {}\n", file_name));

	std::stringstream result;

	range_queries::start(test_data, result);

	return result.str();
}

std::string test_utils::get_answer(std::string_view file_name)
{
	std::ifstream answer_file;

	answer_file.open(file_name);

	if (!answer_file.is_open())
		throw std::runtime_error(std::format("Can't open {}\n", file_name));

	std::string answer((std::istreambuf_iterator<char>(answer_file)),
                        std::istreambuf_iterator<char>());

	return answer;
}
