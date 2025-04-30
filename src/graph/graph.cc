#include "graph.h"

void s21_graph::LoadFromFile(std::string& filename) {
  // loading a graph from a file in the adjacency matrix format.
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::logic_error("Не удалось открыть файл!");
  }
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    s21::vector<int> row;
    std::string cell;
    while (std::getline(iss, cell, ' ')) {
      if (cell.empty()) {
        continue;
      }
      int value = std::stoi(cell);
      if (value < 0) {
        throw std::logic_error("Вес ребра не может быть отрицательным!");
      }
      row.push_back(value);
    }
    graph_.push_back(row);
  }
  file.close();

  if (graph_.size() != graph_[0].size()) {
    throw std::logic_error("Граф не является квадратной матрицей!");
  }

  ParseType();
}

void s21_graph::ExportToDot(std::string& filename) {
  //  exporting a graph to a dot file (see materials)
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file for writing: " + filename);
  }

  bool is_directed = (graph_type_ == GraphType::kUnweightedDirected ||
                      graph_type_ == GraphType::kWeigtedDirected);
  bool is_weighted = (graph_type_ == GraphType::kWeightedUndirected ||
                      graph_type_ == GraphType::kWeigtedDirected);
  std::string edge_connector = is_directed ? " -> " : " -- ";

  file << (is_directed ? "digraph" : "graph") << " G {" << std::endl;
  file << "  node [shape = circle];"
       << std::endl;  // Optional: Consistent node shape

  int size = Size();
  for (int i = 0; i < size; ++i) {
    // For undirected graphs, only iterate j from i to avoid duplicate edges
    // (like 1--2 and 2--1)
    for (int j = (is_directed ? 0 : i); j < size; ++j) {
      // Check if an edge exists
      if (graph_[i][j] > 0) {
        // Write the edge connection (e.g., "1 -> 2" or "1 -- 2")
        file << "  " << (i + 1) << edge_connector << (j + 1);
        // Add weight label if the graph is weighted
        if (is_weighted) {
          file << " [label=\"" << graph_[i][j] << "\"]";
        }
        file << ";" << std::endl;
      }
    }
  }

  file << "}" << std::endl;
  file.close();
}

int s21_graph::Size() const { return graph_.size(); }

void s21_graph::ParseType() {
  bool is_weighted = false;
  bool is_directed = false;

  for (const auto& row : graph_) {
    for (const auto& cell : row) {
      if (cell > 1) {
        is_weighted = true;
      }
    }
  }

  for (size_t i = 0; i < graph_.size(); ++i) {
    for (size_t j = 0; j < graph_[i].size(); ++j) {
      if (graph_[i][j] != graph_[j][i]) {
        is_directed = true;
      }
    }
  }

  if (is_weighted && is_directed) {
    graph_type_ = GraphType::kWeigtedDirected;
  } else if (is_weighted) {
    graph_type_ = GraphType::kWeightedUndirected;
  } else if (is_directed) {
    graph_type_ = GraphType::kUnweightedDirected;
  } else {
    graph_type_ = GraphType::kUnweightedUndirected;
  }
}

GraphType s21_graph::GetType() const { return graph_type_; }

void s21_graph::PrintGraph() const {
  std::cout << "Тип графа: ";
  switch (graph_type_) {
    case GraphType::kUnweightedUndirected:
      std::cout << "Неориентированный, невзвешенный";
      break;
    case GraphType::kUnweightedDirected:
      std::cout << "Ориентированный, невзвешенный";
      break;
    case GraphType::kWeightedUndirected:
      std::cout << "Неориентированный, взвешенный";
      break;
    case GraphType::kWeigtedDirected:
      std::cout << "Ориентированный, взвешенный";
      break;
  }
  std::cout << std::endl;
  std::cout << "Граф:" << std::endl;
  for (const auto& row : graph_) {
    for (const auto& cell : row) {
      std::cout << cell << " ";
    }
    std::cout << std::endl;
  }
}
