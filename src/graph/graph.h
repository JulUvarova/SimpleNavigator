#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Represents graph data as an adjacency matrix.
 */
using GraphData = std::vector<std::vector<int>>;

/**
 * @brief Enumerates the types of graphs.
 */
enum class GraphType {
  kUnweightedUndirected,  ///< Unweighted undirected graph.
  kUnweightedDirected,    ///< Unweighted directed graph.
  kWeightedUndirected,    ///< Weighted undirected graph.
  kWeigtedDirected,       ///< Weighted directed graph.
  kUndefined              ///< Undefined type.
};

/**
 * @brief A class to represent a graph.
 *
 * The graph can be loaded from a file and exported to a DOT format file.
 * It supports different types of graphs (weighted/unweighted,
 * directed/undirected).
 */
class s21_graph {
 public:
  /**
   * @brief Base constructor for s21_graph.
   */
  s21_graph() = default;

  /**
   * @brief Loads a graph from a file.
   *
   * The file format should be:
   * First line: number of vertices.
   * Second line: graph type (0: unweighted undirected, 1: unweighted directed,
   * 2: weighted undirected, 3: weighted directed). Subsequent lines: adjacency
   * matrix.
   * @param filename The path to the file containing the graph data.
   * @throw std::runtime_error if the file cannot be opened or if the file
   * format is invalid.
   */
  void LoadFromFile(std::string& filename);

  /**
   * @brief Exports the graph to a DOT format file.
   * @param filename The path to the file where the DOT representation will be
   * saved.
   * @throw std::runtime_error if the file cannot be opened.
   */
  void ExportToDot(std::string& filename);

  /**
   * @brief Prints the graph's adjacency matrix to the console.
   */
  void PrintGraph() const;

  /**
   * @brief Gets the number of vertices in the graph.
   * @return The number of vertices.
   */
  int Size() const;

  /**
   * @brief Gets the type of the graph.
   * @return The graph type.
   */
  GraphType GetType() const;

  /**
   * @brief Gets the adjacency matrix of the graph.
   * @return A constant reference to the graph data.
   */
  GraphData Get() const;

  /**
   * @brief Accesses the weight of the edge between two vertices.
   * @param i The row index (source vertex).
   * @param j The column index (destination vertex).
   * @return The weight of the edge (i, j). Returns 0 if no edge exists in
   * unweighted graphs.
   */
  const int operator()(const int i, const int j) { return graph_[i][j]; }

 private:
  GraphData graph_;  ///< The adjacency matrix representation of the graph.
  GraphType graph_type_ = GraphType::kUndefined;  ///< The type of the graph.

  /**
   * @brief Parses the graph type from the loaded data.
   * This function is typically called internally by LoadFromFile.
   * @deprecated This method seems to be intended for internal use within
   * LoadFromFile or a similar setup phase. It's not clear how it's used based
   * on the header alone.
   */
  void ParseType();
};