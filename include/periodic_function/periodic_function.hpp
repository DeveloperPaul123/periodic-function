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

    template <typename T> using is_suitable_callback
        = std::enable_if_t<std::is_invocable_v<T> && details::has_default_operator_v<T>>;
  }  // namespace details

  namespace policies {
    /// @name Missed interval policies
    /// @}
    struct schedule_next_missed_interval_policy {
      template <typename TimeType>
      static constexpr TimeType schedule(TimeType callback_time, TimeType interval) {
        if (callback_time >= interval) {
          /**
           * Handle case where callback execution took longer than an interval.
           * We skip the current loop and fire on next iteration possible
           * Use modulus here in case we missed multiple intervals.
           **/
          auto append_time = callback_time % interval;
          if (append_time == TimeType{0}) {
            return interval;
          }
          return append_time;
        }
        // take into account the callback duration for the next cycle
        return interval - callback_time;
      }
    };

    struct invoke_immediately_missed_interval_policy {
      template <typename TimeType> static TimeType schedule(TimeType, TimeType) {
        // immediate dispatch
        return TimeType{0};
      }
    };
    /// @}
  }  // namespace policies

  /**
   * @brief Repeatedly calls a function at a given time interval.
   * @tparam Callback the callback time (std::function or a lambda)
   */
  template <typename Callback,
            typename MissedIntervalPolicy = policies::schedule_next_missed_interval_policy,
            typename = details::is_suitable_callback<Callback>>
  class periodic_function final {
  public:
    using time_type = std::chrono::steady_clock::duration;

    periodic_function(Callback &&callback, const time_type &interval) noexcept
        : interval_(interval), callback_(std::forward<Callback>(callback)) {}
    template <typename ReturnType>

    periodic_function(const periodic_function &other) = delete;
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
      if (this != &other) {
        interval_ = other.interval_;
        callback_ = std::move(other.callback_);
        if (other.is_running()) {
          other.stop();
          start();
        }
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
        const auto thread_start = std::chrono::steady_clock::now();
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
          const auto callback_start = std::chrono::steady_clock::now();
          // suppress exceptions
          try {
            callback_();
          } catch (...) {
          }
          const auto callback_end = std::chrono::steady_clock::now();
          const time_type callback_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
              callback_end - callback_start);
          const time_type append_time
              = MissedIntervalPolicy::schedule(callback_duration, interval_);
          future_time += append_time;
        }
      });
    }

    using mutex_type = std::mutex;
    mutex_type stop_cv_mutex_{};
    std::thread runner_{};
    std::atomic_bool stop_ = false;
    std::condition_variable stop_condition_{};
    time_type interval_{100};
    Callback callback_;
  };

  /// @name CTAD guides
  /// @{
  template <typename ReturnType> periodic_function(ReturnType (*)())
      -> periodic_function<ReturnType (*)()>;
  /// @}

}  // namespace dp
