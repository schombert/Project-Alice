//
// This file is derived from https://github.com/kocienda/Any
// With *very* minor tweaks
//

//
// cyto-any.h 
//
// A class modeled on std::any.
//
// MIT License
// 
// Copyright (c) 2020 Ken Kocienda
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <exception>
#include <initializer_list>
#include <new>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <string.h>

#ifndef ANY_ALWAYS_INLINE
#ifdef NDEBUG
#define ANY_ALWAYS_INLINE inline __attribute__ ((__visibility__("hidden"), __always_inline__))
#else
#define ANY_ALWAYS_INLINE inline
#endif
#endif

#ifdef __cpp_rtti
#define ANY_USE_TYPEINFO 1
#else
#define ANY_USE_TYPEINFO 0
#endif

#ifdef __cpp_exceptions
#define ANY_USE_EXCEPTIONS 1
#else
#define ANY_USE_EXCEPTIONS 0
#endif

#ifndef ANY_USE_SMALL_MEMCPY_STRATEGY
#define ANY_USE_SMALL_MEMCPY_STRATEGY 0
#endif

#define ANY_USE(FEATURE) (defined ANY_USE_##FEATURE && ANY_USE_##FEATURE)

namespace Cyto {

#if ANY_USE_EXCEPTIONS
class bad_any_cast : public std::bad_cast { };
#endif  // ANY_USE(EXCEPTIONS)

static ANY_ALWAYS_INLINE void handle_bad_any_cast() {
#if ANY_USE_EXCEPTIONS
	throw bad_any_cast();
#endif
	abort();
}

template <class T>  struct IsInPlaceType_ : std::false_type { };
template <>         struct IsInPlaceType_<std::in_place_t> : std::true_type { };
template <class T>  struct IsInPlaceType_<std::in_place_type_t<T>> : std::true_type { };
template <size_t S> struct IsInPlaceType_<std::in_place_index_t<S>> : std::true_type { };
template <class T>  constexpr bool IsInPlaceType = IsInPlaceType_<T>::value;

constexpr size_t StorageBufferSize = 3 * sizeof(void*);
using StorageBuffer = std::aligned_storage_t<StorageBufferSize, std::alignment_of_v<void*>>;

template <class T>
using IsStorageBufferSized_ =
std::bool_constant<sizeof(T) <= sizeof(StorageBuffer) &&
	std::alignment_of_v<StorageBuffer> % std::alignment_of_v<T> == 0>;

template <class T> constexpr bool IsStorageBufferSized = IsStorageBufferSized_<T>::value;

union Storage {
	constexpr Storage() { }
	Storage(const Storage&) = delete;
	Storage(Storage&&) = delete;
	Storage& operator=(const Storage&) = delete;
	Storage& operator=(Storage&&) = delete;

	StorageBuffer buf;
	void* ptr = nullptr;
};

#if !ANY_USE_TYPEINFO
template <class T>
struct fallback_typeinfo {
	static constexpr int id = 0;
};

template <class T>
ANY_ALWAYS_INLINE
constexpr const void* fallback_typeid() {
	return &fallback_typeinfo<std::decay_t<T>>::id;
}
#endif  // !ANY_USE(TYPEINFO)

ANY_ALWAYS_INLINE
static constexpr void* void_get(Storage* s, const void* info) {
	return nullptr;
}

ANY_ALWAYS_INLINE
static constexpr void void_copy(Storage* dst, const Storage* src) { }

ANY_ALWAYS_INLINE
static constexpr void void_move(Storage* dst, Storage* src) { }

ANY_ALWAYS_INLINE
static constexpr void void_drop(Storage* s) { }

struct AnyActions {
	using Get = void* (*)(Storage* s, const void* type);
	using Copy = void (*)(Storage* dst, const Storage* src);
	using Move = void (*)(Storage* dst, Storage* src);
	using Drop = void (*)(Storage* s);

