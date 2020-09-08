#ifndef GABNUBUTILITIES_H
#define GABNUBUTILITIES_H
#include <vector>

namespace gnu
{
	inline std::vector<int> findCommonDivisors(int a, int b)
	{
		std::vector<int> divisors;
		for (int i = 1; i <= a && i <= b; i++)
		{
			if (a % i == 0 && b % i == 0)
			{
				divisors.push_back(i);
			}
		}
		return divisors;
	}
	inline std::wstring ConvertS2W(std::string s)
	{
		int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
		wchar_t* buffer = new wchar_t[count];
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
		std::wstring w(buffer);
		delete[] buffer;
		return w;
	}//I shamelessly stole this from OLC :3
}
#endif