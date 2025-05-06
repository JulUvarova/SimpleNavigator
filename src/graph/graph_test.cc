#include "graph.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

TEST(GraphTest, LoadEmptyFileThrowsException) {
  s21_graph graph;
  std::string filename = "empty_test.txt";
  { std::ofstream file(filename); }

  EXPECT_THROW(
      {
        try {
          graph.LoadFromFile(filename);
        } catch (const std::logic_error& e) {
          EXPECT_STREQ(e.what(),
                       "Файл пуст или содержит только пустые строки!");
          throw;
        }
      },
      std::logic_error);

  std::filesystem::remove(filename);
}

TEST(GraphTest, LoadNonExistentFileThrowsException) {
  s21_graph graph;

  EXPECT_THROW(
      {
        try {
          std::string fn = "non_existent_file.txt";
          graph.LoadFromFile(fn);
        } catch (const std::logic_error& e) {
          EXPECT_STREQ(e.what(), "Не удалось открыть файл!");
          throw;
        }
      },
      std::logic_error);
}

TEST(GraphTest, LoadNonSquareMatrixThrowsException) {
  s21_graph graph;
  std::string filename = "non_square_matrix.txt";

  {
    std::ofstream file(filename);
    file << "1 2 3\n";  // 3
    file << "4 5\n";    // 2
  }

  EXPECT_THROW(
      {
        try {
          graph.LoadFromFile(filename);
        } catch (const std::logic_error& e) {
          EXPECT_STREQ(e.what(), "Граф не является квадратной матрицей!");
          throw;
        }
      },
      std::logic_error);

  std::filesystem::remove(filename);
}

TEST(GraphTest, LoadMatrixWithNegativeEdgeThrowsException) {
  s21_graph graph;
  std::string filename = "non_square_matrix.txt";

  {
    std::ofstream file(filename);
    file << "1 -2\n";
    file << "4 5\n";
  }

  EXPECT_THROW(
      {
        try {
          graph.LoadFromFile(filename);
        } catch (const std::logic_error& e) {
          EXPECT_STREQ(e.what(), "Вес ребра не может быть отрицательным!");
          throw;
        }
      },
      std::logic_error);

  std::filesystem::remove(filename);
}

TEST(GraphTest, ExportToDotUnweightedUndirectedGraph) {
  s21_graph graph;
  std::string input_filename = "unweighted_undirected.txt";
  std::string output_filename = "unweighted_undirected.dot";

  // Create a simple unweighted, undirected graph
  {
    std::ofstream file(input_filename);
    file << "0 1 1\n"
         << "1 0 1\n"
         << "1 1 0\n";
  }

  // Load and export
  graph.LoadFromFile(input_filename);
  graph.ExportToDot(output_filename);

  // Check if the dot file exists
  EXPECT_TRUE(std::filesystem::exists(output_filename));

  // Verify the content
  std::string content;
  {
    std::ifstream dot_file(output_filename);
    content = std::string((std::istreambuf_iterator<char>(dot_file)),
                          std::istreambuf_iterator<char>());
  }  // File is closed here when the ifstream goes out of scope

  // Check for the expected content
  EXPECT_TRUE(content.find("graph G {") != std::string::npos);
  EXPECT_TRUE(content.find("node [shape = circle]") != std::string::npos);
  EXPECT_TRUE(content.find("1 -- 2") != std::string::npos);
  EXPECT_TRUE(content.find("1 -- 3") != std::string::npos);
  EXPECT_TRUE(content.find("2 -- 3") != std::string::npos);

  // Cleanup
  std::filesystem::remove(input_filename);
  std::filesystem::remove(output_filename);
}

TEST(GraphTest, ExportToDotWeightedUndirectedGraph) {
  s21_graph graph;
  std::string input_filename = "weighted_undirected.txt";
  std::string output_filename = "weighted_undirected.dot";

  // Create a weighted, undirected graph
  {
    std::ofstream file(input_filename);
    file << "0 2 3\n"
         << "2 0 4\n"
         << "3 4 0\n";
  }

  // Load and export
  graph.LoadFromFile(input_filename);
  graph.ExportToDot(output_filename);

  // Check if the dot file exists
  EXPECT_TRUE(std::filesystem::exists(output_filename));

  // Verify the content
  std::string content;
  {
    std::ifstream dot_file(output_filename);
    content = std::string((std::istreambuf_iterator<char>(dot_file)),
                          std::istreambuf_iterator<char>());
  }  // File is closed here when the ifstream goes out of scope

  // Check for the expected content
  EXPECT_TRUE(content.find("graph G {") != std::string::npos);
  EXPECT_TRUE(content.find("1 -- 2 [label=\"2\"]") != std::string::npos);
  EXPECT_TRUE(content.find("1 -- 3 [label=\"3\"]") != std::string::npos);
  EXPECT_TRUE(content.find("2 -- 3 [label=\"4\"]") != std::string::npos);

  // Cleanup
  std::filesystem::remove(input_filename);
  std::filesystem::remove(output_filename);
}

