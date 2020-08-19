#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <cmath>
#include <periodic_function/periodic_function.hpp>

struct callback_counter {
  std::atomic<int> count{0};
  void on_timeout() {
    ++count;
    std::cout << "on_timeout\n";
  }
};

TEST_CASE("Acceptable function timing") {
  const auto target_interval = 300U;

  struct test_callback {
    std::chrono::steady_clock::time_point last_call{};
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

  test_callback callback{};
  dp::periodic_function func(std::bind(&test_callback::on_timeout, &callback), target_interval);
  func.start();

  const auto total_cycles = 25;
  std::this_thread::sleep_for(std::chrono::milliseconds(target_interval * total_cycles));

  func.stop();

  // subtract 1 because we can't calculate an interval on the first cycle.
  CHECK_EQ(callback.count, total_cycles - 1);

  // calculate the average time between cycles
  const auto average_interval = callback.interval_sum / static_cast<double>(callback.count);

  // Might be better to check each interval instead of the average
  CHECK_LE(std::abs(average_interval - (double)target_interval), 1);
}

TEST_CASE("Callable destruction") {
  const auto interval = 300U;

  callback_counter call_back;

  // periodic function scope, ensure that callback calls stop on function destruction
  {
    dp::periodic_function func(std::bind(&callback_counter::on_timeout, &call_back), interval);
    func.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  std::cout << "Callback count: " << call_back.count << '\n';
  CHECK_EQ(call_back.count, 4);
}

TEST_CASE("Repeatedly start callable") {
  const auto interval = 100U;

  callback_counter counter;

  dp::periodic_function func(std::bind(&callback_counter::on_timeout, &counter), interval);

  func.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  func.start();
  func.start();
  func.start();

  std::cout << "Callback count: " << counter.count << '\n';
}