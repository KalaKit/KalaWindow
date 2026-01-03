//------------------------------------------------------------------------------
// thread_utils.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - lock, lockwait and unlock overrides for handling safe access to variables
//   - lock_m, lockwait_m (where applicable) and unlock_m for mutexes
//   - jthread (joinable thread) which returns the created thread so it can be joined
//   - dthread (self-exiting thread)
//------------------------------------------------------------------------------

#pragma once

#include <atomic>
#include <concepts>
#include <thread>
#include <chrono>

namespace KalaHeaders::KalaThread
{	
	using std::atomic;
	using std::memory_order_acquire;
	using std::memory_order_release;
	using std::memory_order_relaxed;
	using std::convertible_to;
	using std::is_arithmetic_v;
	using std::is_pointer_v;
	using std::same_as;
	using std::thread;
	using std::this_thread::yield;
	using std::forward;
	using std::invocable;
	using std::chrono::milliseconds;
	using std::chrono::steady_clock;
	using std::chrono::duration;
	using std::chrono::time_point;
	using std::remove_cvref_t;
	
	using abool = atomic<bool>;
	
	//
	// CREATE THREAD
	//
	
	//Creates, runs and returns a joinable thread
	template <invocable F>
	thread jthread(F&& func)
	{
		return thread(forward<F>(func));
	}
	
	//Creates, runs and detaches a non-joinable thread
	template <invocable F>
	void dthread(F&& func)
	{
		thread t(forward<F>(func));
		t.detach();
	}
	
	//
	// CONCEPTS
	//
	
	template<typename T>
	concept is_atomic_bool = 
		same_as<remove_cvref_t<T>, atomic<bool>>;
	
	template<typename T>
	concept is_atomic_pointer =
		requires(T& p)
		{
			requires is_pointer_v<typename T::value_type>;
			{ p.exchange(nullptr, memory_order_acquire) } -> convertible_to<typename T::value_type>;
		};
	
	template <typename T>
	concept is_mutex = 
		requires(T& m)
		{
			{ m.try_lock() } -> same_as<bool>; //try to lock once
			{ m.lock() }     -> same_as<void>; //try to lock repeatedly
			{ m.unlock() }   -> same_as<void>;
		};
		
	template <typename M>
	concept is_shared_mutex =
		requires(M& m)
		{
			//exclusive write-only access
			
			{ m.try_lock() } -> same_as<bool>; //try to lock once
			{ m.lock() }     -> same_as<void>; //try to lock repeatedly
			{ m.unlock() }   -> same_as<void>;
			
			//shared read-only access
			
			{ m.try_lock_shared() } -> same_as<bool>; //try to lock once
			{ m.lock_shared() }     -> same_as<void>; //try to lock repeatedly
			{ m.unlock_shared() }   -> same_as<void>;
		};
		
	template <typename M>
	concept is_timed_mutex =
		requires(M& m)
		{
			{ m.try_lock() } -> same_as<bool>; //try to lock once
			{ m.lock() }     -> same_as<void>; //try to lock repeatedly
			{ m.unlock() }   -> same_as<void>;
			
			//timed locking
			
			{ m.try_lock_for(milliseconds(1)) }       -> same_as<bool>; //lock for a time period
			{ m.try_lock_until(steady_clock::now()) } -> same_as<bool>; //lock until a specific time
		};
	
	//
	// MUTEX
	//
		
	//Try to lock a mutex once
	template<is_mutex M>
	bool lock_m(M& m) { return m.try_lock(); }
	
	//Wait and try to lock a mutex
	template<is_mutex M>
	void lockwait_m(M& m) { m.lock(); }
	
	//Unlock a mutex
	template<is_mutex M>
	void unlock_m(M& m) { m.unlock(); }
	
	//
	// SHARED MUTEX
	//
	
	//Lock a shared mutex for exclusive write-only access
	template<is_shared_mutex M>
	bool lock_m(M& m) { return m.try_lock(); }
	//Lock a shared mutex for shared read-only access
	template<is_shared_mutex M>
	bool lock_shared_m(M& m) { return m.try_lock_shared(); }
	
	//Wait and try to lock a shared mutex for exclusive write-only access
	template<is_shared_mutex M>
	void lockwait_m(M& m) { m.lock(); }
	//Wait and try to lock a shared mutex for shared read-only access
	template<is_shared_mutex M>
	void lockwait_shared_m(M& m) { m.lock_shared(); }
	
