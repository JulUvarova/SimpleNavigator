#include <iomanip>
#include <iostream>
#include <limits>

#include "../graph_algorithms/graph_algorithms.h"
#include "../utils/timer.h"

class CLInterface {
 public:
  void Loop();

 private:
  s21_graph graph_;
  bool is_graph_loaded_{false};

  const std::string COLOR_RESET = "\033[0m";
  const std::string COLOR_RED = "\033[31m";
  const std::string COLOR_GREEN = "\033[32m";
  const std::string COLOR_YELLOW = "\033[33m";

  void SalesmanProblemAnalysis();
  void SalesmanProblem();
  void MinimumSpanningTree();
  void ShortestPathBetweenAllVertices();
  void ShortestPathBetweenVertices();
  void DFS();
  void BFS();
  void LoadGraphFromFile();
  void PrintMenu() const;
  bool CheckGraph() const;
  bool ValidateVertex(int vertex) const;
  void PrintInformation(const std::string msg) const;
  void PrintWarning(const std::string msg) const;
  void PrintInput(const std::string msg) const;
  void CleanInput();
  void PrintMatrix(std::vector<std::vector<int>>& matrix);
};