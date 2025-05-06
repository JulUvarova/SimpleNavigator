#include "graph_algorithms.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "../utils/timer.h"

TEST(GraphAlgorithmsTest, WrongInputedVertices) {
  s21_graph graph;
  std::string filename = "test_graph.txt";

  {
    std::ofstream file(filename);
    file << "0 2 4 0 \n"
            "2 0 0 1 \n"
            "4 0 0 0 \n"
            "0 1 0 0 \n";
  }
  graph.LoadFromFile(filename);
  graph.PrintGraph();
  std::filesystem::remove(filename);

  auto dfs = s21_graph_algorithms::DepthFirstSearch(graph, -1);
  auto bfs = s21_graph_algorithms::BreadthFirstSearch(graph, 10);
  auto shortest_path_dijkstra =
      s21_graph_algorithms::GetShortestPathBetweenVertices(graph, 5, 5);

  EXPECT_EQ(dfs.size(), 0);
  EXPECT_EQ(bfs.size(), 0);
  EXPECT_EQ(shortest_path_dijkstra.first, -1);
  EXPECT_EQ(shortest_path_dijkstra.second.size(), 0);
}

TEST(GraphAlgorithmsTest, ConnectedDirectedWeighted) {
  s21_graph graph;
  std::string filename = "test_graph.txt";

  {
    std::ofstream file(filename);
    file << "0 5 0 0 0 0 \n"
            "0 0 3 0 0 0 \n"
            "0 0 0 0 0 4 \n"
            "2 0 0 0 7 0 \n"
            "0 0 0 0 0 1 \n"
            "0 0 0 0 0 0 \n";
  }
  graph.LoadFromFile(filename);
  graph.PrintGraph();
  std::filesystem::remove(filename);

  auto dfs = s21_graph_algorithms::DepthFirstSearch(graph, 0);
  auto bfs = s21_graph_algorithms::BreadthFirstSearch(graph, 3);
  auto shortest_path_dijkstra =
      s21_graph_algorithms::GetShortestPathBetweenVertices(graph, 3, 2);
  auto shortest_path_floyd_warshall =
      s21_graph_algorithms::GetShortestPathsBetweenAllVertices(graph);

  std::vector<int> expected_dfs{0, 1, 2, 5};
  std::vector<int> expected_bfs{3, 0, 4, 1, 5, 2};
  std::vector<int> expected_dijkstra{3, 0, 1, 2};
  std::vector<std::vector<int>> expected_floyd_warshall{
      {0, 5, 8, 0, 0, 12}, {0, 0, 3, 0, 0, 7}, {0, 0, 0, 0, 0, 4},
      {2, 7, 10, 0, 7, 8}, {0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0}};

  EXPECT_EQ(dfs.size(), 4);
  EXPECT_EQ(dfs, expected_dfs);
  EXPECT_EQ(bfs.size(), 6);
  EXPECT_EQ(bfs, expected_bfs);
  EXPECT_EQ(shortest_path_dijkstra.first, 10);
  EXPECT_EQ(shortest_path_dijkstra.second.size(), 4);
  EXPECT_EQ(shortest_path_dijkstra.second, expected_dijkstra);
  EXPECT_EQ(shortest_path_floyd_warshall, expected_floyd_warshall);
  EXPECT_THROW(s21_graph_algorithms::GetLeastSpanningTree(graph),
               std::logic_error);
}

