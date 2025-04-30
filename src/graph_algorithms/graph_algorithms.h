#include "../graph/graph.h"
#include "../containers/s21_containers.h"

struct TsmResult {
  int* vertices;    // an array with the route you are looking for (with the
                    // vertex traverse order). Instead of int* you can use
                    // std::vector<int>
  double distance;  // the length of this route
};

class s21_graph_algorithms {
 public:
  static s21::vector<int> DepthFirstSearch(s21_graph& graph, int start_vertex);
  static void BreadthFirstSearch(s21_graph& graph, int start_vertex);
  static void GetShortestPathBetweenVertices(s21_graph& graph, int vertex1,
                                             int vertex2);
  static void GetShortestPathsBetweenAllVertices(s21_graph& graph);
  static void GetLeastSpanningTree(s21_graph& graph);
  static TsmResult SolveTravelingSalesmanProblem(s21_graph& graph);
  static void AnalyzeTSPAlgorithms(s21_graph& graph);
};