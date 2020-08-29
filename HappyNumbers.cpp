long long int HappyNumbers(long long int N)
{
	long long int answer = 0;
	vector <vector <long long int>> matrix(10, vector <long long int>(9 * N + 1, 0));
	vector <long long int> firstLine( 9 * N + 1,0);
	firstLine[0] = 1;

	for (auto k = 0; k < N + 1; ++k)
	{
		for (auto i = 0; i < 10; i++)
		{
			for (auto j = 0; j < 9 * N + 1; ++j)
			{
				matrix[i][j+i] += firstLine[j];
			}
		}

		for (auto j = 0; j < 9 * N + 1; ++j)
		{
			for (auto i = 0; i < 10; ++i)
			{
				firstLine[j] += matrix[i][j];
			}
		}
	}


	for (auto j = 0; j < 9 * N + 1; ++j)
	{
		firstLine[j] *= firstLine[j];
		answer += firstLine[j];
	}
	return answer;
	
}