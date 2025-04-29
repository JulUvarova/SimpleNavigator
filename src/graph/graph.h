#include <stdexcept>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

using Graph = std::vector<std::vector<int>>;

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

  int Size() const;
  GraphType GetType() const;
  Graph Get() const;

 private:
  Graph graph_;
  GraphType graph_type_;

  void ParseType(const Graph& graph);
};