#include <doctest/doctest.h>

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
    std::atomic<int> count{0};
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

      std::cout << "Elapsed time: " << elapsed_time.count() << '\n';
      last_call = now;
    }
  };

  std::vector<std::chrono::milliseconds> intervals
      = {std::chrono::milliseconds{100},  std::chrono::milliseconds{300},
         std::chrono::milliseconds{500},  std::chrono::milliseconds{1000},
         std::chrono::milliseconds{5000}, std::chrono::milliseconds{10000}};

  for (const auto &interval : intervals) {
    test_callback callback{};

    dp::periodic_function func(std::bind(&test_callback::on_timeout, &callback), interval);
    func.start();

    const auto total_cycles = 25;
    const auto wake_time = std::chrono::high_resolution_clock::now() + (interval * total_cycles);
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
  const std::chrono::milliseconds interval{300};

  callback_counter call_back;

  // periodic function scope, ensure that callback calls stop on function destruction
  {
    dp::periodic_function func(std::bind(&callback_counter::on_timeout, &call_back), interval);
    func.start();
    const auto wake_time
        = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1000);
    std::this_thread::sleep_until(wake_time);
  }

  std::cout << "Callback count: " << call_back.count << '\n';
  CHECK_EQ(call_back.count, 4);
}

TEST_CASE("Repeatedly start callable") {
  const std::chrono::milliseconds interval{100};

  callback_counter counter;

  dp::periodic_function func(std::bind(&callback_counter::on_timeout, &counter), interval);

  const auto call_count = 5;
  for (auto i = 0; i < call_count; ++i) {
    func.start();
  }
  const auto wake_time = std::chrono::high_resolution_clock::now() + (call_count * interval);
  std::this_thread::sleep_until(wake_time);
  func.stop();
  std::cout << "Callback count: " << counter.count << '\n';

  CHECK_EQ(counter.count, call_count);
}