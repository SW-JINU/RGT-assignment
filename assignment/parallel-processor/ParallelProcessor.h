#pragma once
#include <vector>
#include <future>
#include <type_traits>
#include <iterator>

class ParallelProcessor {
private:
    unsigned int num_threads_;

public:
    explicit ParallelProcessor(unsigned int threads) : num_threads_(threads ? threads : 1) {}

    // 순차 map (성능 비교용)
    template <typename Container, typename Function>
    auto sequential_map(const Container& input, Function func)
        -> std::vector<std::decay_t<decltype(func(*std::begin(input)))>>
    {
        using OutT = std::decay_t<decltype(func(*std::begin(input)))>;
        std::vector<OutT> output;
        output.reserve(std::distance(std::begin(input), std::end(input)));

        for (const auto& x : input) {
            output.push_back(func(x));
        }
        return output;
    }

    // 병렬 map (멀티스레딩)
    template <typename Container, typename Function>
    auto parallel_map(const Container& input, Function func)
        -> std::vector<std::decay_t<decltype(func(*std::begin(input)))>>
    {
        using OutT = std::decay_t<decltype(func(*std::begin(input)))>;

        const auto n = static_cast<size_t>(std::distance(std::begin(input), std::end(input)));
        std::vector<OutT> output(n);

        if (n == 0) return output;

        const unsigned int threads = (num_threads_ == 0) ? 1 : num_threads_;
        const size_t chunk = (n + threads - 1) / threads; // 안전한 분할(ceil)

        std::vector<std::future<void>> futures;
        futures.reserve(threads);

        auto it_begin = std::begin(input);

        for (unsigned int i = 0; i < threads; ++i) {
            const size_t start = i * chunk;
            if (start >= n) break;

            const size_t end = std::min(n, start + chunk);

            futures.emplace_back(std::async(std::launch::async, [=, &output, &input]() {
                auto it = std::begin(input);
                std::advance(it, static_cast<long>(start));

                for (size_t idx = start; idx < end; ++idx, ++it) {
                    output[idx] = func(*it);
                }
            }));
        }

        for (auto& f : futures) f.get();
        return output;
    }
};