	constexpr AnyActions() noexcept { }

	constexpr AnyActions(Get g, Copy c, Move m, Drop d, const void* t) noexcept :
		get(g), copy(c), move(m), drop(d), type(t) { }

	Get get = void_get;
	Copy copy = void_copy;
	Move move = void_move;
	Drop drop = void_drop;
#if ANY_USE_TYPEINFO
	const void* type = static_cast<const void*>(&typeid(void));
#else
	const void* type = fallback_typeid<void>();
#endif
};

template <class T>
struct AnyTraits {
#if ANY_USE_SMALL_MEMCPY_STRATEGY
	template <class X = T, class... Args,
		std::enable_if_t<IsStorageBufferSized<X>&& std::is_trivially_copyable_v<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static X& make(Storage* s, std::in_place_type_t<X> vtype, Args &&... args) {
		X v(std::forward<Args>(args)...);
		memcpy(&s->buf, static_cast<void*>(&v), sizeof(X));
		return *(static_cast<X*>(static_cast<void*>(&s->buf)));
	}

	template <class X = T, class... Args,
		std::enable_if_t<IsStorageBufferSized<X> && !std::is_trivially_copyable_v<X>&&
		std::is_nothrow_move_constructible_v<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static X& make(Storage* s, std::in_place_type_t<X> vtype, Args &&... args) {
		return *(::new (static_cast<void*>(&s->buf)) X(std::forward<Args>(args)...));
	}
#else  // ANY_USE(SMALL_MEMCPY_STRATEGY)
	template <class X = T, class... Args,
		std::enable_if_t<IsStorageBufferSized<X>&&
		std::is_nothrow_move_constructible_v<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static X& make(Storage* s, std::in_place_type_t<X> vtype, Args &&... args) {
		return *(::new (static_cast<void*>(&s->buf)) X(std::forward<Args>(args)...));
	}
#endif  // ANY_USE(SMALL_MEMCPY_STRATEGY)

	template <class X = T, class... Args,
		std::enable_if_t<!IsStorageBufferSized<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static X& make(Storage* s, std::in_place_type_t<X> vtype, Args &&... args) {
		s->ptr = new X(std::forward<Args>(args)...);
		return *static_cast<X*>(s->ptr);
	}

private:
	AnyTraits(const AnyTraits&) = default;
	AnyTraits(AnyTraits&&) = default;
	AnyTraits& operator=(const AnyTraits&) = default;
	AnyTraits& operator=(AnyTraits&&) = default;

	template <class X = T>
	ANY_ALWAYS_INLINE
		static bool compare_typeid(const void* id) {
#if ANY_USE_TYPEINFO
		return *(static_cast<const std::type_info*>(id)) == typeid(X);
#else
		return (id && id == fallback_typeid<X>());
#endif
	}

	//
	// get
	//
	template <class X = T,
		std::enable_if_t<std::is_same_v<X, void>, int> = 0>
	ANY_ALWAYS_INLINE
		static void* get(Storage* s, const void* type) {
		return nullptr;
	}

	template <class X = T,
		std::enable_if_t<IsStorageBufferSized<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void* get(Storage* s, const void* type) {
		if(compare_typeid<X>(type)) {
			return static_cast<void*>(&s->buf);
		}
		return nullptr;
	}

	template <class X = T,
		std::enable_if_t<!IsStorageBufferSized<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void* get(Storage* s, const void* type) {
		if(compare_typeid<X>(type)) {
			return s->ptr;
		}
		return nullptr;
	}

	//
	// copy
	//
	template <class X = T,
		std::enable_if_t<std::is_same_v<X, void>, int> = 0>
	ANY_ALWAYS_INLINE
		static void copy(Storage* dst, const Storage* src) { }

#if ANY_USE_SMALL_MEMCPY_STRATEGY
	template <class X = T,
		std::enable_if_t<IsStorageBufferSized<X>&& std::is_trivially_copyable_v<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void copy(Storage* dst, const Storage* src) {
		memcpy(static_cast<void*>(&dst->buf), static_cast<void*>(const_cast<StorageBuffer*>(&src->buf)), sizeof(X));
	}