TEST(GraphAlgorithmsTest, ConnectedUndirectedWeighted) {
  s21_graph graph;
  std::string filename = "test_graph.txt";

  {
    std::ofstream file(filename);
    file << "0  2  4  0  0  0\n"
            "2  0  0  1  0  0\n"
            "4  0  0  0  3  0\n"
            "0  1  0  0  5  7\n"
            "0  0  3  5  0  6\n"
            "0  0  0  7  6  0\n";
  }
  graph.LoadFromFile(filename);
  graph.PrintGraph();
  std::filesystem::remove(filename);

  auto dfs = s21_graph_algorithms::DepthFirstSearch(graph, 0);
  auto bfs = s21_graph_algorithms::BreadthFirstSearch(graph, 0);
  auto shortest_path_dijkstra =
      s21_graph_algorithms::GetShortestPathBetweenVertices(graph, 0, 5);
  auto shortest_path_floyd_warshall =
      s21_graph_algorithms::GetShortestPathsBetweenAllVertices(graph);
  auto prim_tree = s21_graph_algorithms::GetLeastSpanningTree(graph);

  std::vector<int> expected_dfs{0, 1, 3, 4, 2, 5};
  std::vector<int> expected_bfs{0, 1, 2, 3, 4, 5};
  std::vector<int> expected_dijkstra{0, 1, 3, 5};
  std::vector<std::vector<int>> expected_floyd_warshall{
      {0, 2, 4, 3, 7, 10}, {2, 0, 6, 1, 6, 8}, {4, 6, 0, 7, 3, 9},
      {3, 1, 7, 0, 5, 7},  {7, 6, 3, 5, 0, 6}, {10, 8, 9, 7, 6, 0}};

  EXPECT_EQ(dfs.size(), 6);
  EXPECT_EQ(dfs, expected_dfs);
  EXPECT_EQ(bfs.size(), 6);
  EXPECT_EQ(bfs, expected_bfs);
  EXPECT_EQ(shortest_path_dijkstra.first, 10);
  EXPECT_EQ(shortest_path_dijkstra.second.size(), 4);
  EXPECT_EQ(shortest_path_dijkstra.second, expected_dijkstra);
  EXPECT_EQ(shortest_path_floyd_warshall, expected_floyd_warshall);
  EXPECT_EQ(prim_tree.first, 16);
}

TEST(GraphAlgorithmsTest, ConnectedDirectedUnweighted) {
  s21_graph graph;
  std::string filename = "test_graph.txt";

  {
    std::ofstream file(filename);
    file << "0 1 1 0 0 0\n"
            "0 0 0 1 1 0\n"
            "0 0 0 0 0 1\n"
            "0 0 0 0 0 0\n"
            "0 1 0 0 0 1\n"
            "0 0 1 0 1 0\n";
  }
  graph.LoadFromFile(filename);
  graph.PrintGraph();
  std::filesystem::remove(filename);

  auto dfs = s21_graph_algorithms::DepthFirstSearch(graph, 2);
  auto bfs = s21_graph_algorithms::BreadthFirstSearch(graph, 1);
  auto shortest_path_dijkstra =
      s21_graph_algorithms::GetShortestPathBetweenVertices(graph, 0, 3);
  auto shortest_path_floyd_warshall =
      s21_graph_algorithms::GetShortestPathsBetweenAllVertices(graph);

  std::vector<int> expected_dfs{2, 5, 4, 1, 3};
  std::vector<int> expected_bfs{1, 3, 4, 5, 2};
  std::vector<int> expected_dijkstra{0, 1, 3};
  std::vector<std::vector<int>> expected_floyd_warshall{
      {0, 1, 1, 2, 2, 2}, {0, 0, 3, 1, 1, 2}, {0, 3, 0, 4, 2, 1},
      {0, 0, 0, 0, 0, 0}, {0, 1, 2, 2, 0, 1}, {0, 2, 1, 3, 1, 0}};

  EXPECT_EQ(dfs.size(), 5);
  EXPECT_EQ(dfs, expected_dfs);
  EXPECT_EQ(bfs.size(), 5);
  EXPECT_EQ(bfs, expected_bfs);
  EXPECT_EQ(shortest_path_dijkstra.first, 2);
  EXPECT_EQ(shortest_path_dijkstra.second.size(), 3);
  EXPECT_EQ(shortest_path_dijkstra.second, expected_dijkstra);
  EXPECT_EQ(shortest_path_floyd_warshall, expected_floyd_warshall);
  EXPECT_THROW(s21_graph_algorithms::GetLeastSpanningTree(graph),
               std::logic_error);
}

