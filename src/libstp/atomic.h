#ifndef ATOMIC_H_
#define ATOMIC_H_

#ifdef WIN32
#include <windows.h>
#endif

namespace detail
{
// gcc_builtins 原子操作
#ifndef WIN32
template<typename T>
class AtomicIntegerT
{
public:
	AtomicIntegerT(T val = 0)
		: value_(val)
	{
	}

	AtomicIntegerT(const AtomicIntegerT& that)
		: value_(that.value())
	{}

	AtomicIntegerT& operator=(const AtomicIntegerT& that)
	{
		GetAndSet(that.value());
		return *this;
	}

	T value()
	{
		// in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
        // 原子比较和交换
        // type __sync_val_compare_and_swap (type *ptr, type oldval type newval, ...)
        // 如果 *ptr == oldvalue, 就把newvale写入ptr，返回操作之前的值
		return __sync_val_compare_and_swap(&value_, 0, 0);
	}

	T GetAndAdd(T x) // 后置++
	{
		// in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
        // 返回更新前的值
		return __sync_fetch_and_add(&value_, x);
	}

	T AddAndGet(T x)
	{
		return GetAndAdd(x) + x;
	}

	T IncrementAndGet()
	{
		return AddAndGet(1);
	}

	T DecrementAndGet()
	{
		return AddAndGet(-1);
	}

	void Add(T x)
	{
		GetAndAdd(x);
	}

	void Increment()
	{
		IncrementAndGet();
	}

	void Decrement()
	{
		DecrementAndGet();
	}

	T GetAndSet(T new_val)
	{
		// in gcc >= 4.7: __atomic_store_n(&value, newValue, __ATOMIC_SEQ_CST)
        // type __sync_lock_test_and_set (type *ptr, type value, ...)
        // 将*ptr设为value并返回*ptr操作之前的值
		return __sync_lock_test_and_set(&value_, new_val);
	}

private:
	volatile T value_;
};
#endif

} // detail


template<typename T> // T 只能是 long 或者 unsigned long
class Atomic
{
public:
	Atomic(T val = 0) : value_(val)
	{
	}

	~Atomic()
	{
	}

	T operator++(int)
	{
		T val;
#ifdef WIN32
		val = value_;
		++value_;
#else
		val = value_.value();
		value_.Increment();
#endif
		return val;
	}

	T operator++() // prefix++
	{
#ifdef WIN32
		value_ = InterlockedIncrement(&value_);
		return value_;
#else
		return value_.IncrementAndGet();
#endif
	}

	T operator--(int)
	{
		T val;
#ifdef WIN32
		val = value_;
		--value_;
#else
		val = value_.value();
		value_.Decrement();
		return val;
#endif
		return val;
	}

	T operator--() // prefix--
	{
#ifdef WIN32
		value_ = InterlockedDecrement(&value_);
		return value_;
#else
		return value_.DecrementAndGet();
#endif
	}

	T Add(T val)
	{
#ifdef WIN32
		val = InterlockedExchangeAdd(&value_, val); // 返回改变之前的值
		return val;
#else
		return value_.GetAndAdd(val);
#endif
	}

	T value() 
	{
#ifdef WIN32
		return value_;
#else
		return value_.value();
#endif
	}

private:
#ifdef WIN32
	T value_;
#else
	detail::AtomicIntegerT<T> value_;
#endif
};

typedef Atomic<long> AtomicIntegerL;
typedef Atomic<unsigned long> AtomicIntegerUL;

#endif