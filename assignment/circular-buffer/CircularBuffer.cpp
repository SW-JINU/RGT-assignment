#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <stdexcept>

// 고정 크기 원형 버퍼 템플릿 클래스
template <typename T>
class CircularBuffer {
public:
    // Forward Iterator 정의
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        // 반복자 생성자
        Iterator(T* ptr, size_t capacity, size_t current_pos)
            : ptr_(ptr), capacity_(capacity), current_pos_(current_pos) {}

        // 현재 위치의 요소 반환
        reference operator*() const {
            return ptr_[current_pos_ % capacity_];
        }

        pointer operator->() const {
            return &(operator*());
        }

        // 다음 요소로 이동 (전위 증가)
        Iterator& operator++() {
            ++current_pos_;
            return *this;
        }

        // 다음 요소로 이동 (후위 증가)
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        // 반복자 비교
        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.current_pos_ == b.current_pos_;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return !(a == b);
        }

    private:
        T* ptr_;              // 내부 버퍼 시작 주소
        size_t capacity_;     // 버퍼 전체 크기
        size_t current_pos_;  // 논리적 순회 위치
    };

    // 생성자
    explicit CircularBuffer(size_t capacity)
        : capacity_(capacity), size_(0), head_(0), tail_(0) {
        buffer_.resize(capacity_);
    }

    // 상태 조회 함수
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    // 요소 접근 함수
    T& front() {
        if (empty()) throw std::out_of_range("Buffer is empty");
        return buffer_[head_];
    }

    const T& front() const {
        if (empty()) throw std::out_of_range("Buffer is empty");
        return buffer_[head_];
    }

    T& back() {
        if (empty()) throw std::out_of_range("Buffer is empty");
        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    const T& back() const {
        if (empty()) throw std::out_of_range("Buffer is empty");
        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    // 데이터 삽입 / 삭제
    void push_back(const T& item) {
        // 현재 tail 위치에 데이터 저장
        buffer_[tail_] = item;

        // tail을 다음 위치로 이동
        tail_ = (tail_ + 1) % capacity_;

        // 아직 공간이 있으면 크기 증가
        if (size_ < capacity_) {
            size_++;
        }
        // 가득 찬 경우 가장 오래된 데이터 위치 이동
        else {
            head_ = tail_;
        }
    }

    void pop_front() {
        if (empty()) throw std::out_of_range("Buffer is empty");

        // 가장 오래된 요소 제거
        head_ = (head_ + 1) % capacity_;
        size_--;
    }

    // STL 호환 반복자 반환
    Iterator begin() { return Iterator(buffer_.data(), capacity_, head_); }
    Iterator end()   { return Iterator(buffer_.data(), capacity_, head_ + size_); }

private:
    std::vector<T> buffer_;  // 실제 데이터 저장 공간
    size_t capacity_;        // 최대 저장 가능 개수
    size_t size_;            // 현재 저장된 데이터 개수
    size_t head_;            // 가장 오래된 데이터 위치
    size_t tail_;            // 다음 삽입 위치
};

// 테스트 코드
int main() {
    CircularBuffer<double> tempBuffer(5);

    tempBuffer.push_back(23.5);
    tempBuffer.push_back(24.1);
    tempBuffer.push_back(23.8);
    tempBuffer.push_back(25.2);
    tempBuffer.push_back(24.7);
    tempBuffer.push_back(26.1);  // 가장 오래된 값이 덮어써짐

    // STL 알고리즘 사용
    double maxTemp = *std::max_element(tempBuffer.begin(), tempBuffer.end());
    double avgTemp = std::accumulate(tempBuffer.begin(), tempBuffer.end(), 0.0)
                     / tempBuffer.size();

    std::cout << "tempBuffer.size() = " << tempBuffer.size() << "\n";
    std::cout << "tempBuffer.capacity() = " << tempBuffer.capacity() << "\n";
    std::cout << "tempBuffer.empty() = " << (tempBuffer.empty() ? "true" : "false") << "\n";
    std::cout << "maxTemp = " << maxTemp << "\n";
    std::cout << "avgTemp = " << avgTemp << "\n";
    std::cout << "tempBuffer.front() = " << tempBuffer.front() << "\n";
    std::cout << "tempBuffer.back() = " << tempBuffer.back() << "\n";

    return 0;
}
