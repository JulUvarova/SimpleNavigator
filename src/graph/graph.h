#include <stdexcept>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

using GraphData = std::vector<std::vector<int>>;

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

 private:
  GraphData graph_;
  GraphType graph_type_;

  void ParseType();
};