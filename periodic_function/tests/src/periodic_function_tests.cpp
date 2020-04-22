#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <periodic_function/periodic_function.hpp>

TEST_CASE("Timing of function is within reason") {
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

      count++;

      // check elapsed time against the interval time
      const auto now = std::chrono::high_resolution_clock::now();
      const auto elapsed_time
          = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_call);
      interval_sum += (double)elapsed_time.count();

      std::cout << "Elapsed time: " << elapsed_time.count() << std::endl;
      last_call = now;
    }
  };

  test_callback callback{};
  dp::periodic_function func(std::bind(&test_callback::on_timeout, &callback));
  func.call_every(std::chrono::milliseconds(target_interval));

  const auto total_cycles = 25;
  std::this_thread::sleep_for(std::chrono::milliseconds(target_interval * total_cycles));

  func.stop();

  // subtract 1 because we can't calculate an interval on the first cycle.
  CHECK_EQ(callback.count, total_cycles-1);

  // calculate the average time between cycles
  const auto average_interval = callback.interval_sum / (double)callback.count;

  CHECK_LE(std::abs(average_interval - (double)target_interval), 2);
}