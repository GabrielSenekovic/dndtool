#include <vector>

namespace gnu
{
	std::vector<int> findCommonDivisors(int a, int b)
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
}