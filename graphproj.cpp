#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <cmath>
#include <queue>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

class City {
public:
	int id;
	double x,y;
	City(int id, double x, double y): id(id), x(x), y(y) {}

};


class Road {
public:
	int city1, city2;
	int category;

	Road(int city1, int city2, int category): city1(city1), city2(city2), category(category) {}

};


class CityGraph {
	public:
	vector<City> cities;
	vector<Road> roads;

	map<int, vector<pair<int, int>>> adjacencyList;

	const map<int, double> categorySpeeds = {
		{1, 140.0},
		{2, 120.0},
		{3, 90.0},
		{4, 50.0}
	};

	void addRoad(const Road& road){
		roads.push_back(road);
		adjacencyList[road.city1].emplace_back(road.city2, road.category);
		adjacencyList[road.city2].emplace_back(road.city1, road.category);
	}

	void addCity(const City& city){
		cities.push_back(city);
	}

	const City& findCity(int id) const {
		for (const auto& city: cities) {
			if (city.id == id) return city;
		}
		throw runtime_error("City not found");
	}

	double calcDist(int city1, int city2) const {
		const auto& c1 = findCity(city1);
		const auto& c2 = findCity(city2);
		return sqrt(pow(c1.x-c2.x, 2) + pow(c1.y - c2.y, 2));
	}
	vector<int> findPath(int start, int end, bool shortest = true) {
		map<int, double> distances;
		map<int, int> predecessors;
		priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
		for (const auto& city: cities) {
			distances[city.id] = 99999999999999999999999999999.9;
			predecessors[city.id] = -1;
		}
		distances[start] = 0.0;
		pq.emplace(0.0, start);

		while (!pq.empty()){
			auto current = pq.top();
			pq.pop();
			double currdist = current.first;
			int currcity = current.second;

			if (currcity == end) break;
			if (currdist > distances[currcity]) continue;

			for (const auto& neighbor: adjacencyList[currcity]){
				int nCity = neighbor.first;
				int roadcat = neighbor.second;

				double edgW;
				if (shortest) {
					edgW = calcDist(currcity, nCity);
				} else {
					double distance = calcDist(currcity, nCity);
					double speed = categorySpeeds.at(roadcat);
					edgW = distance/speed;
				}
				double ndist = currdist + edgW;
				if (ndist < distances[nCity]) {
					distances[nCity] = ndist;
					predecessors[nCity] = currcity;
					pq.emplace(ndist, nCity);
				}
			}
		}
		vector<int> path;
		if (predecessors[end] == -1) return path;
		for (int at = end; at != -1; at = predecessors[at]){
			path.push_back(at);
		}
		reverse(path.begin(), path.end());

		return path;
	}

	void pathPrint(const vector<int>& path, bool shortest = true){
		if (path.empty()){
			cout << "No route between cities \n";
			return;

		}
		cout << "Optimal route (" << (shortest? "shortest" : "fastest") << "):\n";
		for (size_t i = 0; i < path.size(); i++){
			if (i > 0) cout << "-> ";
			cout << path[i];
		}
		cout << "\n";

		double totalDist = 0.0;
		double totalTime = 0.0;

		for (size_t i = 0; i < path.size() - 1; i++){
			int c1 = path[i];
			int c2 = path[i+1];

			int cat = -1;
			for (const auto& neighbor: adjacencyList[c1]){
				if (neighbor.first == c2) {
					cat = neighbor.second;
					break;

				}
			}
			double distance = calcDist(c1, c2);
			double speed = categorySpeeds.at(cat);
			double time = distance / speed;

			totalDist += distance;
			totalTime += time;

		}
		cout << "Total distance: " << fixed << setprecision(2) << totalDist << "time " << totalTime << "\n";
	}
};


void loadFile(const string& filename, CityGraph& graph) {
	ifstream file(filename);
	if (!file.is_open()){
		throw runtime_error("Cannot open file: " + filename);
	}
	int city_count;
	file >> city_count;

	for (int i = 0; i < city_count; i++){
		int id;
		double x, y;
		file >> id >> x >> y;
		graph.addCity(City(id, x, y));
	}

	int road_count;
	file >> road_count;

	for (int i = 0; i < road_count; i++){
		int c1, c2, cat;
		file >> c1 >> c2 >> cat;
		graph.addRoad(Road(c1, c2, cat));
	}
}

int main() {
	try {
		CityGraph graph;
		loadFile("drogi2.dat", graph);
		int start, end;
		cout << "Start city :";
		cin >> start;
		cout << "End city :";
		cin >> end;

		char choice;
		cout << "Select route: \n";
		cout << "1 - shortest\n";
		cout << "2 - fastest\n";

		cin >> choice;
		bool shortest = (choice == '1');
		vector<int> path = graph.findPath(start, end, shortest);
		graph.pathPrint(path, shortest);


	} catch (const exception& e) {
		cerr << "Error :" << e.what() << endl;
		return 1;
	}
	return 0;
}
