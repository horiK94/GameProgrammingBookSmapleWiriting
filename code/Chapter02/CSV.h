#pragma once
#include <vector>
#include <string>

class CSV
{
public:
	CSV();
	~CSV();
	static std::vector<std::vector<int>> LoadCSV(std::string fileName);
};