TEST(GraphAlgorithmsTest, ConnectedUndirectedUnweighted) {
  s21_graph graph;
  std::string filename = "test_graph.txt";

  {
    std::ofstream file(filename);
    file << "0 1 1 0 0 0\n"
            "1 0 0 1 1 0\n"
            "1 0 0 0 0 1\n"
            "0 1 0 0 0 0\n"
            "0 1 0 0 0 1\n"
            "0 0 1 0 1 0\n";
  }
  graph.LoadFromFile(filename);
  graph.PrintGraph();
  std::filesystem::remove(filename);

  auto dfs = s21_graph_algorithms::DepthFirstSearch(graph, 0);
  auto bfs = s21_graph_algorithms::BreadthFirstSearch(graph, 3);
  auto shortest_path_dijkstra =
      s21_graph_algorithms::GetShortestPathBetweenVertices(graph, 0, 5);
  auto shortest_path_floyd_warshall =
      s21_graph_algorithms::GetShortestPathsBetweenAllVertices(graph);

  std::vector<int> expected_dfs{0, 1, 3, 4, 5, 2};
  std::vector<int> expected_bfs{3, 1, 0, 4, 2, 5};
  std::vector<int> expected_dijkstra{0, 2, 5};
  std::vector<std::vector<int>> expected_floyd_warshall{
      {0, 1, 1, 2, 2, 2}, {1, 0, 2, 1, 1, 2}, {1, 2, 0, 3, 2, 1},
      {2, 1, 3, 0, 2, 3}, {2, 1, 2, 2, 0, 1}, {2, 2, 1, 3, 1, 0}};

  EXPECT_EQ(dfs.size(), 6);
  EXPECT_EQ(dfs, expected_dfs);
  EXPECT_EQ(bfs.size(), 6);
  EXPECT_EQ(bfs, expected_bfs);
  EXPECT_EQ(shortest_path_dijkstra.first, 2);
  EXPECT_EQ(shortest_path_dijkstra.second.size(), 3);
  EXPECT_EQ(shortest_path_dijkstra.second, expected_dijkstra);
  EXPECT_EQ(shortest_path_floyd_warshall, expected_floyd_warshall);
  EXPECT_THROW(s21_graph_algorithms::GetLeastSpanningTree(graph),
               std::logic_error);
}

TEST(GraphAlgorithmsTest, DisconnectedUndirectedWeighted) {
  s21_graph graph;
  std::string filename = "test_graph.txt";

  {
    std::ofstream file(filename);
    file << "0  2  4  0  0  0\n"
            "2  0  3  0  0  0\n"
            "4  3  0  0  0  0\n"
            "0  0  0  0  1  0\n"
            "0  0  0  1  0  0\n"
            "0  0  0  0  0  0\n";
  }
  graph.LoadFromFile(filename);
  graph.PrintGraph();
  std::filesystem::remove(filename);

  auto dfs = s21_graph_algorithms::DepthFirstSearch(graph, 3);
  auto bfs = s21_graph_algorithms::BreadthFirstSearch(graph, 4);
  auto shortest_path_dijkstra =
      s21_graph_algorithms::GetShortestPathBetweenVertices(graph, 0, 5);
  auto shortest_path_floyd_warshall =
      s21_graph_algorithms::GetShortestPathsBetweenAllVertices(graph);

  std::vector<int> expected_dfs{3, 4};
  std::vector<int> expected_bfs{4, 3};
  std::vector<int> expected_dijkstra{};
  std::vector<std::vector<int>> expected_floyd_warshall{
      {0, 2, 4, 0, 0, 0}, {2, 0, 3, 0, 0, 0}, {4, 3, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 0}, {0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0}};

  EXPECT_EQ(dfs.size(), 2);
  EXPECT_EQ(dfs, expected_dfs);
  EXPECT_EQ(bfs.size(), 2);
  EXPECT_EQ(bfs, expected_bfs);
  EXPECT_EQ(shortest_path_dijkstra.first, -1);
  EXPECT_EQ(shortest_path_dijkstra.second.size(), 0);
  EXPECT_EQ(shortest_path_dijkstra.second, expected_dijkstra);
  EXPECT_EQ(shortest_path_floyd_warshall, expected_floyd_warshall);
  EXPECT_THROW(s21_graph_algorithms::GetLeastSpanningTree(graph),
               std::logic_error);
}