	template <class X = T,
		std::enable_if_t<IsStorageBufferSized<X> && !std::is_trivially_copyable_v<X>&&
		std::is_nothrow_move_constructible_v<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void copy(Storage* dst, const Storage* src) {
		AnyTraits::make(dst, std::in_place_type_t<X>(), *static_cast<X const*>(static_cast<void const*>(&src->buf)));
	}
#else  // ANY_USE(SMALL_MEMCPY_STRATEGY)
	template <class X = T,
		std::enable_if_t<IsStorageBufferSized<X>&&
		std::is_nothrow_move_constructible_v<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void copy(Storage* dst, const Storage* src) {
		AnyTraits::make(dst, std::in_place_type_t<X>(), *static_cast<X const*>(static_cast<void const*>(&src->buf)));
	}
#endif   // ANY_USE(SMALL_MEMCPY_STRATEGY)

	template <class X = T,
		std::enable_if_t<!IsStorageBufferSized<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void copy(Storage* dst, const Storage* src) {
		AnyTraits::make(dst, std::in_place_type_t<X>(), *static_cast<X const*>(static_cast<void const*>(src->ptr)));
	}

	//
	// move
	//
#if ANY_USE_SMALL_MEMCPY_STRATEGY
	template <class X = T,
		std::enable_if_t<std::is_same_v<X, void>, int> = 0>
	ANY_ALWAYS_INLINE
		static void move(Storage* dst, Storage* src) { }

	template <class X = T,
		std::enable_if_t<IsStorageBufferSized<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void move(Storage* dst, Storage* src) {
		memcpy(static_cast<void*>(&dst->buf), static_cast<void*>(const_cast<StorageBuffer*>(&src->buf)), sizeof(X));
	}
#else  // ANY_USE(SMALL_MEMCPY_STRATEGY)
	template <class X = T,
		std::enable_if_t<IsStorageBufferSized<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void move(Storage* dst, Storage* src) {
		AnyTraits::make(dst, std::in_place_type_t<X>(), std::move(*static_cast<X const*>(static_cast<void const*>(&src->buf))));
	}
#endif   // ANY_USE(SMALL_MEMCPY_STRATEGY)

	template <class X = T,
		std::enable_if_t<!IsStorageBufferSized<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void move(Storage* dst, Storage* src) {
		dst->ptr = src->ptr;
	}

	//
	// drop
	//
	template <class X = T,
		std::enable_if_t<std::is_same_v<X, void>, int> = 0>
	ANY_ALWAYS_INLINE
		static void drop(Storage* s) { }

	template <class X = T,
		std::enable_if_t<std::is_trivially_destructible_v<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void drop(Storage* s) { }

	template <class X = T,
		std::enable_if_t<IsStorageBufferSized<X> && !std::is_trivially_destructible_v<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void drop(Storage* s) {
		X& t = *static_cast<X*>(static_cast<void*>(const_cast<StorageBuffer*>(&s->buf)));
		t.~X();
	}

	template <class X = T,
		std::enable_if_t<!IsStorageBufferSized<X>, int> = 0>
	ANY_ALWAYS_INLINE
		static void drop(Storage* s) {
		delete static_cast<X*>(s->ptr);
	}

public:
	static constexpr AnyActions actions = AnyActions(get<T>, copy<T>, move<T>, drop<T>,
#if ANY_USE_TYPEINFO
		& typeid(T)
#else
		fallback_typeid<T>()
#endif
	);
};

class Any;

template <class V, class T = std::decay_t<V>>
using IsAnyConstructible_ =
std::bool_constant<!std::is_same_v<T, Any> && !IsInPlaceType<V>&&
	std::is_copy_constructible_v<T>>;

template <class V> constexpr bool IsAnyConstructible = IsAnyConstructible_<V>::value;

template <class T, class U, class ...Args>
using IsAnyInitializerListConstructible_ =
std::bool_constant<std::is_constructible_v<T, std::initializer_list<U>&, Args...>&&
	std::is_copy_constructible_v<T>>;

template <class T, class U, class ...Args> constexpr bool IsAnyInitializerListConstructible =
IsAnyInitializerListConstructible_<T, U, Args...>::value;

class Any {
public:
	constexpr Any() noexcept : actions(VoidAnyActions) { }

