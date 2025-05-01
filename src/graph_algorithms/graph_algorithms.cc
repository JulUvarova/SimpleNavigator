#include "graph_algorithms.h"
#include "graph_tsm_aco.h"
#include <stdexcept>

s21::vector<int> s21_graph_algorithms::DepthFirstSearch(s21_graph& graph,
                                                        int start_vertex) {
  s21::vector<int> path;
  if (!CheckVertex(graph, start_vertex)) {
    return path;  // !или кидать ошибку?
  }

  s21::stack<int> stack;
  s21::vector<bool> visited;
  for (int i = 0; i < graph.Size(); ++i) {
    visited.push_back(false);
  }

  stack.push(start_vertex);

  while (!stack.empty()) {
    int curr = stack.top();
    stack.pop();

    if (visited[curr]) continue;
    path.push_back(curr);
    visited[curr] = true;

    for (int i = graph.Size() - 1; i >= 0; --i) {
      // for (int i = 0; i < graph.Size(); ++i) {
      if (graph(curr, i) > 0 && !visited[i]) {
        stack.push(i);
      }
    }
  }
  return path;
}

s21::vector<int> s21_graph_algorithms::BreadthFirstSearch(s21_graph& graph,
                                                          int start_vertex) {
  s21::vector<int> path;
  if (!CheckVertex(graph, start_vertex)) {
    return path;  // !или кидать ошибку?
  }

  s21::queue<int> queue;
  s21::vector<bool> visited;
  for (int i = 0; i < graph.Size(); ++i) {
    visited.push_back(0);
  }

  queue.push(start_vertex);
  visited[start_vertex] = true;

  while (!queue.empty()) {
    int curr = queue.front();
    queue.pop();
    path.push_back(curr);

    for (int i = 0; i < graph.Size(); ++i) {
      if (graph(curr, i) > 0 && !visited[i]) {
        queue.push(i);
        visited[i] = true;
      }
    }
  }
  //! поверка на несвязность? По условию графы связные дб
  return path;
}

std::pair<int, s21::vector<int>>
s21_graph_algorithms::GetShortestPathBetweenVertices(s21_graph& graph,
                                                     int start, int finish) {
  s21::vector<int> path;
  if (!CheckVertex(graph, start) || !CheckVertex(graph, finish) ||
      start == finish) {
    return {-1, path};  // !или кидать ошибку?
  }

  s21::vector<int> distance;
  s21::vector<int> previous;
  s21::queue<int> queue;
  s21::vector<bool> visited;
  int max = std::numeric_limits<int>::max();
  for (int i = 0; i < graph.Size(); ++i) {
    distance.push_back(max);
    previous.push_back(-1);
    visited.push_back(false);
  }
  distance[start] = 0;
  previous[start] = start;
  queue.push(start);
  visited[start] = true;

  while (!queue.empty()) {
    int curr = queue.front();
    queue.pop();

    for (int i = 0; i < graph.Size(); ++i) {
      int weight = graph(curr, i);
      if (weight > 0) {
        if (!visited[i]) {
          queue.push(i);
          visited[i] = true;
        }
        if (distance[curr] + weight < distance[i]) {
          distance[i] = distance[curr] + weight;
          previous[i] = curr;
        }
      }
    }
  }

  if (distance[finish] == max) return {-1, path};  //! not found

  for (int i = finish; i != previous[i] && previous[i] != -1; i = previous[i]) {
    path.push_back(i);
  }
  path.push_back(start);

  std::reverse(path.begin(), path.end());

  return {distance[finish], path};
}

void s21_graph_algorithms::GetShortestPathsBetweenAllVertices(
    s21_graph& graph) {
  // searching for the shortest paths between all pairs of vertices in a
  // graph using the Floyd-Warshall algorithm. As a result, the function
  // returns the matrix of the shortest paths between all vertices of the
  // graph.
}

void s21_graph_algorithms::GetLeastSpanningTree(s21_graph& graph) {
  // searching for the minimal spanning tree in a graph using Prim's
  // algorithm. As a result, the function should return the adjacency matrix
  // for the minimal spanning tree.
}

TsmResult s21_graph_algorithms::SolveTravelingSalesmanProblem(
    s21_graph& graph) {
  if (graph.Size() <= 1) {
      // Handle trivial cases or throw an error if a tour needs >1 city
      if (graph.Size() == 1) {
           TsmResult result;
           result.vertices.push_back(0);
           result.vertices.push_back(0);
           result.distance = 0.0;
           return result;
      } else {
           throw std::invalid_argument("TSP requires at least one vertex.");
      }
  }

  try {
    // Optionally define custom parameters
    // s21_aco::AcoParams params;
    // params.num_ants = 20;
    // params.num_iterations = 200;
    // ... set other params ...
    // s21_aco::AntColonyOptimizer aco_solver(graph, params);

    // Use default parameters
    s21_aco::AntColonyOptimizer aco_solver(graph);

    // Run the algorithm
    TsmResult result = aco_solver.Run();

    // The ACO solver already adds the start node at the end.
    // The TsmResult struct expects std::vector<int>, which we are now using.
    return result;

  } catch (const std::exception& e) {
    // Rethrow or handle the exception appropriately
    // For example, log the error and return a default/error TsmResult
    std::cerr << "Error solving TSP: " << e.what() << std::endl;
    // Depending on requirements, either rethrow or return an 'error' state
    // For now, return an empty/invalid TsmResult to avoid program termination
    // throw; 
    TsmResult error_result;
    error_result.vertices = {}; // Empty vector
    error_result.distance = std::numeric_limits<double>::infinity();
    return error_result;
  }
}

void s21_graph_algorithms::AnalyzeTSPAlgorithms(s21_graph& graph) {
  // It is necessary to choose two additional algorithms to solve the
  // traveling salesman problem and implement them as methods of the
  // GraphAlgorithms class.
  // Add to the console interface the ability to perform a comparison of
  // speed of the three algorithms (the ant colony algorithm and two
  // randomly selected algorithms): The study starts for a graph that was
  // previously loaded from a file. As part of the study you need to keep
  // track of the time it took to solve the salesman's problem N times in a
  // row, by each of the algorithms. Where N is set from the keyboard. The
  // results of the time measurement must be displayed in the console.
  // Example: For N = 1000 it will measure how long it will take to solve
  // the traveling salesman problem 1000 times for the current given graph
  // by an ant colony algorithm and two randomly chosen algorithms.
}

bool s21_graph_algorithms::CheckVertex(s21_graph& graph, int vertex) {
  if (vertex < 0 || vertex >= graph.Size()) return false;
  return true;
}