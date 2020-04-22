#pragma once

#include <mutex>
#include <thread>
#include <type_traits>

namespace dp {
  template <typename Callback> 
  class periodic_function {
  public:
    using time_type = unsigned long long;

    periodic_function(Callback &&callback) : callback_(callback) {}

    ~periodic_function() { if(is_running_) stop(); }

    void call_every(const std::chrono::milliseconds &interval) {
      call_every_internal(interval.count());
    }

    void call_every(const time_type &milliseconds) {
      call_every_internal(milliseconds);
    }

    void stop() {
      std::lock_guard<std::mutex> guard(stop_flag_mutex_);
      stop_ = true;
      is_running_ = false;
    }

    [[nodiscard]] bool is_running() const { return is_running_; }

  private:
    void call_every_internal(const time_type &interval) {
      interval_ = interval;
      this->stop_ = false;
      periodic_function_thread_ = std::thread([this]() {
        while (true) {
          if (safe_read_stop_flag()) break;
          callback_();
          if (safe_read_stop_flag()) break;
          std::this_thread::sleep_for(std::chrono::milliseconds(safe_interval_read()));
          if (safe_read_stop_flag()) break;
        }
      });
      periodic_function_thread_.detach();
      is_running_ = true;
    }

    bool safe_read_stop_flag() {
      std::lock_guard<std::mutex> guard(stop_flag_mutex_);
      return this->stop_;
    }

    time_type safe_interval_read() {
      std::lock_guard<std::mutex> guard(interval_mutex_);
      return interval_;
    }

    bool stop_{false};
    bool is_running_{false};

    std::mutex stop_flag_mutex_;
    std::mutex interval_mutex_;

    time_type interval_{100};
    std::thread periodic_function_thread_;

    Callback callback_;
  };
}  // namespace dp
