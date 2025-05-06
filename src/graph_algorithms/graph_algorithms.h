#pragma once
#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

#include "../containers/s21_containers.h"
#include "../graph/graph.h"

/**
 * @brief Structure to store the result of the Traveling Salesman Problem.
 */
struct TsmResult {
  std::vector<int>
      vertices;         ///< An array with the route (vertex traverse order).
  double distance = 0;  ///< The length of this route.
};

/**
 * @brief Enumerates the algorithms available for solving the Traveling Salesman
 * Problem.
 */
enum class TSPAlgorithm {
  ACO,               ///< Ant Colony Optimization algorithm.
  NEAREST_NEIGHBOR,  ///< Nearest Neighbor algorithm.
  BRUTE_FORCE        ///< Brute Force algorithm.
};

/**
 * @brief A class containing algorithms for graph processing.
 *
 * This class provides static methods for various graph algorithms such as
 * Depth First Search, Breadth First Search, shortest path calculations,
 * finding the least spanning tree, and solving the Traveling Salesman Problem.
 */
class s21_graph_algorithms {
 public:
  /**
   * @brief Performs a Depth First Search on the graph.
   * @param graph The graph to search.
   * @param start_vertex The vertex to start the search from (1-indexed).
   * @return A vector of visited vertices in DFS order.
   * @throw std::out_of_range if start_vertex is invalid.
   */
  static std::vector<int> DepthFirstSearch(s21_graph& graph, int start_vertex);

  /**
   * @brief Performs a Breadth First Search on the graph.
   * @param graph The graph to search.
   * @param start_vertex The vertex to start the search from (1-indexed).
   * @return A vector of visited vertices in BFS order.
   * @throw std::out_of_range if start_vertex is invalid.
   */
  static std::vector<int> BreadthFirstSearch(s21_graph& graph,
                                             int start_vertex);
  /**
   * @brief Finds the shortest path between two vertices using Dijkstra's
   * algorithm.
   * @param graph The graph to search.
   * @param vertex1 The starting vertex (1-indexed).
   * @param vertex2 The ending vertex (1-indexed).
   * @return A pair containing the shortest distance and the path (vector of
   * vertices). If no path exists, distance is infinity and path is empty.
   * @throw std::out_of_range if vertex1 or vertex2 is invalid.
   */
  static std::pair<int, std::vector<int>> GetShortestPathBetweenVertices(
      s21_graph& graph, int vertex1, int vertex2);

  /**
   * @brief Finds the shortest paths between all pairs of vertices using
   * Floyd-Warshall algorithm.
   * @param graph The graph to process.
   * @return A matrix where element (i, j) is the shortest distance from vertex
   * i to vertex j. std::numeric_limits<int>::max() indicates no path.
   */
  static std::vector<std::vector<int>> GetShortestPathsBetweenAllVertices(
      s21_graph& graph);

  /**
   * @brief Finds the Least Spanning Tree of the graph using Prim's algorithm.
   * @param graph The graph to process. Must be weighted and undirected.
   * @return A pair containing the total weight of the MST and the adjacency
   * matrix of the MST. Returns {-1, {}} if the graph is not suitable (e.g.,
   * disconnected and unable to form a single tree, or not weighted undirected).
   */
  static std::pair<int, std::vector<std::vector<int>>> GetLeastSpanningTree(
      s21_graph& graph);

  /**
   * @brief Solves the Traveling Salesman Problem for the given graph.
   * @param graph The graph to solve TSP for. Must be a complete weighted graph.
   * @param algorithm The algorithm to use (ACO, Nearest Neighbor, Brute Force).
   * @return A TsmResult struct containing the best route found and its
   * distance.
   * @throw std::invalid_argument if the graph is not suitable for TSP (e.g.,
   * not complete, or has 0 vertices).
   */
  static TsmResult SolveTravelingSalesmanProblem(
      s21_graph& graph, TSPAlgorithm algorithm = TSPAlgorithm::ACO);
  // static void AnalyzeTSPAlgorithms(s21_graph& graph, int iterations = 1000);

 private:
  /**
   * @brief Represents positive infinity for integer distances.
   */
  inline static const int kIntMax = std::numeric_limits<int>::max();

  /**
   * @brief Checks if a vertex index is valid for the given graph.
   * @param graph The graph.
   * @param vertex The vertex index (1-indexed).
   * @return True if the vertex is valid, false otherwise.
   */
  static bool CheckVertex(s21_graph& graph, int vertex);
};