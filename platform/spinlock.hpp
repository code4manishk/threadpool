#ifndef SPINLOCK_HPP__
#define SPINLOCK_HPP__

#include <atomic>
#include <pthread.h>

namespace thp {
namespace platform {

struct spin_mutex {
    void lock() noexcept {
        auto ticket = in.fetch_add(1, std::memory_order_relaxed);
        while(out.load(std::memory_order_acquire) != ticket);
    }
    void unlock() noexcept
    {
        out.store(out.load(std::memory_order_relaxed) + 1, std::memory_order_release);
    }
 
private:
    alignas(hardware_destructive_interference_size) std::atomic<std::uint32_t> in{0};
    alignas(hardware_destructive_interference_size) std::atomic<std::uint32_t> out{0};
};

struct spin_shared_mutex {
  explicit spin_shared_mutex() {
    pthread_rwlock_init(&mu_, nullptr);
  }

  ~spin_shared_mutex() noexcept {
    pthread_rwlock_destroy(&mu_);
  }

  bool try_lock() noexcept {
    return 0 == pthread_rwlock_trywrlock(&mu_);
  }

  void lock_shared() noexcept {
    while(0 != pthread_rwlock_rdlock(&mu_));
  }

  void unlock_shared() noexcept {
    pthread_rwlock_unlock(&mu_);
  }

  void lock() noexcept {
    while(0 != pthread_rwlock_wrlock(&mu_));
  }

  void unlock() noexcept {
    pthread_rwlock_unlock(&mu_);
  }

  pthread_rwlock_t mu_ = PTHREAD_RWLOCK_INITIALIZER;
};

} // namespace platform
} // namespace thp


#endif // SPINLOCK_HPP__
