#include <mutex>
#include <vector>

#include "thread_pool.h"

void printNodeNumber(size_t i) {
  std::cout << "Running task: " << i << std::endl;
}

class TaskScheduler {
 public:
  TaskScheduler(const std::vector<std::vector<size_t>>& depenency_nodes) : dependency_nodes_(depenency_nodes) {
    processed_nodes_ = std::vector<bool>(dependency_nodes_.size(), false);
    thread_pool_.setCallback(std::bind(&TaskScheduler::callback, this, std::placeholders::_1));
  };

  void wait() {
    thread_pool_.pushTask(Task(0, 0, printNodeNumber));
    while (true) {
      bool is_finished = true;
      {
        std::scoped_lock<std::mutex> lock{m_};
        for (auto& nodes : dependency_nodes_) {
          if (nodes.empty() == false) {
            is_finished = false;
          }
        }
      }
      if (is_finished == true) {
        break;
      }
    }
    thread_pool_.waitForTasks();
  }

 private:
  void callback(const size_t node_number) {
    {
      std::scoped_lock<std::mutex> lock{m_};

      processed_nodes_[node_number] = true;
      std::cout << "Finished task " << node_number << std::endl;
      // remove finished jobs
      size_t i = 0;
      for (auto& nodes : dependency_nodes_) {
        nodes.erase(
            std::remove_if(nodes.begin(), nodes.end(), [node_number](const size_t x) { return x == node_number; }),
            nodes.end());
      }

      // if no dependencies are found, add the task to queue
      for (size_t i = 0; i < dependency_nodes_.size(); i++) {
        if (processed_nodes_[i] == true) {
          continue;
        }

        if (dependency_nodes_[i].empty()) {
          thread_pool_.pushTask(Task(i, 0, printNodeNumber));
          processed_nodes_[i] = true;
        }
      }
    }
  }

 private:
  ThreadPool thread_pool_;

  std::vector<std::vector<size_t>> dependency_nodes_;
  std::mutex m_;
  std::vector<bool> processed_nodes_;
};