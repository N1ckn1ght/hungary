/* Просто доработан интерфейс file input - file output, костыльный прямоугольник и костыльное нахождение максимума.
	Существует реализация алгоритма за O(n^2 * m), данный же алгоритм решает за O(max(n, m)^4), что значительно медленнее. 
	
	Ключевая идея оптимизированного алгоритма заключается в рассмотрении строк матрицы, добавляя их по одной.*/

#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <set>

using namespace std;

vector <vector <int>> initial;
vector <int> x_potential;
vector <int> y_potential;

vector <vector <int>> graph;			// graph[i] - список вершин второй доли, к которым есть ЖЁСТКИЕ рёбра из вершины i первой доли
vector <int> matches;					// вектор macthes хранит информацию о текущем паросочетании следующим образом,
										// где i - номер вершины второй доли, matches[i] есть вершина первой доли, соединённая с i
vector <char> used;

vector <char> found;
set <int> z1;
vector <char> z2;

// алгоритм куна нахождения наибольшего паросочетания в двудольном графе
bool next_generation(int node)
{
	if (!used[node])
	{
		z1.insert(node);			// мод.

		used[node] = true;
		for (size_t i = 0; i < graph[node].size(); i++)
		{
			int try_node = graph[node][i];

			z2[try_node] = true;	// мод.

			if (matches[try_node] == -1 || next_generation(matches[try_node]))
				// в случае, если данной вершине второй доли не присвоена вершина первой доли, или есть возможность рекурсивно переприсвоить
			{
				matches[try_node] = node;
				return true;
			}
		}
	}

	return false;
}

int main()
{
	ifstream in;
	ofstream out;
	in.open("./input.txt");

	int n, m, N, mn = INT_MAX, mx = INT_MIN;
	string arg;
	in >> n >> m >> arg;
	// n - количество строк (Y), m - количество столбцов (X)

	if (arg != "min" && arg != "max")
	{
		cout << "Unknown argument. Use min or max. System halted!\n";
		return 0;
	}

	// Вводимые числа должны быть строго меньше 1 млрд.
	N = max(n, m);
	initial.resize(N, vector <int>(N, 0));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			in >> initial[i][j];
			mn = min(mn, initial[i][j]);
			mx = max(mx, initial[i][j]);
		}
	}
	in.close();

	if (mn < 0)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				initial[i][j] -= mn;
			}
		}
		mx -= mn;
	}
	if (arg == "max")
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				initial[i][j] = mx - initial[i][j];
			}
		}
		for (int i = 0; i < N; i++)
		{
			for (int j = m; j < N; j++)
			{
				initial[i][j] = 0;
			}
		}
		for (int i = n; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				initial[i][j] = 0;
			}
		}
	}
	else
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = m; j < N; j++)
			{
				initial[i][j] = 1E9;
			}
		}
		for (int i = n; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				initial[i][j] = 1E9;
			}
		}
	}

	x_potential.resize(N, INT_MAX);
	y_potential.resize(N, INT_MAX);
	matches.resize(N, -1);
	found.resize(N, false);

	// изначальный расчёт массивов потенциала, потенциал есть сумма сумм значений этих массивов
	// условие: x_potential[j] + y_potential[i] <= initial[i][j]
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			y_potential[i] = min(y_potential[i], initial[i][j]);
		}
	}
	for (int j = 0; j < N; j++)
	{
		for (int i = 0; i < N; i++)
		{
			x_potential[j] = min(x_potential[j], initial[i][j] - y_potential[i]);
		}
	}

	// цикл до тех пор, пока не соберутся все n сооветствий
	int counter = 0;
	while (counter < N)
	{
		graph.clear();
		graph.resize(N, vector <int>());

		// назовём ребро (i, j) жёстким, если выполняется условие
		// x_potential[j] + y_potential[i] = initial[i][j]
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				if (x_potential[j] + y_potential[i] == initial[i][j])
				{
					graph[i].push_back(j);
				}
			}
		}

		// пытаемся через алгоритм куна увеличить мощность паросочетания не увеличивая потенциал,
		// т.е. работаем с графом с ЖЁСТКИМИ рёбрами.
		int temp_counter = counter;

		// z1, z2 списки нужны для МАГИИ, описанной ниже - нужно запомнить, какие вершины были посещены алгоритмом куна
		z1.clear();
		z2.clear();
		z2.resize(N, false);

		for (int i = 0; i < N; i++)
		{
			if (!found[i])
			{
				used.clear();
				used.resize(N, false);
				if (next_generation(i))
				{
					temp_counter++;
					found[i] = true;
				}
			}
		}

		// в случае, если произошло увеличения мощности паросочетания, продолжаем/заканчиваем цикл без изменений
		if (temp_counter > counter)
		{
			counter = temp_counter;
		}
		else
		{
			/* Иначе (здесь и далее творится настоящая магия) - увеличиваем потенциал.
				Для увеличения потенциала мы должны рассмотреть ПОСЕЩЁННЫЕ алгоритмом куна ВЕРШИНЫ ПЕРВОЙ ДОЛИ
																и НЕ ПОСЕЩЁННЫЕ алгоритмом ВЕРШИНЫ ВТОРОЙ ДОЛИ
				(которые можно получить балаьно вычеркнув посещённые).

				Эта магия и есть "вычёркивание нулей минимальным количеством прямых"!
				Среди оставшихся i и j находим дельту потенциала, равный минимуму из initial[i][j] - (y_potential[i] + x_potential[j])

				Пересчёт потенциала происходит следующим образом:
					Для всех посещённых вершин первой доли y_potential[i] += дельта,
					Для всех НЕпосещённых вершин второй доли x_potential[j] -= дельта.

				Получившиеся осевые потенциалы по прежнему остаются корректными. */
			int delta = INT_MAX;
			for (set <int>::iterator it = z1.begin(); it != z1.end(); it++)
			{
				for (int j = 0; j < N; j++)
				{
					if (!z2[j])
					{
						delta = min(delta, initial[*it][j] - y_potential[*it] - x_potential[j]);
					}
				}
			}
			for (set <int>::iterator it = z1.begin(); it != z1.end(); it++)
			{
				y_potential[*it] += delta;
			}
			for (int j = 0; j < N; j++)
			{
				if (z2[j])
				{
					x_potential[j] -= delta;
				}
			}
		}
	}

	cout << "\nCheck the \"./output.txt\" file.\n";
	for (int i = 0; i < N; i++)
	{
		cout << i << " " << matches[i] << "\n";
	}

	out.open("./output.txt");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (matches[j] == i)
			{
				out << "1 ";
			}
			else
			{
				out << "0 ";
			}
		}
		out << "\n";
	}
}