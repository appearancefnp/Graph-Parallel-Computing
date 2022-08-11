#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

struct Task {
  Task() = default;

  Task(size_t id, size_t status, std::function<void(size_t)> function) {
    task_id = id;
    task_status = status;
    task_function = function;
  }

  size_t task_id;
  size_t task_status;
  std::function<void(size_t)> task_function;
};

class ThreadPool {
 public:
  ThreadPool() : thread_count_(std::thread::hardware_concurrency()) { createThreads(); }

  ~ThreadPool() {
    waitForTasks();
    running_ = false;
    destroyThreads();
  }

  void pushTask(const Task& task) {
    {
      const std::scoped_lock lock(queue_mutex_);
      total_tasks_++;
      tasks_.push(task);
      std::cout << "Pushed task: " << task.task_id << std::endl;
    }
  }

  void setCallback(const std::function<void(size_t)>& callback) { callback_ = callback; }

  void waitForTasks() {
    while (true) {
      if (total_tasks_ == 0) {
        break;
      }
      std::this_thread::yield();
    }
  }

 private:
  void worker() {
    while (running_) {
      Task task;
      if (popTask(task)) {
        task.task_function(task.task_id);
        callback_(task.task_id);
        total_tasks_--;
      }
      std::this_thread::yield();
    }
  }

  bool popTask(Task& task) {
    const std::scoped_lock lock(queue_mutex_);
    if (tasks_.empty()) {
      return false;
    }

    task = std::move(tasks_.front());
    tasks_.pop();
    return true;
  }

  void createThreads() {
    threads_.resize(thread_count_);
    for (size_t i = 0; i < threads_.size(); ++i) {
      threads_[i] = std::thread(&ThreadPool::worker, this);
    }
  }

  void destroyThreads() {
    for (size_t i = 0; i < threads_.size(); ++i) {
      threads_[i].join();
    }
  }

 private:
  std::vector<std::thread> threads_;
  size_t thread_count_;

  mutable std::mutex queue_mutex_{};
  std::atomic<bool> running_{true};
  std::atomic<size_t> total_tasks_{0};
  std::queue<Task> tasks_{};
  std::function<void(size_t)> callback_;
};