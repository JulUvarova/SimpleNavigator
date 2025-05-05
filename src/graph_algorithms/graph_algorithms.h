#pragma once
#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

#include "../containers/s21_containers.h"
#include "../graph/graph.h"

struct TsmResult {
  std::vector<int> vertices;  // an array with the route you are looking for
                              // (with the vertex traverse order). Instead of
                              // int* you can use std::vector<int>
  double distance = 0;        // the length of this route
};

enum class TSPAlgorithm { ACO, NEAREST_NEIGHBOR, BRUTE_FORCE };

class s21_graph_algorithms {
 public:
  static std::vector<int> DepthFirstSearch(s21_graph& graph, int start_vertex);
  static std::vector<int> BreadthFirstSearch(s21_graph& graph,
                                             int start_vertex);
  static std::pair<int, std::vector<int>> GetShortestPathBetweenVertices(
      s21_graph& graph, int vertex1, int vertex2);
  static std::vector<std::vector<int>> GetShortestPathsBetweenAllVertices(
      s21_graph& graph);
  static std::pair<int, std::vector<std::vector<int>>> GetLeastSpanningTree(
      s21_graph& graph);
  static TsmResult SolveTravelingSalesmanProblem(
      s21_graph& graph, TSPAlgorithm algorithm = TSPAlgorithm::ACO);
  static void AnalyzeTSPAlgorithms(s21_graph& graph, int iterations = 1000);

 private:
  inline static const int kIntMax = std::numeric_limits<int>::max();

  static bool CheckVertex(s21_graph& graph, int vertex);
};