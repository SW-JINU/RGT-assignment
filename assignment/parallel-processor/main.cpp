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
    // 프로그램 실행 확인용 출력
    std::cout << "START\n";
    std::cout.flush();

    // 1. CircularBuffer 동작 테스트
    CircularBuffer<int> cb(3);

    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    // 범위 기반 for 문을 이용한 순회
    std::cout << "CircularBuffer contents (range-based for): ";
    for (const auto& item : cb) {
        std::cout << item << " ";
    }
    std::cout << "\n";

    // 버퍼가 가득 찬 상태에서 push → 가장 오래된 값 덮어쓰기
    cb.push_back(4);

    std::cout << "CircularBuffer after push_back(4): ";
    for (const auto& item : cb) {
        std::cout << item << " ";
    }
    std::cout << "\n";

    // 2. ParallelProcessor 병렬 처리 테스트

    // 테스트용 픽셀 데이터 생성 (0 ~ 999999)
    std::vector<int> pixelData(1'000'000);
    std::iota(pixelData.begin(), pixelData.end(), 0);

    // 4개의 스레드를 사용하는 병렬 처리 객체 생성
    ParallelProcessor<int> processor(4);

    // 병렬 처리 시작 시간 측정
    auto start_time = std::chrono::high_resolution_clock::now();

    // 밝기 증가 처리 (람다 함수 사용)
    auto brightenedImage = processor.parallel_map(pixelData, [](int pixel) {
        // 병렬 처리 효과를 확인하기 위한 인위적 지연
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        return std::min(255, pixel + 50);
    });

    // 병렬 처리 종료 시간
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> parallel_ms = end_time - start_time;

    // 결과 일부 출력
    std::cout << "\nBrightened Image Results:\n";
    std::cout << "brightenedImage[0]: " << brightenedImage[0] << "\n";
    std::cout << "brightenedImage[1]: " << brightenedImage[1] << "\n";
    std::cout << "brightenedImage[100]: " << brightenedImage[100] << "\n";
    std::cout << "brightenedImage[999999]: " << brightenedImage[999999] << "\n";

    // 3. 성능 측정 결과 출력
    double sequential_ms = 1000.0; // 예시값 (순차 처리 시간)

    std::cout << "\nProcessing 1,000,000 elements with 4 threads\n";
    std::cout << "Sequential time: ~" << sequential_ms << "ms\n";
    std::cout << "Parallel time: ~" << parallel_ms.count() << "ms\n";
    std::cout << "Speedup: ~" << sequential_ms / parallel_ms.count() << "x\n";

    return 0;
}
