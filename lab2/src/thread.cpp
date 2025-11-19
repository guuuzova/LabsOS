#include <pthread.h>
#include <utility>
#include <system_error>

#include "thread.h"


namespace thread {
struct ThreadInfo {
    pthread_t thread;
};

Thread::Thread(ThreadFunc func): func_(func) {
    handle_ = new ThreadInfo();
}

Thread::Thread(Thread&& other) noexcept: func_(other.func_), handle_(other.handle_) {
    other.func_ = nullptr;
    other.handle_ = nullptr;
}

Thread::~Thread() noexcept {
    delete handle_;
}

Thread& Thread::operator=(Thread&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    Thread tmp = std::move(other);
    this->Swap(tmp);
    return *this;
}

void Thread::Run(void* data) {
    int res = pthread_create(&(handle_->thread), NULL, func_, data);
    if (res != 0) {
        throw std::system_error(res, std::system_category(), "pthread_create failed");
    }
}

void Thread::Join() {
    int res = pthread_join(handle_->thread, NULL);
    if (res != 0) {
        throw std::system_error(res, std::system_category(), "pthread_join failed");
    }
}

void Thread::Swap(Thread& other) {
    std::swap(func_, other.func_);
    std::swap(handle_, other.handle_);
}

}