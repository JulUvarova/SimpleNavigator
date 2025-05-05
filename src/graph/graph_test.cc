#include "graph.h"

#include <gtest/gtest.h>

#include <filesystem>

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
  std::string filename = "non_existent_file.txt";

  EXPECT_THROW(
      {
        try {
          graph.LoadFromFile(filename);
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

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
