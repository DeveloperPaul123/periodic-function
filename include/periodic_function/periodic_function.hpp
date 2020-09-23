#pragma once

#include <chrono>
#include <future>
#include <thread>
#include <type_traits>

namespace dp {
  namespace details {
    template <class T, typename = void> struct has_default_operator : std::false_type {};

    template <typename T> struct has_default_operator<T, std::void_t<decltype(std::declval<T>()())>>
        : std::true_type {};

    template <typename T> inline constexpr auto has_default_operator_v
        = has_default_operator<T>::value;
  }  // namespace details

  /**
   * @brief Repeatedly calls a function at a given time interval.
   * @tparam Callback the callback time (std::function or a lambda)
   */
  template <typename Callback, typename = std::enable_if_t<std::is_invocable<Callback>::value>,
            typename = std::enable_if_t<dp::details::has_default_operator_v<Callback>>>
  class periodic_function final {
  public:
    using time_type = std::chrono::system_clock::duration;

    periodic_function(Callback &&callback, const time_type &interval) noexcept
        : interval_(interval), callback_(std::forward<Callback>(callback)) {}
    periodic_function(periodic_function &other) = delete;
    periodic_function(periodic_function &&other) noexcept
        : interval_(other.interval_), callback_(std::move(other.callback_)) {
      if (other.is_running()) {
        other.stop();
        start();
      }
    }
    ~periodic_function() { stop(); }

    periodic_function &operator=(const periodic_function &other) = delete;
    periodic_function &operator=(periodic_function &&other) noexcept {
      interval_ = other.interval_;
      callback_ = std::move(other.callback_);
      if (other.is_running()) {
        other.stop();
        start();
      }
      return *this;
    }

    /**
     * @brief Start calling the callback function.
     * @details If the callback is running already, calling start again will stop any existing
     * callback execution and will restart it. This may result in the callback being called with a
     * shorter time interval than expected.
     */
    void start() { start_internal(); }

    /**
     * @brief Stop calling the callback function if the timer is running.
     */
    void stop() {
      {
        std::unique_lock<mutex_type> lock(stop_cv_mutex_);
        stop_ = true;
      }
      stop_condition_.notify_one();
      // ensure that the thread exits.
      if (runner_.joinable()) {
        runner_.join();
      }
      {
        // reset stop condition
        std::unique_lock<mutex_type> lock(stop_cv_mutex_);
        stop_ = false;
      }
    }

    /**
     * @brief Returns a boolean to indicate if the timer is running.
     * @return true if the timer is running, false otherwise.
     */
    [[nodiscard]] bool is_running() const { return runner_.joinable(); }

  private:
    void start_internal() {
      if (is_running()) stop();
      runner_ = std::thread([this]() {
        const auto thread_start = std::chrono::high_resolution_clock::now();
        // pre-calculate time
        auto future_time = thread_start + interval_;
        while (true) {
          // sleep first
          {
            std::unique_lock<mutex_type> lock(stop_cv_mutex_);
            stop_condition_.wait_until(lock, future_time, [&]() -> bool { return stop_; });
            // check for stoppage here while in the scope of the lock
            if (stop_) break;
          }

          // execute the callback and measure execution time
          const auto callback_start = std::chrono::high_resolution_clock::now();
          // suppress exceptions
          try {
            callback_();
          } catch (...) {
          }
          const auto callback_end = std::chrono::high_resolution_clock::now();
          const auto callback_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
              callback_end - callback_start);
          if (callback_duration >= interval_) {
            /**
             * Handle case where callback execution took longer than an interval.
             * We skip the current loop and fire on next iteration possible
             * Use modulus here in case we missed multiple intervals.
             **/
            auto append_time = callback_duration % interval_;
            if (append_time == time_type{0}) {
              future_time += interval_;
            } else {
              future_time += append_time;
            }
          } else {
            // take into account the callback duration for the next cycle
            future_time += interval_ - callback_duration;
          }
        }
      });
    }

    /// @brief Private members
    /// @{
    using mutex_type = std::mutex;
    mutex_type stop_cv_mutex_;
    std::thread runner_{};
    std::atomic_bool stop_ = false;
    std::condition_variable stop_condition_;
    time_type interval_{100};
    Callback callback_;
    /// @}
  };
}  // namespace dp
