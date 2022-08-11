#include <filesystem>
#include <iostream>

#include "graph.h"
#include "task_scheduler.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Provide an graph text file" << std::endl;
    return 1;
  }

  std::string input_filename = argv[1];

  if (std::filesystem::is_regular_file(input_filename) == false) {
    std::cout << "Input filename doesnt exist:" << input_filename << std::endl;
    return 1;
  }

  DependencyGraph graph;
  graph.parseTextFile(input_filename);
  graph.removeTransitivity();
  graph.hasCycles();
  graph.printConnections();
  const auto dependent_nodes_vec = graph.dependentNodes();

  TaskScheduler tasks(dependent_nodes_vec);
  tasks.wait();

  return 0;
}