// TSP tests
TEST(TSPTest, ACO_incorrect_algorithm) {
  s21_graph graph;
  std::string filename = "test_graph.txt";
  {
    std::ofstream file(filename);
    file << "0 1 1 0 0 0\n"
            "1 0 1 1 0 0\n"
            "1 1 0 1 0 0\n"
            "0 1 1 0 1 0\n"
            "0 0 0 1 0 1\n"
            "0 0 0 0 1 0\n";
  }
  graph.LoadFromFile(filename);
  std::filesystem::remove(filename);

  auto result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
      graph, static_cast<TSPAlgorithm>(4));
  EXPECT_EQ(result.distance, std::numeric_limits<double>::infinity());
  EXPECT_EQ(result.vertices.size(), 0);
}

// ACO for trivial graph
TEST(TSPTest, ACO_trivial_graph) {
  s21_graph graph;
  std::string filename = "test_graph.txt";
  {
    std::ofstream file(filename);
    file << "0\n";
  }
  graph.LoadFromFile(filename);
  std::filesystem::remove(filename);

  auto result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
      graph, TSPAlgorithm::ACO);
  EXPECT_EQ(result.distance, 0);
  EXPECT_EQ(result.vertices.size(), 2);
}

// ACO for non-connected graph
TEST(TSPTest, ACO_non_connected_graph) {
  s21_graph graph;
  std::string filename = "test_graph.txt";
  {
    std::ofstream file(filename);
    file << "0 1 1 0 0 0\n"
            "1 0 1 1 0 0\n"
            "1 1 0 1 0 0\n"
            "0 1 1 0 1 0\n"
            "0 0 0 1 0 1\n"
            "0 0 0 0 1 0\n";
  }
  graph.LoadFromFile(filename);
  std::filesystem::remove(filename);

  auto result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
      graph, TSPAlgorithm::ACO);
  EXPECT_EQ(result.distance, std::numeric_limits<double>::infinity());
  EXPECT_EQ(result.vertices.size(), 0);
}

TEST(TSPTest, ACO_connected_graph) {
  s21_graph graph;
  std::string filename = "test_graph.txt";
  {
    std::ofstream file(filename);
    file << "0 1 2\n"
            "1 0 2\n"
            "2 2 0\n";
  }
  graph.LoadFromFile(filename);
  std::filesystem::remove(filename);

  auto result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
      graph, TSPAlgorithm::ACO);
  EXPECT_EQ(result.distance, 5);
  EXPECT_EQ(result.vertices.size(), 4);
}

TEST(TSPTest, NN_connected_graph) {
  s21_graph graph;
  std::string filename = "test_graph.txt";
  {
    std::ofstream file(filename);
    file << "0 1 2\n"
            "1 0 2\n"
            "2 2 0\n";
  }
  graph.LoadFromFile(filename);
  std::filesystem::remove(filename);

  auto result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
      graph, TSPAlgorithm::NEAREST_NEIGHBOR);
  EXPECT_EQ(result.distance, 5);
  EXPECT_EQ(result.vertices.size(), 4);
}

TEST(TSPTest, BF_connected_graph) {
  s21_graph graph;
  std::string filename = "test_graph.txt";
  {
    std::ofstream file(filename);
    file << "0 1 2\n"
            "1 0 2\n"
            "2 2 0\n";
  }
  graph.LoadFromFile(filename);
  std::filesystem::remove(filename);

  auto result = s21_graph_algorithms::SolveTravelingSalesmanProblem(
      graph, TSPAlgorithm::BRUTE_FORCE);
  EXPECT_EQ(result.distance, 5);
  EXPECT_EQ(result.vertices.size(), 4);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