TEST(GraphTest, ExportToDotUnweightedDirectedGraph) {
  s21_graph graph;
  std::string input_filename = "unweighted_directed.txt";
  std::string output_filename = "unweighted_directed.dot";

  // Create an unweighted, directed graph
  {
    std::ofstream file(input_filename);
    file << "0 1 0\n"
         << "0 0 1\n"
         << "1 0 0\n";
  }

  // Load and export
  graph.LoadFromFile(input_filename);
  graph.ExportToDot(output_filename);

  // Check if the dot file exists
  EXPECT_TRUE(std::filesystem::exists(output_filename));

  // Verify the content
  std::string content;
  {
    std::ifstream dot_file(output_filename);
    content = std::string((std::istreambuf_iterator<char>(dot_file)),
                          std::istreambuf_iterator<char>());
  }  // File is closed here when the ifstream goes out of scope

  // Check for the expected content
  EXPECT_TRUE(content.find("digraph G {") != std::string::npos);
  EXPECT_TRUE(content.find("1 -> 2") != std::string::npos);
  EXPECT_TRUE(content.find("2 -> 3") != std::string::npos);
  EXPECT_TRUE(content.find("3 -> 1") != std::string::npos);

  // Cleanup
  std::filesystem::remove(input_filename);
  std::filesystem::remove(output_filename);
}

TEST(GraphTest, ExportToDotWeightedDirectedGraph) {
  s21_graph graph;
  std::string input_filename = "weighted_directed.txt";
  std::string output_filename = "weighted_directed.dot";

  // Create a weighted, directed graph
  {
    std::ofstream file(input_filename);
    file << "0 5 0\n"
         << "0 0 2\n"
         << "7 0 0\n";
  }

  // Load and export
  graph.LoadFromFile(input_filename);
  graph.ExportToDot(output_filename);

  // Check if the dot file exists
  EXPECT_TRUE(std::filesystem::exists(output_filename));

  // Verify the content
  std::string content;
  {
    std::ifstream dot_file(output_filename);
    content = std::string((std::istreambuf_iterator<char>(dot_file)),
                          std::istreambuf_iterator<char>());
  }  // File is closed here when the ifstream goes out of scope

  // Check for the expected content
  EXPECT_TRUE(content.find("digraph G {") != std::string::npos);
  EXPECT_TRUE(content.find("1 -> 2 [label=\"5\"]") != std::string::npos);
  EXPECT_TRUE(content.find("2 -> 3 [label=\"2\"]") != std::string::npos);
  EXPECT_TRUE(content.find("3 -> 1 [label=\"7\"]") != std::string::npos);

  // Cleanup
  std::filesystem::remove(input_filename);
  std::filesystem::remove(output_filename);
}

TEST(GraphTest, ExportToDotNonWritableFileThrowsException) {
  // This test might not work on all systems depending on permissions
  // Skip this test if we can't create a non-writable directory
  if (std::filesystem::exists("/nonexistent")) {
    return;
  }

  s21_graph graph;
  std::string input_filename = "simple_graph.txt";
  // Create a simple graph
  {
    std::ofstream file(input_filename);
    file << "0 1\n"
         << "1 0\n";
  }

  // Load graph
  graph.LoadFromFile(input_filename);

  // Expect exception when trying to export to a non-writable location
  EXPECT_THROW(
      {
        try {
          std::string output_filename = "/nonexistent/test.dot";
          graph.ExportToDot(output_filename);
        } catch (const std::runtime_error& e) {
          EXPECT_TRUE(
              std::string(e.what()).find("Unable to open file for writing") !=
              std::string::npos);
          throw;
        }
      },
      std::runtime_error);

  // Cleanup
  std::filesystem::remove(input_filename);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
