#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

// g++ -O2 -o program.exe main.cpp

using std::chrono::duration;
using std::chrono::high_resolution_clock;
using nanoseconds = duration<double, std::nano>;

class NumpyCpp {
public:
  // y = a * x + b
  static std::vector<float> linspace(const int &start, const int &end,
                                     const int &points, const bool &endpoint) {
    std::vector<float> ans{};

    // TODO proper error handler
    if (points <= 0)
      return ans;

    ans.reserve(points);

    // se endpoint == false => deltaX = points
    // senão                => deltaX = points - 1
    auto deltaX = points - endpoint;
    auto deltaY = end - start;
    auto alpha = ((float)deltaY) / deltaX;
    auto beta = (float)start;

    for (auto i = 0; i < points; ++i)
      ans.push_back(alpha * i + beta);

    return ans;
  }

  // Docs for lambda expressions in C++ and their capture clause
  // https://learn.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170#capture-clause
  static auto linspace_curied(const int &start) {
    return [=](const int &end) {
      return [=](const int &points) {
        return [=](const bool &endpoint) {
          return NumpyCpp::linspace(start, end, points, endpoint);
        };
      };
    };
  }
};

// ========================================
// Begin helper functions
template <typename T>
std::vector<float> run_and_measure_time(const bool &endpoint, T func) {
  const auto t_begin = high_resolution_clock::now();
  const auto exec_result = func(endpoint);
  const auto t_end = high_resolution_clock::now();

  const nanoseconds t_diff = t_end - t_begin;
  std::cout << "Time: " << t_diff.count() << "ns\n";

  return exec_result;
}

// Simulate Python3 REPL print format
template <typename T> void print(const std::vector<T> &res) {
  std::cout << "array([";

  const auto resSize{res.size()};
  for (auto i = 0; i < resSize; ++i) {
    std::cout << res[i];
    if (i < resSize - 1)
      std::cout << ", ";
  }

  std::cout << "])" << std::endl;
}
// End helper functions
// ========================================

int main() {
  const std::string line(60, '-');

  const std::vector<int> start{0, 0, 0, 0, 0, 2, 2};
  const std::vector<int> end{0, 6, 6, 4, 4, 3, 3};
  const std::vector<int> points{5, 4, 4, 10, 10, 5, 5};
  const std::vector<bool> endpoint{true, true, false, true, false, false, true};

  const auto start_size = start.size();
  for (auto i = 0; i < start_size; ++i) {
    std::cout << line << "\n\n";

    // auto res = NumpyCpp::linspace(start[i], end[i], points[i], endpoint[i]);
    // Partial application with bind
    std::cout << "Partial application with std::bind:\n";
    auto p1 = std::bind(NumpyCpp::linspace, start[i], end[i], points[i],
                        std::placeholders::_1);
    auto res1 = run_and_measure_time(endpoint[i], p1);
    print(res1);

    // Partial application with curied lambda functions
    std::cout << "\nPartial application with curied lambda functions:\n";
    auto p2 = NumpyCpp::linspace_curied(start[i])(end[i])(points[i]);
    auto res2 = run_and_measure_time(endpoint[i], p2);
    print(res2);
  }

  return 0;
}