	template <class V, class T = std::decay_t<V>, std::enable_if_t<IsAnyConstructible<V>, int> = 0>
	Any(V&& v) : actions(&AnyTraits<T>::actions) {
		AnyTraits<T>::make(&storage, std::in_place_type_t<T>(), std::forward<V>(v));
	}

	template <class V, class... Args, class T = std::decay_t<V>, std::enable_if_t<IsAnyConstructible<T>, int> = 0>
	explicit Any(std::in_place_type_t<V> vtype, Args &&... args) : actions(&AnyTraits<T>::actions) {
		AnyTraits<T>::make(&storage, vtype, std::forward<Args>(args)...);
	}

	template <class V, class U, class ...Args, class T = std::decay_t<V>,
		std::enable_if_t<IsAnyInitializerListConstructible<T, U, Args...>, int> = 0>
	explicit Any(std::in_place_type_t<V> vtype, std::initializer_list<U> list, Args &&... args) :
		actions(&AnyTraits<T>::actions) {
		AnyTraits<T>::make(&storage, vtype, V{ list, std::forward<Args>(args)... });
	}

	Any(const Any& other) : actions(other.actions) {
		actions->copy(&storage, &other.storage);
	}

	Any(Any&& other) noexcept : actions(other.actions) {
		actions->move(&storage, &other.storage);
		other.actions = VoidAnyActions;
	}

	Any& operator=(const Any& other) {
		if(this != &other) {
			actions->drop(&storage);
			actions = other.actions;
			actions->copy(&storage, &other.storage);
		}
		return *this;
	}

	Any& operator=(Any&& other) noexcept {
		if(this != &other) {
			actions->drop(&storage);
			actions = other.actions;
			actions->move(&storage, &other.storage);
			other.actions = VoidAnyActions;
		}
		return *this;
	}

	template <class V, class T = std::decay_t<V>, std::enable_if_t<IsAnyConstructible<T>, int> = 0>
	Any& operator=(V&& v) {
		*this = Any(std::forward<V>(v));
		return *this;
	}

	~Any() {
		actions->drop(&storage);
	}

	template <class V, class... Args, class T = std::decay_t<V>,
		std::enable_if_t<std::is_constructible_v<T, Args...>&& std::is_copy_constructible_v<T>, int> = 0>
	T& emplace(Args &&... args) {
		actions->drop(&storage);
		actions = &AnyTraits<T>::actions;
		return AnyTraits<T>::make(&storage, std::in_place_type_t<T>(), std::forward<Args>(args)...);
	}

	template <class V, class U, class... Args, class T = std::decay_t<V>,
		std::enable_if_t<IsAnyInitializerListConstructible<T, U, Args...>, int> = 0>
	T& emplace(std::initializer_list<U> list, Args &&... args) {
		reset();
		actions = &AnyTraits<T>::actions;
		return AnyTraits<T>::make(&storage, std::in_place_type_t<T>(), V{ list, std::forward<Args>(args)... });
	}

	ANY_ALWAYS_INLINE
		void reset() {
		actions->drop(&storage);
		actions = VoidAnyActions;
	}