	//Unlock a shared mutex for exclusive write-only access
	template<is_shared_mutex M>
	void unlock_m(M& m) { m.unlock(); }
	//Unlock a shared mutex for shared read-only access
	template<is_shared_mutex M>
	void unlock_shared_m(M& m) { m.unlock_shared(); }
	
	//
	// TIMED MUTEX
	//
		
	//Lock a timed mutex
	template<is_timed_mutex M>
	bool lock_m(M& m) { return m.try_lock(); }
	
	//Wait and try to lock a timed mutex
	template<is_timed_mutex M>
	void lockwait_m(M& m) { m.lock(); }
	
	//Unlock a timed mutex
	template<is_timed_mutex M>
	void unlock_m(M& m) { m.unlock(); }
	
	//Wait for duration and keep trying to lock timed mutex
	template<is_timed_mutex M, class Rep, class Dur>
	bool lock_for_m(M& m, const duration<Rep, Dur>& t)
	{
		return m.try_lock_for(t);
	}
	
	//Wait until time point and keep trying to lock timed mutex
	template<is_timed_mutex M, class Clock, class Dur>
	bool lock_until_m(M& m, const time_point<Clock, Dur>& t)
	{
		return m.try_lock_until(t);
	}
	
	//
	// LOCK
	//
	
	//Lock an atomic boolean for thread-safe use
	template <is_atomic_bool T>
	inline bool lock(T& flag)
	{
		return !flag.exchange(true, memory_order_acquire);
	}
	
	//Lock an atomic arithmetic for thread-safe use
	template <typename T>
	inline T lock(atomic<T>& value)
		requires is_arithmetic_v<T>
	{
		return value.exchange(
			value.load(memory_order_relaxed),
			memory_order_acquire);
	}
	
	//Lock an atomic raw pointer for thread-safe use
	template <is_atomic_pointer T>
	inline bool lock(T& ptr)
	{
		return ptr.exchange(nullptr, memory_order_acquire) != nullptr;
	}
	
	//
	// LOCKWAIT
	//
	
	//Wait until able to lock the atomic boolean for thread-safe use
	template <is_atomic_bool T>
	inline void lockwait(T& flag)
	{
		int spin_count = 0;
		while (flag.exchange(true, memory_order_acquire))
		{
			if (++spin_count > 50)
			{
				yield();
				spin_count = 0;
			}
		}
	}
	
	//Wait until able to lock the atomic arithmetic for thread-safe use
	template <typename T>
	inline void lockwait(atomic<T>& value)
		requires is_arithmetic_v<T>
	{
		int spin_count = 0;	
		while (value.exchange(
			value.load(memory_order_relaxed),
			memory_order_acquire))
		{
			if (++spin_count > 50)
			{
				yield();
				spin_count = 0;
			}
		}
	}
	
	//Wait until able to lock the atomic pointer for thread-safe use
	template <is_atomic_pointer T>
	inline void lockwait(T& ptr)
	{
		int spin_count = 0;
		while (ptr.exchange(nullptr, memory_order_acquire) == nullptr)
		{
			if (++spin_count > 50)
			{
				yield();
				spin_count = 0;
			}
		}
	}
	
	//
	// UNLOCK
	//
	
	//Unlock a locked atomic boolean for thread-safe use
	template <is_atomic_bool T>
	inline bool unlock(T& flag)
	{
		//unlocking something that wasn't locked
		if (!flag.load(memory_order_acquire)) return false;
		
		flag.store(false, memory_order_release);
		return true;
	}
	
	//Unlock a locked atomic arithmetic for thread-safe use
	template <typename T>
	inline bool unlock(atomic<T>& value)
		requires is_arithmetic_v<T>
	{
		//load current expected "locked" value
		T current = value.load(memory_order_acquire);
		
		//try to store back the same value
		T previous = value.exchange(
			current,
			memory_order_release);
			
		//if previous is same as current then it was successfully unlocked,
		//otherwise it was already unlocked
		return previous == current;
	}
	
	//Unlock a locked atomic pointer for thread-safe use
	template <is_atomic_pointer T>
	inline bool unlock(T& ptr, typename T::value_type value)
	{
		//return false if already unlocked
		if (ptr.load(memory_order_acquire) != nullptr) return false;
		
		ptr.store(value, memory_order_release);
		return true;
	}
}