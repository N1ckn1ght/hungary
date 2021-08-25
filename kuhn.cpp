#include <iostream>
#include <vector>

using namespace std;

vector <vector <int>> graph;			// graph[i] - список вершин второй доли, к которым есть рёбра из вершины i первой доли
vector <int> matches;					// вектор macthes хранит информацию о текущем паросочетании следующим образом,
										// где i - номер вершины второй доли, matches[i] есть вершина первой доли, соединённая с i
vector <char> used;

bool next_generation(int node)
{
	if (!used[node])
	{
		used[node] = true;
		for (size_t i = 0; i < graph[node].size(); i++)
		{
			int try_node = graph[node][i];
			if (matches[try_node] == -1 || next_generation(matches[try_node]))	
				// в случае, если данной вершине второй доли (try_node) не присвоена вершина первой доли
				// ИЛИ есть возможность переприсвоить той вершине первой доли другую вершину второй доли (рекурсивно)
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
	setlocale(LC_ALL, "Russian");
	int n, k;							// n - количество вершин в первой доли графа, k во второй
	int amount;							// amount - количество рёбер. задаются рёбра, из которых и можно составить паросочетание
										// рёбра будут задаваться в формате N, K и нумерваться от 0 -
										// так, например, 0 0 ведёт из вершины 0 первой доли в вершину 0 второй доли графа
	
	cin >> n >> k >> amount;
	graph.resize(n, vector <int>());
	matches.resize(n, -1);				// по умолчанию, вектор matches заполнен -1, что означает отсутствие найденных пар для вершин второй доли

	for (int i = 0; i < amount; i++)
	{
		int N, K;
		cin >> N >> K;
		if (find(graph[N].begin(), graph[N].end(), K) == graph[N].end())
		{
			graph[N].push_back(K);
		}
		else
		{
			cout << "Предупреждение: обнаружено ребро, повторяющее себя.\n";
			cout << "Т.к. по условию такое невозможно, оно не пойдёт в счётчик количества рёбер.\n";
			i--;
		}
	}

	int counter = 0;					// счётчик мощности (размера) паросочетания
	for (int i = 0; i < n; i++)
	{
		used.clear();
		used.resize(n, false);
		if (next_generation(i))			// запускаем для каждой вершины первой доли
		{
			counter++;					// в случае успешного увеличения паросочетания, увеличиваем счётчик
		}
	}

	cout << "\nМощность паросочетания: " << counter << "\n";
	for (int i = 0; i < k; i++)
	{
		if (matches[i] != -1)
		{
			cout << matches[i] << " " << i << "\n";
		}
	}
}

/* Метод оптимизации: 
	Для лучшей работы алгоритма лучше использовать next_generation() для улучшения уже имеющегося паросочетания.
	Исходное паросочетание можно найти, например, эвристикой, или жадным алгоритмом (брать ноды наименьшей степени).
*/