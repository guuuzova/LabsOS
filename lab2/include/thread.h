namespace thread {
using ThreadFunc = typeof(void*(void*))*;
struct ThreadInfo;
using ThreadHandle = ThreadInfo*;

class Thread {
  private:
    ThreadFunc func_;
    ThreadHandle handle_;
  public:
    Thread(ThreadFunc func);
    Thread(const Thread&) = delete;
    Thread(Thread&& other) noexcept;

    Thread& operator=(const Thread&) = delete;
    Thread& operator=(Thread&& other) noexcept;

    void Swap(Thread& other);
    void Run(void* data);
    void Join();
    ~Thread() noexcept;
};
}