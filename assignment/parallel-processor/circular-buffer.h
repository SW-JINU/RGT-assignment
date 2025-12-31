#pragma once
#include <vector>
#include <iterator>
#include <stdexcept>

// 고정 크기 원형 버퍼 템플릿 클래스
template <typename T>
class CircularBuffer {
public:
    // non-const Forward Iterator
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        // 버퍼 시작 주소, 전체 크기, 논리적 인덱스를 받아 생성
        Iterator(T* ptr, size_t capacity, size_t current_pos)
            : ptr_(ptr), capacity_(capacity), current_pos_(current_pos) {}

        // 현재 논리 위치의 요소 반환
        reference operator*() const {
            return ptr_[current_pos_ % capacity_];
        }

        pointer operator->() const { return &(operator*()); }

        // 다음 요소로 이동
        Iterator& operator++() {
            ++current_pos_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.ptr_ == b.ptr_ && a.current_pos_ == b.current_pos_;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return !(a == b);
        }

    private:
        T* ptr_;               // 내부 버퍼 시작 주소
        size_t capacity_;      // 전체 버퍼 크기
        size_t current_pos_;   // 논리적 순회 위치
    };

    // const Forward Iterator
    class ConstIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = const T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;

        ConstIterator(const T* ptr, size_t capacity, size_t current_pos)
            : ptr_(ptr), capacity_(capacity), current_pos_(current_pos) {}

        reference operator*() const {
            return ptr_[current_pos_ % capacity_];
        }

        pointer operator->() const { return &(operator*()); }

        ConstIterator& operator++() {
            ++current_pos_;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const ConstIterator& a, const ConstIterator& b) {
            return a.ptr_ == b.ptr_ && a.current_pos_ == b.current_pos_;
        }

        friend bool operator!=(const ConstIterator& a, const ConstIterator& b) {
            return !(a == b);
        }

    private:
        const T* ptr_;
        size_t capacity_;
        size_t current_pos_;
    };

    // 생성자
    explicit CircularBuffer(size_t capacity)
        : capacity_(capacity), size_(0), head_(0), tail_(0) {
        if (capacity_ == 0)
            throw std::invalid_argument("capacity must be greater than 0");
        buffer_.resize(capacity_);
    }

    // 상태 조회
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    // 맨 앞 요소 접근
    T& front() {
        if (empty()) throw std::out_of_range("Buffer is empty");
        return buffer_[head_];
    }

    const T& front() const {
        if (empty()) throw std::out_of_range("Buffer is empty");
        return buffer_[head_];
    }

    // 맨 뒤 요소 접근
    T& back() {
        if (empty()) throw std::out_of_range("Buffer is empty");
        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    const T& back() const {
        if (empty()) throw std::out_of_range("Buffer is empty");
        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    // 요소 추가 (가득 찬 경우 가장 오래된 데이터 덮어씀)
    void push_back(const T& item) {
        buffer_[tail_] = item;
        tail_ = (tail_ + 1) % capacity_;

        if (size_ < capacity_)
            ++size_;
        else
            head_ = tail_;
    }

    // 가장 오래된 요소 제거
    void pop_front() {
        if (empty()) throw std::out_of_range("Buffer is empty");
        head_ = (head_ + 1) % capacity_;
        --size_;
    }

    // 반복자 반환
    Iterator begin() { return Iterator(buffer_.data(), capacity_, head_); }
    Iterator end()   { return Iterator(buffer_.data(), capacity_, head_ + size_); }

    ConstIterator begin() const { return ConstIterator(buffer_.data(), capacity_, head_); }
    ConstIterator end()   const { return ConstIterator(buffer_.data(), capacity_, head_ + size_); }

    ConstIterator cbegin() const { return begin(); }
    ConstIterator cend()   const { return end(); }

private:
    std::vector<T> buffer_;  // 실제 데이터 저장 공간
    size_t capacity_;        // 최대 저장 개수
    size_t size_;            // 현재 저장된 개수
    size_t head_;            // 가장 오래된 요소 위치
    size_t tail_;            // 다음 삽입 위치
};
