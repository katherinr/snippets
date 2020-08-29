#include "stdio.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cctype>
#include <locale>         // std::locale, std::tolowe
//������������������ ��������� ���� 
template <typename T1, typename T2>
struct less_letter
{
	typedef std::pair<T1, T2> type;
	bool operator ()(type const& a, type const& b) const
	{
		auto & a_it = a.first.begin();
		auto & b_it = b.first.begin();
		bool sign = false;

		if (a.second > b.second)
		{
			sign = true;
		}
		else if (a.second < b.second)
		{
			sign = false;
		}
		else
		{
			for (; a_it != a.first.end(), b_it != b.first.end();)
			{
				if (char(*a_it) == char(*b_it))
				{
					a_it++;
					b_it++;
				}
				else
				{
					if (char(*a_it) > char(*b_it))
					{
						sign = false;
					}
					else if (char(*a_it) < char(*b_it))
					{
						sign = true;
					}

					break;
				}
			}
		}

		return sign;
	}
};

///******************************************************************************************************
int main(int argc, char* argv[])
{
	char *my_input = nullptr, *my_output = nullptr;
	//setlocale(LC_ALL, "ru_RU.UTF8");
	//���������� ���������� ��������� ������
	if (argc == 0)
	{
	//	std::cerr << "No arguments in input!" << std::endl;
		return 0;
	}
	else if (argc == 1)
	{
		//������ ����� ����� � ������
		my_input = "in.txt";
		my_output = "out.txt";
	}
	else
	{
		//������� �������� ������ ����� � ������
		my_input = argv[1];
		my_output = argv[2];
	}
	
	///******************************************************************************************************
	//���������� �� �����
	
	std::wifstream fin(my_input); // ������� ���� ��� ������
	std::unordered_map <std::wstring, int> raw_input; //��������� � ��������� ��������� �����
	std::wstring buf_str;
	std::locale loc("ru_RU.UTF8");// ("en_US.UTF8");
	//loc.all();// = "ru_RU.UTF8";

	while (getline(fin, buf_str, L' '))
	{
		for (rsize_t i = 0; i < buf_str.size(); ++i)
		{
			if (isalpha(buf_str[i],loc))
			{
				if (isupper(buf_str[i],loc))
					buf_str[i] = std::tolower(buf_str[i],loc);
			}
			else 
			{
				//�������� ������ ��������
				buf_str.erase(buf_str.begin() + i);
			}
		}
	
		auto & found_str = raw_input.find(buf_str);

		//��� ���� ����� �����, ����� �� ��������� � ���������, ����������� ������� ����� �����
		if (found_str != raw_input.end())
		{
			found_str->second += 1;
		}
		else
		{
			raw_input.insert(std::make_pair(buf_str, 1));
		}
	}

	fin.close(); // ��������� ����

	///******************************************************************************************************
	//���������� �� ������� ����������
	std::vector<  std::pair<  std::wstring, int> > raw_output(raw_input.begin(), raw_input.end());
	std::sort(raw_output.begin(), raw_output.end(), less_letter<std::wstring, int>());

	///******************************************************************************************************
	//������ ����� ������� � ����
	
	std::wofstream output(my_output);

	for (const auto &iter : raw_output)
	{
		std::wstring buf_str = std::to_wstring(iter.second) + L' ' + iter.first;
		output << buf_str << std::endl;
	}
	output.close();
}



