#pragma once

class MutexLockGuard {
    public:
        MutexLockGuard(os_mutex_t* mutex) : mutex_(mutex) {
            os_mutex_lock(*mutex_);
        }
        
        ~MutexLockGuard() {
            os_mutex_unlock(*mutex_);
        }
        
        // Disable copy and assignment.
        MutexLockGuard(const MutexLockGuard&) = delete;
        MutexLockGuard& operator=(const MutexLockGuard&) = delete;
    
    private:
        os_mutex_t* mutex_;
    };