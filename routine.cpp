/*
12
ADD 5 Salary
ADD 31 Walk
ADD 30 WalkPreparations
NEXT
DUMP 5
DUMP 28
NEXT
DUMP 31
DUMP 30
DUMP 28
ADD 28 Payment
DUMP 28

1 Salary
2 WalkPreparations Walk
0
0
2 WalkPreparations Walk
3 WalkPreparations Walk Payment

*/

#include "stdio.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct command
{
	string action;
	size_t day = 0;
	std::string routine;
};

void process_commands(std::vector <command> &commands)
{
	size_t month = 1;
	size_t days = 31;

	std::vector <size_t> thone{ 1, 3, 5, 7, 8, 10, 12 };
	
	if (commands.empty())
		return;

	std::vector <std::vector<string>> current_month(days);

	for (const auto & act : commands)
	{

		if (act.action == "ADD")
		{
			current_month[act.day - 1].push_back(act.routine);
		}

		if (act.action == "DUMP")
		{
			cout << current_month[act.day - 1].size() << " ";
			for (const auto &it : current_month[act.day - 1])
			{
				cout << it << " ";
			}

			cout << endl;
		}
			  
		if (act.action == "NEXT")
		{
			auto prev_days = days;
			month++;

			if (month == 2)
				days = 28;
			else if (month == 12)
			{
				current_month.clear();
				month = 1;
				current_month.resize(days);
			}
			else
			{
				auto curr_month = std::find(thone.begin(), thone.end(), month);

				if (curr_month != thone.end())
					days = 31;
				else
					days = 30;
			}

			std::vector<string> routines;
			
			if (days < prev_days)
			{
			
				for (auto i = days-1; i < prev_days; ++i)
				{
					if (!current_month[i].empty())
					{
						for (const auto &d_r : current_month[i])
						{
							routines.push_back(d_r);
						}
					}
				}
			}
			
			current_month.resize(days);

			if (days < prev_days && !routines.empty())
			{
				current_month.back().insert(current_month.back().end(), routines.begin(), routines.end());
			}
		}

	}

	return;
}


int main()
{
	size_t q = 0;
	cin >> q;
	std::vector <command> 	commands(q);

	for (auto &it : commands)
	{
		cin >> it.action;

		if (it.action == "NEXT")
			continue;
		else
		{
			cin >> it.day;
			if (it.action == "ADD" )
				cin >> it.routine;
		}
	}
	process_commands(commands);
	system("pause");
	return 0;
}
