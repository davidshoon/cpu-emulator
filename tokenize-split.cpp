#include <string>
#include <iostream>
#include <vector>

std::vector <std::string> split(std::string input, std::string delimiter)
{
	std::vector <std::string> tokens;

	auto start = 0U;
	auto end = input.find_first_of(delimiter);

	while (end != std::string::npos) {
		tokens.push_back(input.substr(start, end - start));
		start = end + 1;
		end = input.find_first_of(delimiter, start);
	}

	tokens.push_back(input.substr(start, end));

	return tokens;
}

int main()
{
	std::vector <std::string> code;
	std::string line;

	while (getline(std::cin, line)) {
		code.push_back(line);
	}

	for (auto l : code) {
		auto v = split(l, " ");
		for (auto t : v) {
			std::cout << t << ",";
		}
		std::cout << std::endl;
	}
}
