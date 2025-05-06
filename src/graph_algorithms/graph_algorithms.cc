#include "graph_algorithms.h"

#include <iomanip>
#include <stdexcept>

#include "../utils/timer.h"
#include "graph_tsp_aco.h"
#include "graph_tsp_bf.h"
#include "graph_tsp_nn.h"

std::vector<int> s21_graph_algorithms::DepthFirstSearch(s21_graph& graph,
                                                        int start_vertex) {
  std::vector<int> path;
  if (!CheckVertex(graph, start_vertex)) {
    return path;
  }
  s21::stack<int> stack;
  std::vector<bool> visited(graph.Size(), false);

  stack.push(start_vertex);

  while (!stack.empty()) {
    int curr = stack.top();
    stack.pop();

    if (visited[curr]) continue;
    path.push_back(curr);
    visited[curr] = true;

    for (int i = graph.Size() - 1; i >= 0; --i) {
      if (graph(curr, i) > 0 && !visited[i]) {
        stack.push(i);
      }
    }
  }
  return path;
}

std::vector<int> s21_graph_algorithms::BreadthFirstSearch(s21_graph& graph,
                                                          int start_vertex) {
  std::vector<int> path;
  if (!CheckVertex(graph, start_vertex)) {
    return path;
  }
  s21::queue<int> queue;
  std::vector<bool> visited(graph.Size(), 0);

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
  return path;
}

std::pair<int, std::vector<int>>
s21_graph_algorithms::GetShortestPathBetweenVertices(s21_graph& graph,
                                                     int start, int finish) {
  std::vector<int> path;
  if (!CheckVertex(graph, start) || !CheckVertex(graph, finish) ||
      start == finish) {
    return {-1, path};
  }

  std::vector<int> distance(graph.Size(), kIntMax);
  std::vector<int> previous(graph.Size(), -1);
  std::vector<bool> visited(graph.Size(), false);
  s21::queue<int> queue;

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

  if (distance[finish] == kIntMax) return {-1, path};  // not found

  for (int i = finish; i != previous[i] && previous[i] != -1; i = previous[i]) {
    path.push_back(i);
  }
  path.push_back(start);

  std::reverse(path.begin(), path.end());

  return {distance[finish], path};
}

std::vector<std::vector<int>>
s21_graph_algorithms::GetShortestPathsBetweenAllVertices(s21_graph& graph) {
  std::vector<std::vector<int>> distances;
  for (int i = 0; i < graph.Size(); ++i) {
    distances.push_back(std::vector<int>());
    for (int j = 0; j < graph.Size(); ++j) {
      if (i == j)
        distances[i].push_back(0);
      else if (graph(i, j) == 0)
        distances[i].push_back(kIntMax);
      else
        distances[i].push_back(graph(i, j));
    }
  }

  for (int k = 0; k < graph.Size(); ++k) {
    for (int i = 0; i < graph.Size(); ++i) {
      for (int j = 0; j < graph.Size(); ++j) {
        if (distances[i][k] < kIntMax && distances[k][j] < kIntMax) {
          distances[i][j] =
              std::min(distances[i][j], distances[i][k] + distances[k][j]);
        }
      }
    }
  }

  for (int i = 0; i < graph.Size(); ++i) {
    for (int j = 0; j < graph.Size(); ++j) {
      if (distances[i][j] == kIntMax) distances[i][j] = 0;
    }
  }

  return distances;
}

std::pair<int, std::vector<std::vector<int>>>
s21_graph_algorithms::GetLeastSpanningTree(s21_graph& graph) {
  if (graph.GetType() != GraphType::kWeightedUndirected ||
      BreadthFirstSearch(graph, 0).size() != graph.Size()) {
    throw std::invalid_argument(
        "Алгоритм Прима применим только к связанным взвешенным "
        "неориентированным "
        "графам!");
  }
  std::vector<std::vector<int>> res(graph.Size(),
                                    std::vector<int>(graph.Size(), 0));
  if (graph.Size() == 0) return {0, res};

  std::vector<int> key(graph.Size(), kIntMax);
  std::vector<bool> visited(graph.Size(), false);  // добавлено в дерево
  std::vector<int> parents(graph.Size(), -1);      // с кем связаны вершины

  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      edges_heap;  // ребро, вершина

  // начинаю с вершины 0
  key[0] = 0;
  edges_heap.push(std::make_pair(0, 0));

  int weight_sum = 0;

  while (!edges_heap.empty()) {
    auto curr = edges_heap.top();
    edges_heap.pop();

    if (visited[curr.second]) continue;

    visited[curr.second] = true;
    weight_sum += curr.first;
    for (int i = 0; i < graph.Size(); ++i) {
      int edge = graph(curr.second, i);
      if (edge != 0 && !visited[i] && edge < key[i]) {
        key[i] = edge;
        parents[i] = curr.second;
        edges_heap.push(std::make_pair(edge, i));
      }
    }
  }

  for (int i = 0; i < parents.size(); ++i) {
    if (parents[i] != -1) {
      res[i][parents[i]] = graph(i, parents[i]);
      res[parents[i]][i] = res[i][parents[i]];
    }
  }

  return {weight_sum, res};
}

TsmResult s21_graph_algorithms::SolveTravelingSalesmanProblem(
    s21_graph& graph, TSPAlgorithm algorithm) {
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
    switch (algorithm) {
      case TSPAlgorithm::ACO: {
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
        return result;
        break;
      }

      case TSPAlgorithm::NEAREST_NEIGHBOR: {
        s21_nn::NearestNeighborSolver nn_solver(graph);
        TsmResult result = nn_solver.Run();
        return result;
        break;
      }

      case TSPAlgorithm::BRUTE_FORCE: {
        s21_bf::BruteForceOptimizer bf_solver(graph);
        TsmResult result = bf_solver.Run();
        return result;
        break;
      }

      default:
        throw std::invalid_argument("Invalid TSP algorithm specified.");
    }

  } catch (const std::exception& e) {
    // Rethrow or handle the exception appropriately
    // For example, log the error and return a default/error TsmResult
    std::cerr << "Error solving TSP: " << e.what() << std::endl;
    // Depending on requirements, either rethrow or return an 'error' state
    // For now, return an empty/invalid TsmResult to avoid program termination
    // throw;
    TsmResult error_result;
    error_result.vertices = {};  // Empty vector
    error_result.distance = std::numeric_limits<double>::infinity();
    return error_result;
  }
}

// void s21_graph_algorithms::AnalyzeTSPAlgorithms(s21_graph& graph,
//                                                  int iterations) {

// }

bool s21_graph_algorithms::CheckVertex(s21_graph& graph, int vertex) {
  if (vertex < 0 || vertex >= graph.Size()) return false;
  return true;
}