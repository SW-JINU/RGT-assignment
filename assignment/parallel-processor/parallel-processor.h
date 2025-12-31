#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <chrono>
#include <functional>

// 병렬 처리를 수행하는 템플릿 클래스
template <typename T>
class ParallelProcessor {
private:
    unsigned int num_threads;   // 사용할 스레드 개수

public:
    // 생성자: 스레드 개수 설정
    explicit ParallelProcessor(unsigned int threads) : num_threads(threads) {
        if (num_threads == 0) num_threads = 1;
    }

    // 입력 컨테이너에 대해 함수(func)를 병렬로 적용하는 map 함수
    template <typename Function>
    std::vector<T> parallel_map(const std::vector<T>& input, Function func) {
        std::vector<T> output(input.size());

        // 스레드 하나가 처리할 데이터 범위 크기
        size_t chunk_size = input.size() / num_threads;

        // 비동기 작업을 저장할 future 컨테이너
        std::vector<std::future<void>> futures;

        for (unsigned int i = 0; i < num_threads; ++i) {
            size_t start = i * chunk_size;
            size_t end = (i == num_threads - 1)
                           ? input.size()
                           : (i + 1) * chunk_size;

            // 각 구간을 별도 스레드에서 처리
            futures.emplace_back(
                std::async(std::launch::async, [=, &output, &input]() {
                    for (size_t j = start; j < end; ++j) {
                        output[j] = func(input[j]);
                    }
                })
            );
        }

        // 모든 스레드가 끝날 때까지 대기
        for (auto& fut : futures) {
            fut.get();
        }

        return output;
    }
};
