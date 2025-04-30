#include <stdexcept>

// #include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../containers/s21_vector.h"

using GraphData = s21::vector<s21::vector<int>>;

enum class GraphType {
  kUnweightedUndirected,
  kUnweightedDirected,
  kWeightedUndirected,
  kWeigtedDirected
};

class s21_graph {
 public:
  void LoadFromFile(std::string& filename);
  void ExportToDot(std::string& filename);
  void PrintGraph() const;

  int Size() const;
  GraphType GetType() const;
  GraphData Get() const;
  const int operator()(const int i, const int j) { return graph_[i][j]; }

 private:
  GraphData graph_;
  GraphType graph_type_;

  void ParseType();
};