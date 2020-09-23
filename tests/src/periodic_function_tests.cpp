#include <doctest/doctest.h>

#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <periodic_function/periodic_function.hpp>

struct callback_counter {
  std::atomic<int> count{0};
  void on_timeout() {
    ++count;
    std::cout << "on_timeout\n";
  }
};

TEST_CASE("Acceptable function timing") {
  struct test_callback {
    std::chrono::high_resolution_clock::time_point last_call{};
    double interval_sum{0.0};
    std::atomic<int> count{};
    bool first_call = true;
    void on_timeout() {
      if (first_call) {
        first_call = false;
        last_call = std::chrono::high_resolution_clock::now();
        return;
      }

      ++count;

      // check elapsed time against the interval time
      const auto now = std::chrono::high_resolution_clock::now();
      const auto elapsed_time
          = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_call);
      interval_sum += static_cast<double>(elapsed_time.count());

      std::cout << "Elapsed time: " << elapsed_time.count() << std::endl;
      last_call = now;
    }
  };

  std::vector<std::chrono::milliseconds> intervals
      = {std::chrono::milliseconds{100}, std::chrono::milliseconds{300},
         std::chrono::milliseconds{500}, std::chrono::milliseconds{1000}};

  for (const auto &interval : intervals) {
    test_callback callback{};

    dp::periodic_function func(std::bind(&test_callback::on_timeout, &callback), interval);
    func.start();

    const auto total_cycles = 25;
    const auto wake_time
        = std::chrono::high_resolution_clock::now() + (interval * total_cycles) + (interval / 2);
    std::this_thread::sleep_until(wake_time);

    func.stop();

    // subtract 1 because we can't calculate an interval on the first cycle.
    CHECK_EQ(callback.count, total_cycles - 1);

    // calculate the average time between cycles
    const auto average_interval = callback.interval_sum / static_cast<double>(callback.count);

    // Might be better to check each interval instead of the average
    CHECK_LE(std::abs(average_interval - static_cast<double>(interval.count())), 1);
  }
}

TEST_CASE("Callable destruction") {
  // really long interval, 10 minutes
  const std::chrono::milliseconds interval{10 * 60 * 1000};

  callback_counter call_back;

  const auto start_time = std::chrono::high_resolution_clock::now();
  // periodic function scope, ensure that callback calls stop on function destruction
  {
    dp::periodic_function func(std::bind(&callback_counter::on_timeout, &call_back), interval);
    func.start();
  }

  const auto end_time = std::chrono::high_resolution_clock::now();
  // ensure that we were not blocked "waiting" during the extremely long timeout
  CHECK_LE(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count(),
           100);

  std::cout << "Callback count: " << call_back.count << '\n';
  CHECK_EQ(call_back.count, 0);
}

TEST_CASE("Repeatedly start callable") {
  const std::chrono::milliseconds interval{200};

  callback_counter counter;

  dp::periodic_function func(std::bind(&callback_counter::on_timeout, &counter), interval);

  const auto call_count = 5;
  for (auto i = 0; i < call_count; ++i) {
    func.start();
  }
  const auto wake_time
      = std::chrono::high_resolution_clock::now() + (call_count * interval) + (interval / 2);
  std::this_thread::sleep_until(wake_time);
  func.stop();
  std::cout << "Callback count: " << counter.count << '\n';

  CHECK_EQ(counter.count, call_count);
}

TEST_CASE("Callback takes as long or longer than interval") {
  using time_type = std::chrono::milliseconds;
  constexpr static auto interval_count = 4U;
  // note that in the last case the interval time = the callback execution time
  std::array<time_type, interval_count> intervals
      = {time_type{300}, time_type{500}, time_type{700}, time_type{1000}};
  std::array<time_type, interval_count> callback_intervals{time_type{500}, time_type{700},
                                                           time_type{1000}, time_type{1000}};
  for (auto i = 0U; i < interval_count; ++i) {
    const auto interval = intervals[i];
    const auto callback_duration = callback_intervals[i];

    callback_counter counter;
    dp::periodic_function func(
        [&]() -> void {
          counter.on_timeout();
          std::this_thread::sleep_for(callback_duration);
        },
        interval);

    func.start();
    const auto call_count = 10;
    const auto wake_time
        = std::chrono::high_resolution_clock::now() + (call_count * interval) + (interval / 2);

    // sleep until the "wake time"
    std::this_thread::sleep_until(wake_time);
    func.stop();

    // counter should be total time / callback duration
    const auto total_time = call_count * interval;
    const auto total_counts = total_time.count() / callback_duration.count();

    CHECK_EQ(total_counts, counter.count);
  }
}

TEST_CASE("Suppress exceptions in callback") {
  const auto interval = std::chrono::milliseconds{300};

  callback_counter counter;
  dp::periodic_function function(
      [&]() {
        counter.on_timeout();
        throw std::runtime_error("Error in callback.");
      },
      interval);

  const auto call_count = 10;
  CHECK_NOTHROW({
    function.start();
    const auto wake_time
        = std::chrono::high_resolution_clock::now() + (call_count * interval) + (interval / 2);
    std::this_thread::sleep_until(wake_time);
    function.stop();
  });

  CHECK_EQ(counter.count, call_count);
}

TEST_CASE("Check move ctor and assignment operator") {
  const auto interval = std::chrono::milliseconds{500};
  const auto call_count = 4;
  callback_counter counter;
  dp::periodic_function func1(
      [&, function_id = 1]() {
        counter.on_timeout();
        std::cout << "function id: " << function_id << "\n";
      },
      interval);

  func1.start();
  dp::periodic_function func2 = std::move(func1);

  CHECK_EQ(func2.is_running(), true);

  dp::periodic_function func3(std::move(func2));

  CHECK_EQ(func3.is_running(), true);

  std::this_thread::sleep_for(interval * call_count + (interval / 2));

  CHECK_EQ(counter.count, call_count);
}