#include "CircularBuffer.h"
#include "ParallelProcessor.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>

int main() {
    // 1) CircularBuffer 동작 테스트
    CircularBuffer<int> cb(3);
    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    std::cout << "CircularBuffer contents (range-based for): ";
    for (const auto& item : cb) std::cout << item << " ";
    std::cout << "\n";

    cb.push_back(4); // overwrite
    std::cout << "CircularBuffer after push_back(4): ";
    for (const auto& item : cb) std::cout << item << " ";
    std::cout << "\n\n";

    // 2) 픽셀 데이터 준비
    std::vector<int> pixelData(1'000'000);
    std::iota(pixelData.begin(), pixelData.end(), 0);

    ParallelProcessor processor(4);

    // (A) brightenedImage
    auto brighten_func = [](int pixel) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        return std::min(255, pixel + 50);
    };

    // 순차 시간 측정
    auto seq_start = std::chrono::high_resolution_clock::now();
    auto bright_seq = processor.sequential_map(pixelData, brighten_func);
    auto seq_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> sequential_ms = seq_end - seq_start;

    // 병렬 시간 측정
    auto par_start = std::chrono::high_resolution_clock::now();
    auto bright_par = processor.parallel_map(pixelData, brighten_func);
    auto par_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> parallel_ms = par_end - par_start;

    // 결과 출력 (과제 예시)
    std::cout << "Brightened Image Results:\n";
    std::cout << "brightenedImage[0]: " << bright_par[0] << "\n";
    std::cout << "brightenedImage[1]: " << bright_par[1] << "\n";
    std::cout << "brightenedImage[100]: " << bright_par[100] << "\n";
    std::cout << "brightenedImage[999999]: " << bright_par[999999] << "\n\n";

    // (B) pixelStrings
    auto pixelStrings = processor.parallel_map(pixelData, [](int pixel) {
        return std::string("pixel_") + std::to_string(pixel);
    });

    std::cout << "pixelStrings Results:\n";
    std::cout << "pixelStrings[0]: " << pixelStrings[0] << "\n";
    std::cout << "pixelStrings[1]: " << pixelStrings[1] << "\n";
    std::cout << "pixelStrings[100]: " << pixelStrings[100] << "\n\n";

    // (C) squaredPixels
    auto squaredPixels = processor.parallel_map(pixelData, [](int pixel) {
        return pixel * pixel;
    });

    std::cout << "squaredPixels Results:\n";
    std::cout << "squaredPixels[0]: " << squaredPixels[0] << "\n";
    std::cout << "squaredPixels[1]: " << squaredPixels[1] << "\n";
    std::cout << "squaredPixels[10]: " << squaredPixels[10] << "\n\n";

    // 3) 성능 측정 결과 출력
    std::cout << "Processing 1,000,000 elements with 4 threads\n";
    std::cout << "Sequential time: " << sequential_ms.count() << "ms\n";
    std::cout << "Parallel time: " << parallel_ms.count() << "ms\n";
    std::cout << "Speedup: " << (sequential_ms.count() / parallel_ms.count()) << "x\n";

    return 0;
}
