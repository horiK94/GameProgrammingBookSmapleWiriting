#include "CSV.h"
#include <iostream>
#include <fstream>
#include "SDL//SDL.h"
#include <sstream>

CSV::CSV()
{
}


CSV::~CSV()
{
}

std::vector<std::vector<int>> CSV::LoadCSV(std::string fileName)
{
	std::ifstream ifs(fileName);		//ファイル読み込み
	if (!ifs)
	{
		SDL_Log("filename: %s failed loading.", fileName);
		return { {} };
	}
	std::vector<std::vector<int>> csvData;

	std::string line;
	std::string num;
	int lineIndex = 0;
	while (std::getline(ifs, line))
	{
		std::stringstream ss{line};
		//csvData.emplace_back();
		csvData.push_back(std::vector<int>());
		while (std::getline(ss, num, ','))
		{
			if (num.compare("") == 0)
			{
				//"-1,"とかで終わっているときに、最後のカンマの後の""が入ってきてしまうため例外処理
				continue;
			}
			std::stringstream numss{ num };
			int value;
			numss >> value;
			csvData[lineIndex].push_back(static_cast<int>(value));
		}
		lineIndex++;
	}
	return csvData;
}
