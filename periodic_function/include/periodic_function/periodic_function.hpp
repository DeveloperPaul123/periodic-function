#pragma once

#include <mutex>
#include <thread>
#include <type_traits>

namespace dp {
  /**
   * @brief Repeatedly calls a function at a given time interval.
   * @tparam Callback the callback time (std::function or a lambda)
   */
  template <typename Callback> class periodic_function {
  public:
    using time_type = unsigned long long;

    periodic_function(Callback &&callback, const time_type &interval)
        : interval_(interval), callback_(callback) {}

    ~periodic_function() {
      if (is_running_) stop();
    }

    /**
     * @brief Start calling the callback function.
     * @details If the callback is running already, calling start again will stop any existing
     * callback execution and will restart it. This may result in the callback being called with a
     * shorter time interval than expected.
     */
    void start() { start_internal(interval_); }

    /**
     * @brief Stop calling the callback function if the timer is running.
     */
    void stop() {
      stop_ = true;
      is_running_ = false;
    }

    /**
     * @brief Returns a boolean to indicate if the timer is running.
     * @return true if the timer is running, false otherwise.
     */
    [[nodiscard]] bool is_running() const { return is_running_; }

  private:
    void start_internal(const time_type &interval) {
      if (is_running()) stop();
      std::thread([this, interval]() {
        while (true) {
          if (stop_) break;
          callback_();
          std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
      }).detach();
      is_running_ = true;
    }

    std::atomic_bool stop_{false};
    std::atomic_bool is_running_{false};

    time_type interval_{100};

    Callback callback_;
  };
}  // namespace dp
