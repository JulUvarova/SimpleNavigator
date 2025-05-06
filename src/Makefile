# Dependencies: gtest, lcov, (?)doxygen

# googletest on windows
# git clone https://github.com/google/googletest.git external/googletest
# cd external/googletest
# mkdir build
# cd build
# cmake .. -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=C:\msys64\ucrt64\bin\mingw32-make.exe
# mingw32-make

CXX = g++

# Base flags
BASE_CXXFLAGS = -std=c++20 -I./
BASE_LDFLAGS = -pthread

# OS-specific flags
ifeq ($(OS), Windows_NT)
    # Windows (MinGW/MSYS2)
    GTEST_INC = -I./external/googletest/googletest/include
    GTEST_LIB = -L./external/googletest/build/lib
    CXXFLAGS = $(BASE_CXXFLAGS) $(GTEST_INC)
    LDFLAGS = $(BASE_LDFLAGS) $(GTEST_LIB) -lgtest -lgtest_main
    GCOV_FLAGS = --coverage -lgcov
else
    # Assume Linux/Unix
    CXXFLAGS = $(BASE_CXXFLAGS)
    LDFLAGS = $(BASE_LDFLAGS) -lgtest -lgtest_main
    GCOV_FLAGS = --coverage -lsubunit -lgcov
endif

CPPCHECK_FLAGS = --enable=all --suppress=missingIncludeSystem --language=c++ \
				 --quiet --suppress=unusedFunction --suppress=unusedStructMember \
				 --suppress=shadowFunction --suppress=missingInclude --suppress=unknownMacro \
				 --suppress=unmatchedSuppression --suppress=missingInclude --suppress=checkersReport

PROJECT_NAME = SimpleNavigator

CONTAINERS_INC = containers/*.h

CLI_SRC = cli/cli.cc

GRAPH_LIB = s21_graph.a
GRAPH_SRC = graph/graph.cc
GRAPH_TEST_SRC = graph/graph_test.cc
TEST_GRAPH_BIN = test_graph

GRAPH_ALGORITHMS_LIB = s21_graph_algorithms.a
GRAPH_ALGORITHMS_SRC = graph_algorithms/graph_algorithms.cc
GRAPH_ALGORITHMS_TEST_SRC = graph_algorithms/graph_algorithms_test.cc
TEST_ALG_BIN = test_graph_algorithms

#########################################
#---- Build and run SimpleNavigator ----#
#########################################
all: $(PROJECT_NAME)
	./$(PROJECT_NAME)

$(PROJECT_NAME): $(CONTAINERS_INC) s21_graph s21_graph_algorithms main.cpp
	$(CXX) $(CXXFLAGS) main.cpp $(CLI_SRC) -o $@ $(GRAPH_LIB) $(GRAPH_ALGORITHMS_LIB) $(LDFLAGS)
	rm -f cli/*.o $(GRAPH_LIB) $(GRAPH_ALGORITHMS_LIB)

#########################################
#------------ Build Library ------------#
#########################################
s21_graph: $(GRAPH_SRC:.cc=.o) $(CONTAINERS_INC)
	ar rcs $(GRAPH_LIB) $^

s21_graph_algorithms: $(GRAPH_ALGORITHMS_SRC:.cc=.o) $(CONTAINERS_INC)
	ar rcs $(GRAPH_ALGORITHMS_LIB) $^

%.o: %.cc $(CONTAINERS_INC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

#########################################
#--------- Build and run Tests ---------#
#########################################
test: test_graph test_graph_algorithms

test_graph: s21_graph $(GRAPH_TEST_SRC) 
	$(CXX) $(CXXFLAGS) -o $@ $(GRAPH_TEST_SRC) $(GRAPH_LIB) $(LDFLAGS)
	./$@

test_graph_algorithms: s21_graph s21_graph_algorithms $(GRAPH_ALGORITHMS_TEST_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(GRAPH_ALGORITHMS_TEST_SRC) $(GRAPH_ALGORITHMS_LIB) $(GRAPH_LIB) $(LDFLAGS)
	./$@

#########################################
#----------- Test coverage -------------#
#########################################
gcov_report: clean_coverage
	$(CXX) $(GCOV_FLAGS) $(CXXFLAGS) $(GRAPH_TEST_SRC) $(GRAPH_SRC) -o $(TEST_GRAPH_BIN) $(LDFLAGS)
	./$(TEST_GRAPH_BIN)
	lcov --ignore-errors mismatch,mismatch --no-external -t "$(TEST_GRAPH_BIN)" -o ./$(TEST_GRAPH_BIN).info -c -d .
	lcov --remove ./$(TEST_GRAPH_BIN).info "*_test.cc" ".h" -o ./$(TEST_GRAPH_BIN).info

	$(CXX) $(GCOV_FLAGS) $(CXXFLAGS) $(GRAPH_ALGORITHMS_TEST_SRC) $(GRAPH_ALGORITHMS_SRC) $(GRAPH_SRC) -o $(TEST_ALG_BIN) $(LDFLAGS)
	./$(TEST_ALG_BIN)
	lcov --ignore-errors mismatch,mismatch --no-external -t "$(TEST_ALG_BIN)" -o ./$(TEST_ALG_BIN).info -c -d .
	lcov --remove ./$(TEST_ALG_BIN).info "*_test.cc" ".h" -o ./$(TEST_ALG_BIN).info

# 	# Merge coverage data and generate report
	lcov -a ./$(TEST_ALG_BIN).info -a ./$(TEST_GRAPH_BIN).info -o merged_coverage.info
	genhtml -o report merged_coverage.info

	open ./report/index.html

clean_coverage:
	rm -rf coverage* report
	rm -f *.gcda *.gcno *.info test_obj_data test_transform
	
#########################################
#----------- Documentation -------------#
#########################################
dvi: 
	doxygen
	open docs/html/index.html

clean_dvi:
	rm -rf ./docs

#########################################
#---------------- Checks ---------------#
#########################################
check:
	cppcheck */*.cc */*.h $(CPPCHECK_FLAGS)
	clang-format -style='{BasedOnStyle: Google}' -n */*.cc */*.h 

check_fix:
	cppcheck */*.cc */*.h $(CPPCHECK_FLAGS)
	clang-format -style='{BasedOnStyle: Google}' -i */*.cc */*.h 


#########################################
#---------------- Clean ----------------#
#########################################
clean: clean_coverage 
	rm -f $(CONTAINERS_LIB) $(GRAPH_LIB) $(GRAPH_ALGORITHMS_LIB)
	rm -f containers/*.o graph/*.o graph_algorithms/*.o cli/*.o
	rm -f test_graph test_graph_algorithms $(PROJECT_NAME)