	ANY_ALWAYS_INLINE
		void swap(Any& rhs) noexcept {
		if(this == &rhs) {
			return;
		}

		Any tmp;

		// swap storage
		rhs.actions->move(&tmp.storage, &rhs.storage);
		actions->move(&rhs.storage, &storage);
		rhs.actions->move(&storage, &tmp.storage);

		// swap actions
		tmp.actions = rhs.actions;
		rhs.actions = actions;
		actions = tmp.actions;
	}

	template <bool B>
	ANY_ALWAYS_INLINE
		bool has_value() const noexcept {
		return (actions != VoidAnyActions) == B;
	}

	ANY_ALWAYS_INLINE
		bool has_value() const noexcept {
		return has_value<true>();
	}

#if ANY_USE_TYPEINFO
	const std::type_info& type() const noexcept {
		return *static_cast<const std::type_info*>(actions->type);
	}
#endif

	template<typename CHECK_TYPE>
	ANY_ALWAYS_INLINE
	bool holds_type() const noexcept {
#if ANY_USE_TYPEINFO
		return typeid(CHECK_TYPE) == *static_cast<const std::type_info*>(actions->type);
#else
		return (actions->type == fallback_typeid<CHECK_TYPE>());
#endif
	}

	template <class V> friend std::remove_cv_t<std::remove_reference_t<V>>* any_cast(Any* a) noexcept;

private:
	static constexpr AnyActions _VoidAnyActions = AnyActions();
	static constexpr const AnyActions* const VoidAnyActions = &_VoidAnyActions;

	const AnyActions* actions;
	Storage storage;
};

ANY_ALWAYS_INLINE
void swap(Any& lhs, Any& rhs) noexcept {
	lhs.swap(rhs);
}

template <class T, class ...Args>
ANY_ALWAYS_INLINE
Any make_any(Args&&... args) {
	return Any(std::in_place_type<T>, std::forward<Args>(args)...);
}

template <class T, class U, class ...Args>
ANY_ALWAYS_INLINE
Any make_any(std::initializer_list<U> il, Args&&... args) {
	return Any(std::in_place_type<T>, il, std::forward<Args>(args)...);
}

template <class V, class T = std::remove_cv_t<std::remove_reference_t<V>>,
	std::enable_if_t < std::is_constructible<V, const T&>{}, int > = 0 >
V any_cast(const Any & a) {
	auto tmp = any_cast<std::add_const_t<T>>(&a);
	if(tmp == nullptr) {
		handle_bad_any_cast();
	}
	return static_cast<V>(*tmp);
}

template <class V, class T = std::remove_cv_t<std::remove_reference_t<V>>,
	std::enable_if_t < std::is_constructible<V, const T&>{}, int > = 0 >
V any_cast(Any & a) {
	auto tmp = any_cast<T>(&a);
	if(tmp == nullptr) {
		handle_bad_any_cast();
	}
	return static_cast<V>(*tmp);
}

template <class V, class T = std::remove_cv_t<std::remove_reference_t<V>>,
	std::enable_if_t < std::is_constructible<V, const T&>{}, int > = 0 >
V any_cast(Any && a) {
	auto tmp = any_cast<T>(&a);
	if(tmp == nullptr) {
		handle_bad_any_cast();
	}
	return static_cast<V>(std::move(*tmp));
}

template <class V, class T = std::remove_cv_t<std::remove_reference_t<V>>>
const T* any_cast(const Any* a) noexcept {
	return any_cast<V>(const_cast<Any*>(a));
}

template <class V>
std::remove_cv_t<std::remove_reference_t<V>>* any_cast(Any* a) noexcept {
	using T = std::remove_cv_t<std::remove_reference_t<V>>;
	using U = std::decay_t<V>;
	if(a && a->has_value()) {
		void* p = a->actions->get(&a->storage,
#if ANY_USE_TYPEINFO
		& typeid(U)
#else
		fallback_typeid<U>()
#endif
		);
		return (std::is_function<V>{}) ? nullptr : static_cast<T*>(p);
	}
	return nullptr;
}

}  // namespace Cyto
