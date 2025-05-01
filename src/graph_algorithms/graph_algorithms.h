#pragma once
#include <algorithm>
#include <limits>

#include "../containers/s21_containers.h"
#include "../graph/graph.h"

struct TsmResult {
  std::vector<int> vertices;  // an array with the route you are looking for
                              // (with the vertex traverse order). Instead of
                              // int* you can use std::vector<int>
  double distance;  // the length of this route
};

class s21_graph_algorithms {
 public:
  static s21::vector<int> DepthFirstSearch(s21_graph& graph, int start_vertex);
  static s21::vector<int> BreadthFirstSearch(s21_graph& graph,
                                             int start_vertex);
  static std::pair<int, s21::vector<int>> GetShortestPathBetweenVertices(
      s21_graph& graph, int vertex1, int vertex2);
  static s21::vector<s21::vector<int>> GetShortestPathsBetweenAllVertices(
      s21_graph& graph);
  static void GetLeastSpanningTree(s21_graph& graph);
  static TsmResult SolveTravelingSalesmanProblem(s21_graph& graph);
  static void AnalyzeTSPAlgorithms(s21_graph& graph);

 private:
  static bool CheckVertex(s21_graph& graph, int vertex);
};