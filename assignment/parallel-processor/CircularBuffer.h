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

        Iterator(T* ptr, size_t capacity, size_t current_pos)
            : ptr_(ptr), capacity_(capacity), current_pos_(current_pos) {}

        reference operator*() const { return ptr_[current_pos_ % capacity_]; }
        pointer operator->() const { return &(operator*()); }

        Iterator& operator++() { ++current_pos_; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.ptr_ == b.ptr_ && a.current_pos_ == b.current_pos_;
        }
        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return !(a == b);
        }

    private:
        T* ptr_;
        size_t capacity_;
        size_t current_pos_;
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

        reference operator*() const { return ptr_[current_pos_ % capacity_]; }
        pointer operator->() const { return &(operator*()); }

        ConstIterator& operator++() { ++current_pos_; return *this; }
        ConstIterator operator++(int) { ConstIterator tmp = *this; ++(*this); return tmp; }

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
        if (capacity_ == 0) throw std::invalid_argument("capacity must be greater than 0");
        buffer_.resize(capacity_);
    }

    // 상태 조회
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    // 맨 앞/뒤 요소 접근 (const/non-const)
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

    // 요소 추가 (가득 차면 가장 오래된 데이터 덮어쓰기)
    void push_back(const T& item) {
        buffer_[tail_] = item;
        tail_ = (tail_ + 1) % capacity_;

        if (size_ < capacity_) ++size_;
        else head_ = tail_;
    }

    // 가장 오래된 요소 제거
    void pop_front() {
        if (empty()) throw std::out_of_range("Buffer is empty");
        head_ = (head_ + 1) % capacity_;
        --size_;
    }

    // 반복자 반환 (const/non-const)
    Iterator begin() { return Iterator(buffer_.data(), capacity_, head_); }
    Iterator end()   { return Iterator(buffer_.data(), capacity_, head_ + size_); }

    ConstIterator begin() const { return ConstIterator(buffer_.data(), capacity_, head_); }
    ConstIterator end()   const { return ConstIterator(buffer_.data(), capacity_, head_ + size_); }

    ConstIterator cbegin() const { return begin(); }
    ConstIterator cend()   const { return end(); }

private:
    std::vector<T> buffer_;
    size_t capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;
};
