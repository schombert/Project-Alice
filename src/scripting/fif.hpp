#pragma once
#include "unordered_dense.h"
#include <vector>
#include <optional>
#include <variant>
#include "stdint.h"
#include <string>
#include <string_view>
#include <assert.h>
#include <charconv>
#include <span>
#include <limits>

#ifdef _WIN64
#define USE_LLVM
#else
#endif

#ifdef USE_LLVM

#include "llvm-c/Core.h"
#include "llvm-c/Types.h"
#include "llvm-c/LLJIT.h"
#include "llvm-c/LLJITUtils.h"
#include "llvm-c/Analysis.h"
#include "llvm-c/Error.h"
#include "llvm-c/ExecutionEngine.h"
#include "llvm-c/Orc.h"
#include "llvm-c/OrcEE.h"
#include "llvm-c/Linker.h"
#include "llvm-c/lto.h"
#include "llvm-c/Support.h"
#include "llvm-c/Target.h"
#include "llvm-c/TargetMachine.h"
#include "llvm-c/Transforms/PassBuilder.h"

#ifdef _WIN64
#define NATIVE_CC LLVMCallConv::LLVMWin64CallConv
#else
#define NATIVE_CC LLVMCallConv::LLVMX8664SysVCallConv
#endif

#endif
namespace fif {

enum class stack_type {
	interpreter_stack = 0,
	bytecode_compiler = 1,
	llvm_compiler = 2
};

struct indirect_string_hash {
	using is_avalanching = void;
	using is_transparent = void;

	indirect_string_hash() {
	}
	auto operator()(std::string_view sv) const noexcept -> uint64_t {
		return ankerl::unordered_dense::detail::wyhash::hash(sv.data(), sv.size());
	}
	auto operator()(std::unique_ptr<char[]> const& str) const noexcept -> uint64_t {
		return ankerl::unordered_dense::detail::wyhash::hash(str.get(), std::string_view{ str.get() }.size());
	}
};
struct indirect_string_eq {
	using is_transparent = void;


	indirect_string_eq() {
	}

	bool operator()(std::unique_ptr<char[]> const& l, std::unique_ptr<char[]> const& r) const noexcept {
		return std::string_view{ l.get() } == std::string_view{ r.get() };
	}
	bool operator()(std::unique_ptr<char[]> const& l, std::string_view r) const noexcept {
		return std::string_view{ l.get() } == r;
	}
	bool operator()(std::string_view r, std::unique_ptr<char[]> const& l) const noexcept {
		return std::string_view{ l.get() } == r;
	}
	bool operator()(std::unique_ptr<char[]> const& l, std::string const& r) const noexcept {
		return std::string_view{ l.get() } == r;
	}
	bool operator()(std::string const& r, std::unique_ptr<char[]> const& l) const noexcept {
		return std::string_view{ l.get() } == r;
	}
};

#ifdef USE_LLVM
#else
#define LLVMValueRef void*
#define LLVMBasicBlockRef void*
#define LLVMTypeRef void*
#endif

class state_stack {
protected:
	std::vector<int32_t> main_types;
	std::vector<int32_t> return_types;
public:
	size_t min_main_depth = 0;
	size_t min_return_depth = 0;

	void mark_used_from_main(size_t count) {
		min_main_depth = std::min(std::max(main_types.size(), count) - count, min_main_depth);
	}
	void mark_used_from_return(size_t count) {
		min_return_depth = std::min(std::max(return_types.size(), count) - count, min_return_depth);
	}

	state_stack() = default;
	state_stack(state_stack&&) = default;
	state_stack(const state_stack&) = default;
	virtual ~state_stack() = default;

	virtual void pop_main() = 0;
	virtual void pop_return() = 0;
	virtual int64_t main_data(size_t index) const = 0;
	virtual int64_t return_data(size_t index) const = 0;
	virtual LLVMValueRef main_ex(size_t index) const = 0;
	virtual LLVMValueRef return_ex(size_t index) const = 0;
	virtual int64_t main_data_back(size_t index) const = 0;
	virtual int64_t return_data_back(size_t index) const = 0;
	virtual LLVMValueRef main_ex_back(size_t index) const = 0;
	virtual LLVMValueRef return_ex_back(size_t index) const = 0;
	virtual void set_main_data(size_t index, int64_t value) = 0;
	virtual void set_return_data(size_t index, int64_t value)  = 0;
	virtual void set_main_ex(size_t index, LLVMValueRef value)  = 0;
	virtual void set_return_ex(size_t index, LLVMValueRef value)  = 0;
	virtual void set_main_data_back(size_t index, int64_t value)  = 0;
	virtual void set_return_data_back(size_t index, int64_t value)  = 0;
	virtual void set_main_ex_back(size_t index, LLVMValueRef value)  = 0;
	virtual void set_return_ex_back(size_t index, LLVMValueRef value)  = 0;
	virtual stack_type get_type() const = 0;
	virtual void move_into(state_stack&& other) = 0;
	virtual void copy_into(state_stack const& other) = 0;
	virtual void push_back_main(int32_t type, int64_t data, LLVMValueRef expr) = 0;
	virtual void push_back_return(int32_t type, int64_t data, LLVMValueRef expr) = 0;
	virtual std::unique_ptr< state_stack> copy() const = 0;
	virtual void resize(size_t main_sz, size_t return_sz) = 0;

	size_t main_size() const {
		return main_types.size();
	}
	size_t return_size()const {
		return return_types.size();
	}
	int32_t main_type(size_t index) const {
		return std::abs(main_types[index]);
	}
	int32_t return_type(size_t index) const {
		return std::abs(return_types[index]);
	}
	int32_t main_type_back(size_t index) const {
		return std::abs(main_types[main_types.size() - (index + 1)]);
	}
	int32_t return_type_back(size_t index) const {
		return std::abs(return_types[return_types.size() - (index + 1)]);
	}
	void set_main_type(size_t index, int32_t type) {
		main_types[index] = type;
		min_main_depth = std::min(min_main_depth, index);
	}
	void set_return_type(size_t index, int32_t type) {
		return_types[index] = type;
		min_return_depth = std::min(min_return_depth, index);
	}
	void set_main_type_back(size_t index, int32_t type) {
		main_types[main_types.size() - (index + 1)] = type;
		min_main_depth = std::min(min_main_depth, main_types.size() - (index + 1));
	}
	void set_return_type_back(size_t index, int32_t type) {
		return_types[return_types.size() - (index + 1)] = type;
		min_return_depth = std::min(min_return_depth, return_types.size() - (index + 1));
	}
};

class llvm_stack : public state_stack {
public:
	std::vector<LLVMValueRef> main_exs;
	std::vector<LLVMValueRef> return_exs;

	virtual void pop_main() override {
		if(main_exs.empty())
			return;
		main_exs.pop_back();
		main_types.pop_back();
		min_main_depth = std::min(min_main_depth, main_types.size());
	}
	virtual void pop_return() override {
		if(return_exs.empty())
			return;
		return_exs.pop_back();
		return_types.pop_back();
		min_return_depth = std::min(min_return_depth, return_types.size());
	}
	virtual void resize(size_t main_sz, size_t return_sz) override {
		min_return_depth = std::min(min_return_depth, return_sz);
		min_main_depth = std::min(min_main_depth, main_sz);
		return_types.resize(return_sz);
		return_exs.resize(return_sz);
		main_types.resize(main_sz);
		main_exs.resize(main_sz);
	}
	virtual int64_t main_data(size_t index) const  override {
		return 0;
	}
	virtual int64_t return_data(size_t index) const override {
		return 0;
	}
	virtual LLVMValueRef main_ex(size_t index) const override {
		return main_exs[index];
	}
	virtual LLVMValueRef return_ex(size_t index) const override {
		return return_exs[index];
	}
	virtual int64_t main_data_back(size_t index)const override {
		return 0;
	}
	virtual int64_t return_data_back(size_t index)const override {
		return 0;
	}
	virtual LLVMValueRef main_ex_back(size_t index)const override {
		return main_exs[main_exs.size() - (index + 1)];
	}
	virtual LLVMValueRef return_ex_back(size_t index) const override {
		return return_exs[return_exs.size() - (index + 1)];
	}
	virtual void set_main_data(size_t index, int64_t value) override {
		min_main_depth = std::min(min_main_depth, index);
	}
	virtual void set_return_data(size_t index, int64_t value)override {
		min_return_depth = std::min(min_return_depth, index);
	}
	virtual void set_main_ex(size_t index, LLVMValueRef value) override {
		main_exs[index] = value;
		min_main_depth = std::min(min_main_depth, index);
	}
	virtual void set_return_ex(size_t index, LLVMValueRef value) override {
		return_exs[index] = value;
		min_return_depth = std::min(min_return_depth, index);
	}
	virtual void set_main_data_back(size_t index, int64_t value)override {
		min_main_depth = std::min(min_main_depth, main_types.size() - (index + 1));
	}
	virtual void set_return_data_back(size_t index, int64_t value) override {
		min_return_depth = std::min(min_return_depth, return_types.size() - (index + 1));
	}
	virtual void set_main_ex_back(size_t index, LLVMValueRef value)override {
		main_exs[main_exs.size() - (index + 1)] = value;
		min_main_depth = std::min(min_main_depth, main_types.size() - (index + 1));
	}
	virtual void set_return_ex_back(size_t index, LLVMValueRef value)override {
		return_exs[return_exs.size() - (index + 1)] = value;
		min_return_depth = std::min(min_return_depth, return_types.size() - (index + 1));
	}
	virtual stack_type get_type() const override {
		return stack_type::llvm_compiler;
	}
	virtual void move_into(state_stack&& other)override {
		if(other.get_type() != stack_type::llvm_compiler)
			std::abort();

		llvm_stack&& o = static_cast<llvm_stack&&>(other);
		main_exs = std::move(o.main_exs);
		return_exs = std::move(o.return_exs);
		main_types = std::move(o.main_types);
		return_types = std::move(o.return_types);
		min_main_depth = std::min(min_main_depth, o.min_main_depth);
		min_return_depth = std::min(min_return_depth, o.min_return_depth);
	}
	virtual void copy_into(state_stack const& other)override {
		if(other.get_type() != stack_type::llvm_compiler)
			std::abort();

		llvm_stack const& o = static_cast<llvm_stack const&>(other);
		main_exs = o.main_exs;
		return_exs = o.return_exs;
		main_types = o.main_types;
		return_types = o.return_types;
		min_main_depth = std::min(min_main_depth, o.min_main_depth);
		min_return_depth = std::min(min_return_depth, o.min_return_depth);
	}
	virtual void push_back_main(int32_t type, int64_t data, LLVMValueRef expr)override {
		main_exs.push_back(expr);
		main_types.push_back(type);
	}
	virtual void push_back_return(int32_t type, int64_t data, LLVMValueRef expr)override {
		return_exs.push_back(expr);
		return_types.push_back(type);
	}
	virtual std::unique_ptr<state_stack> copy() const override {
		auto temp_new = std::make_unique<llvm_stack>(*this);
		temp_new->min_main_depth = main_types.size();
		temp_new->min_return_depth = return_types.size();
		return temp_new;
	}
};

class type_stack : public state_stack {
public:
	virtual void pop_main() override {
		if(main_types.empty())
			return;
		main_types.pop_back();
		min_main_depth = std::min(min_main_depth, main_types.size());
	}
	virtual void pop_return()override {
		if(return_types.empty())
			return;
		return_types.pop_back();
		min_return_depth = std::min(min_return_depth, return_types.size());
	}
	virtual void resize(size_t main_sz, size_t return_sz) override {
		min_return_depth = std::min(min_return_depth, return_sz);
		min_main_depth = std::min(min_main_depth, main_sz);
		return_types.resize(return_sz);
		main_types.resize(main_sz);
	}
	virtual int64_t main_data(size_t index) const override {
		return 0;
	}
	virtual int64_t return_data(size_t index) const override {
		return 0;
	}
	virtual LLVMValueRef main_ex(size_t index) const override {
		return nullptr;
	}
	virtual LLVMValueRef return_ex(size_t index) const override {
		return nullptr;
	}
	virtual int64_t main_data_back(size_t index)const override {
		return 0;
	}
	virtual int64_t return_data_back(size_t index)const override {
		return 0;
	}
	virtual LLVMValueRef main_ex_back(size_t index)const override {
		return nullptr;
	}
	virtual LLVMValueRef return_ex_back(size_t index) const override {
		return nullptr;
	}
	virtual stack_type get_type() const override {
		return stack_type::bytecode_compiler;
	}
	virtual void set_main_data(size_t index, int64_t value)override {
		min_main_depth = std::min(min_main_depth, index);
	}
	virtual void set_return_data(size_t index, int64_t value) override {
		min_return_depth = std::min(min_return_depth, index);
	}
	virtual void set_main_ex(size_t index, LLVMValueRef value) override {
		min_main_depth = std::min(min_main_depth, index);
	}
	virtual void set_return_ex(size_t index, LLVMValueRef value)override {
		min_return_depth = std::min(min_return_depth, index);
	}
	virtual void set_main_data_back(size_t index, int64_t value) override {
		min_main_depth = std::min(min_main_depth, main_types.size() - (index + 1));
	}
	virtual void set_return_data_back(size_t index, int64_t value) override {
		min_return_depth = std::min(min_return_depth, return_types.size() - (index + 1));
	}
	virtual void set_main_ex_back(size_t index, LLVMValueRef value) override {
		min_main_depth = std::min(min_main_depth, main_types.size() - (index + 1));
	}
	virtual void set_return_ex_back(size_t index, LLVMValueRef value) override {
		min_return_depth = std::min(min_return_depth, return_types.size() - (index + 1));
	}
	virtual void move_into(state_stack&& other) override {
		if(other.get_type() != stack_type::bytecode_compiler)
			std::abort();

		type_stack&& o = static_cast<type_stack&&>(other);
		main_types = std::move(o.main_types);
		return_types = std::move(o.return_types);
		min_main_depth = std::min(min_main_depth, o.min_main_depth);
		min_return_depth = std::min(min_return_depth, o.min_return_depth);
	}
	virtual void copy_into(state_stack const& other) override {
		if(other.get_type() != stack_type::bytecode_compiler)
			std::abort();

		type_stack const& o = static_cast<type_stack const&>(other);
		main_types = o.main_types;
		return_types = o.return_types;
		min_main_depth = std::min(min_main_depth, o.min_main_depth);
		min_return_depth = std::min(min_return_depth, o.min_return_depth);
	}
	virtual void push_back_main(int32_t type, int64_t data, LLVMValueRef expr) override {
		main_types.push_back(type);
	}
	virtual void push_back_return(int32_t type, int64_t data, LLVMValueRef expr) override {
		return_types.push_back(type);
	}
	virtual std::unique_ptr<state_stack> copy() const override {
		auto temp_new = std::make_unique<type_stack>(*this);
		temp_new->min_main_depth = main_types.size();
		temp_new->min_return_depth = return_types.size();
		return temp_new;
	}
};


class environment;
struct type;

using fif_call = int32_t * (*)(state_stack&, int32_t*, environment*);

struct interpreted_word_instance {
	std::vector<int32_t> compiled_bytecode;
	LLVMValueRef llvm_function = nullptr;
	int32_t stack_types_start = 0;
	int32_t stack_types_count = 0;
	int32_t typechecking_level = 0;
	bool being_compiled = false;
	bool llvm_compilation_finished = false;
	bool is_imported_function = false;
};

struct compiled_word_instance {
	fif_call implementation = nullptr;
	int32_t stack_types_start = 0;
	int32_t stack_types_count = 0;
};

using word_types = std::variant<interpreted_word_instance, compiled_word_instance>;

struct word {
	std::vector<int32_t> instances;
	std::string source;
	int32_t specialization_of = -1;
	int32_t stack_types_start = 0;
	int32_t stack_types_count = 0;
	bool treat_as_base = false;
	bool immediate =  false;
	bool being_typechecked = false;
};

inline LLVMValueRef empty_type_fn(LLVMValueRef r, int32_t type, environment*) {
	return r;
}

#ifdef USE_LLVM
using llvm_zero_expr = LLVMValueRef (*)(LLVMContextRef, int32_t t, environment*);
#endif
using interpreter_zero_expr = int64_t(*)(int32_t t, environment*);
using interpreted_new = int64_t(*)();

struct type {
	static constexpr uint32_t FLAG_REFCOUNTED = 0x00000001;
	static constexpr uint32_t FLAG_SINGLE_MEMBER = 0x00000002;
	static constexpr uint32_t FLAG_TEMPLATE = 0x00000004;

#ifdef USE_LLVM
	LLVMTypeRef llvm_type = nullptr;
	llvm_zero_expr zero_constant = nullptr;
#else
	void* llvm_type = nullptr;
	void* zero_constant = nullptr;
#endif
	interpreter_zero_expr interpreter_zero = nullptr;

	int32_t type_slots = 0;
	int32_t non_member_types = 0;
	int32_t decomposed_types_start = 0;
	int32_t decomposed_types_count = 0;

	uint32_t flags = 0;

	bool refcounted_type() const {
		return (flags & FLAG_REFCOUNTED) != 0;
	}
	bool single_member_struct() const {
		return (flags & FLAG_SINGLE_MEMBER) != 0;
	}
	bool is_struct_template() const {
		return (flags & FLAG_TEMPLATE) != 0;
	}
};

constexpr inline int32_t fif_i32 = 0;
constexpr inline int32_t fif_f32 = 1;
constexpr inline int32_t fif_bool = 2;
constexpr inline int32_t fif_type = 3;
constexpr inline int32_t fif_i64 = 4;
constexpr inline int32_t fif_f64 = 5;
constexpr inline int32_t fif_u32 = 6;
constexpr inline int32_t fif_u64 = 7;
constexpr inline int32_t fif_i16 = 8;
constexpr inline int32_t fif_u16 = 9;
constexpr inline int32_t fif_i8 = 10;
constexpr inline int32_t fif_u8 = 11;
constexpr inline int32_t fif_nil = 12;
constexpr inline int32_t fif_ptr = 13;
constexpr inline int32_t fif_opaque_ptr = 14;
constexpr inline int32_t fif_struct = 15;

class environment;

class dictionary {
public:
	ankerl::unordered_dense::map<std::string, int32_t> words;
	ankerl::unordered_dense::map<std::string, int32_t> types;
	std::vector<word> word_array;
	std::vector<type> type_array;
	std::vector<word_types> all_instances;
	std::vector<int32_t> all_compiled;
	std::vector<int32_t> all_stack_types;

#ifdef USE_LLVM
	void ready_llvm_types(LLVMContextRef llvm_context) {
		type_array[fif_i32].llvm_type = LLVMInt32TypeInContext(llvm_context);
		type_array[fif_i32].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt32TypeInContext(c), 0, true);
		};
		type_array[fif_type].llvm_type = LLVMInt32TypeInContext(llvm_context);
		type_array[fif_type].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt32TypeInContext(c), 0, true);
		};
		type_array[fif_f32].llvm_type = LLVMFloatTypeInContext(llvm_context);
		type_array[fif_f32].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstReal(LLVMFloatTypeInContext(c), 0.0);
		};
		type_array[fif_i64].llvm_type = LLVMInt64TypeInContext(llvm_context);
		type_array[fif_i64].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt64TypeInContext(c), 0, true);
		};
		type_array[fif_f64].llvm_type = LLVMDoubleTypeInContext(llvm_context);
		type_array[fif_f64].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstReal(LLVMDoubleTypeInContext(c), 0.0);
		};
		type_array[fif_bool].llvm_type =  LLVMInt1TypeInContext(llvm_context);
		type_array[fif_bool].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt1TypeInContext(c), 0, true);
		};
		type_array[fif_u32].llvm_type = LLVMInt32TypeInContext(llvm_context);
		type_array[fif_u32].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt32TypeInContext(c), 0, false);
		};
		type_array[fif_u64].llvm_type = LLVMInt64TypeInContext(llvm_context);
		type_array[fif_u64].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt64TypeInContext(c), 0, false);
		};
		type_array[fif_u16].llvm_type = LLVMInt16TypeInContext(llvm_context);
		type_array[fif_u16].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt16TypeInContext(c), 0, false);
		};
		type_array[fif_u8].llvm_type = LLVMInt8TypeInContext(llvm_context);
		type_array[fif_u8].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt8TypeInContext(c), 0, false);
		};
		type_array[fif_i16].llvm_type = LLVMInt16TypeInContext(llvm_context);
		type_array[fif_i16].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt16TypeInContext(c), 0, true);
		};
		type_array[fif_i8].llvm_type = LLVMInt8TypeInContext(llvm_context);
		type_array[fif_i8].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMConstInt(LLVMInt8TypeInContext(c), 0, true);
		};
		type_array[fif_nil].llvm_type = LLVMVoidTypeInContext(llvm_context);
		type_array[fif_nil].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMGetUndef(LLVMVoidTypeInContext(c));
		};
		type_array[fif_ptr].llvm_type = LLVMPointerTypeInContext(llvm_context, 0);
		type_array[fif_ptr].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMGetUndef(LLVMPointerTypeInContext(c, 0));
		};
		type_array[fif_opaque_ptr].llvm_type = LLVMPointerTypeInContext(llvm_context, 0);
		type_array[fif_opaque_ptr].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMGetUndef(LLVMPointerTypeInContext(c, 0));
		};
		type_array[fif_struct].llvm_type = LLVMVoidTypeInContext(llvm_context);
		type_array[fif_struct].zero_constant = +[](LLVMContextRef c, int32_t t, environment*) {
			return LLVMGetUndef(LLVMVoidTypeInContext(c));
		};
	}
#endif
	dictionary() {
		types.insert_or_assign(std::string("i32"), fif_i32);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("f32"), fif_f32);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("bool"), fif_bool);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("type"), fif_type);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("i64"), fif_i64);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("f64"), fif_f64);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("u32"), fif_u32);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("u64"), fif_u64);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("i16"), fif_i16);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("u16"), fif_i16);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("i8"), fif_i8);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("u8"), fif_u8);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("nil"), fif_nil);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("ptr"), fif_ptr);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 1, 0, 0, 0 });
		types.insert_or_assign(std::string("opaque_ptr"), fif_nil);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
		types.insert_or_assign(std::string("struct"), fif_struct);
		type_array.push_back(type{ nullptr, nullptr, nullptr, 0, 0, 0, 0 });
	}
};


enum class control_structure : uint8_t {
	none, function, str_if, str_while_loop, str_do_loop, mode_switch, globals
};

class compiler_state;

struct typecheck_3_record {
	int32_t stack_height_added_at = 0;
	int32_t rstack_height_added_at = 0;
	int32_t stack_consumed = 0;
	int32_t rstack_consumed = 0;
};

struct var_data {
	int64_t data; // only in interpreter mode -- first so that we can treat the address as the data addres
	LLVMValueRef alloc; // only in llvm mode
	int32_t type;
};
struct let_data {
	int64_t data; // only in interpreter mode
	LLVMValueRef expression; // only in llvm mode
	int32_t type;
};

class opaque_compiler_data {
protected:
	opaque_compiler_data* parent = nullptr;
public:
	opaque_compiler_data(opaque_compiler_data* parent) : parent(parent) { }

	virtual ~opaque_compiler_data() = default;
	virtual control_structure get_type() {
		return control_structure::none;
	}
	virtual LLVMValueRef llvm_function() {
		return parent ? parent->llvm_function() : nullptr;
	}
	virtual LLVMBasicBlockRef* llvm_block() {
		return parent ? parent->llvm_block() : nullptr;
	}
	virtual int32_t word_id() {
		return parent ? parent->word_id() : -1;
	}
	virtual int32_t instance_id() {
		return parent ? parent->instance_id() : -1;
	}
	virtual std::vector<int32_t>* bytecode_compilation_progress() {
		return parent ? parent->bytecode_compilation_progress() : nullptr;
	}
	virtual ankerl::unordered_dense::map<uint64_t, typecheck_3_record>* typecheck_record() {
		return parent ? parent->typecheck_record() : nullptr;
	}
	virtual state_stack* working_state() {
		return parent ? parent->working_state() : nullptr;
	}
	virtual void set_working_state(std::unique_ptr<state_stack> p) {
		if(parent) 
			parent->set_working_state(std::move(p));
	}
	virtual bool finish(environment& env) {
		return true;
	}
	virtual var_data* get_var(std::string const& name) {
		return parent ? parent->get_var(name) : nullptr;
	}
	virtual let_data* get_let(std::string const& name) {
		return parent ? parent->get_let(name) : nullptr;
	}
	virtual var_data* create_var(std::string const& name, int32_t type) {
		return parent ? parent->create_var(name, type) : nullptr;
	}
	virtual let_data* create_let(std::string const& name, int32_t type, int64_t data, LLVMValueRef expression) {
		return parent ? parent->create_let(name, type, data, expression) : nullptr;
	}
	virtual std::vector<int32_t>* type_substitutions() {
		return parent ? parent->type_substitutions() : nullptr;
	}
};


class compiler_state {
public:
	std::unique_ptr<opaque_compiler_data> data;
};

enum class fif_mode {
	interpreting = 0,
	compiling_bytecode = 1,
	compiling_llvm = 2,
	terminated = 3,
	error = 4,
	typechecking_lvl_1 = 5,
	typechecking_lvl_2 = 6,
	typechecking_lvl_3 = 7,
	typechecking_lvl1_failed = 8,
	typechecking_lvl2_failed = 9,
	typechecking_lvl3_failed = 10
};

inline bool non_immediate_mode(fif_mode m) {
	return m != fif_mode::error && m != fif_mode::terminated && m != fif_mode::interpreting;
}
inline bool typechecking_mode(fif_mode m) {
	return m == fif_mode::typechecking_lvl_1 || m == fif_mode::typechecking_lvl_2 || m == fif_mode::typechecking_lvl_3
		|| m == fif_mode::typechecking_lvl1_failed || m == fif_mode::typechecking_lvl2_failed || m == fif_mode::typechecking_lvl3_failed;
}
inline bool typechecking_failed(fif_mode m) {
	return m == fif_mode::typechecking_lvl1_failed || m == fif_mode::typechecking_lvl2_failed || m == fif_mode::typechecking_lvl3_failed;
}
inline fif_mode reset_typechecking(fif_mode m) {
	if(m == fif_mode::typechecking_lvl1_failed)
		return fif_mode::typechecking_lvl_1;
	return m;
}
inline fif_mode fail_typechecking(fif_mode m) {
	if(m == fif_mode::typechecking_lvl_1)
		return fif_mode::typechecking_lvl1_failed;
	if(m == fif_mode::typechecking_lvl_2)
		return fif_mode::typechecking_lvl2_failed;
	if(m == fif_mode::typechecking_lvl_3)
		return fif_mode::typechecking_lvl3_failed;
	return m;
}
struct import_item {
	std::string name;
	void* ptr = nullptr;
};
class environment {
public:
	ankerl::unordered_dense::set<std::unique_ptr<char[]>, indirect_string_hash, indirect_string_eq> string_constants;
	std::vector<LLVMValueRef> exported_functions;
	std::vector<import_item> imported_functions;
#ifdef USE_LLVM
	LLVMOrcThreadSafeContextRef llvm_ts_context = nullptr;
	LLVMContextRef llvm_context = nullptr;
	LLVMModuleRef llvm_module = nullptr;
	LLVMBuilderRef llvm_builder = nullptr;
	LLVMTargetRef llvm_target = nullptr;
	LLVMTargetMachineRef llvm_target_machine = nullptr;
	LLVMTargetDataRef llvm_target_data = nullptr;
	LLVMOrcLLJITRef llvm_jit = nullptr;
	char* llvm_target_triple = nullptr;
	char* llvm_target_cpu = nullptr;
	char* llvm_target_cpu_features = nullptr;
#endif
	dictionary dict;

	std::vector<std::unique_ptr<opaque_compiler_data>> compiler_stack;
	std::vector<std::string_view> source_stack;

	fif_mode mode = fif_mode::interpreting;
	std::function<void(std::string_view)> report_error = [](std::string_view) { std::abort(); };

	environment();
	~environment() {
#ifdef USE_LLVM
		LLVMDisposeMessage(llvm_target_triple);
		LLVMDisposeMessage(llvm_target_cpu);
		LLVMDisposeMessage(llvm_target_cpu_features);

		if(llvm_jit)
			LLVMOrcDisposeLLJIT(llvm_jit);
		if(llvm_target_data)
			LLVMDisposeTargetData(llvm_target_data);
		if(llvm_target_machine)
			LLVMDisposeTargetMachine(llvm_target_machine);
		if(llvm_builder)
			LLVMDisposeBuilder(llvm_builder);
		if(llvm_module)
			LLVMDisposeModule(llvm_module);
		if(llvm_ts_context)
			LLVMOrcDisposeThreadSafeContext(llvm_ts_context);
#endif
	}

	std::string_view get_string_constant(std::string_view data) {
		if(auto it = string_constants.find(data); it != string_constants.end()) {
			return std::string_view{ it->get() };
		}
		std::unique_ptr<char[]> temp = std::unique_ptr<char[]>(new char[data.length() + 1 ]);
		memcpy(temp.get(), data.data(), data.length());
		temp.get()[data.length()] = 0;
		auto data_ptr = temp.get();
		string_constants.insert(std::move(temp));
		return std::string_view{ data_ptr };
	}
};

class compiler_globals_layer : public opaque_compiler_data {
public:
	ankerl::unordered_dense::map<std::string, std::unique_ptr<var_data>> global_vars;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<let_data>> global_lets;
	environment& env;
	compiler_globals_layer(opaque_compiler_data* p, environment& env) : opaque_compiler_data(p), env(env) {
	}

	virtual control_structure get_type() override {
		return control_structure::globals;
	}

	var_data* get_global_var(std::string const& name) {
		if(auto it = global_vars.find(name); it != global_vars.end()) {
			return it->second.get();
		}
		return nullptr;
	}
	var_data* create_global_var(std::string const& name, int32_t type) {
		if(auto it = global_vars.find(name); it != global_vars.end()) {
			if(it->second->type == type)
				return it->second.get();
			else
				return nullptr;
		}
		auto added = global_vars.insert_or_assign(name, std::make_unique<var_data>());
		added.first->second->type = type;
		added.first->second->data = 0;
#ifdef USE_LLVM
		added.first->second->alloc = LLVMAddGlobal(env.llvm_module, env.dict.type_array[type].llvm_type, name.c_str());
		LLVMSetInitializer(added.first->second->alloc, env.dict.type_array[type].zero_constant(env.llvm_context, type, &env));
#endif
		return added.first->second.get();
	}
	let_data* get_global_let(std::string const& name) {
		if(auto it = global_lets.find(name); it != global_lets.end()) {
			return it->second.get();
		}
		return nullptr;
	}
	let_data* create_global_let(std::string const& name, int32_t type, int64_t data, LLVMValueRef expression) {
		if(auto it = global_lets.find(name); it != global_lets.end()) {
			return nullptr;
		}
		auto added = global_lets.insert_or_assign(name, std::make_unique<let_data>());
		added.first->second->type = type;
		added.first->second->data = data;
		added.first->second->expression = expression;
		if(auto it = global_lets.find(name); it != global_lets.end()) {
			return it->second.get();
		}
		return nullptr;
	}
};

inline environment::environment() {
#ifdef USE_LLVM
	llvm_ts_context = LLVMOrcCreateNewThreadSafeContext();
	llvm_context = LLVMOrcThreadSafeContextGetContext(llvm_ts_context);
	llvm_module = LLVMModuleCreateWithNameInContext("module_main", llvm_context);
	llvm_builder = LLVMCreateBuilderInContext(llvm_context);

	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();

	llvm_target = LLVMGetFirstTarget();
	llvm_target_triple = LLVMGetDefaultTargetTriple();
	llvm_target_cpu = LLVMGetHostCPUName();
	llvm_target_cpu_features = LLVMGetHostCPUFeatures();

	llvm_target_machine = LLVMCreateTargetMachine(llvm_target, llvm_target_triple, llvm_target_cpu, llvm_target_cpu_features, LLVMCodeGenOptLevel::LLVMCodeGenLevelAggressive, LLVMRelocMode::LLVMRelocDefault, LLVMCodeModel::LLVMCodeModelJITDefault);

	LLVMSetTarget(llvm_module, llvm_target_triple);
	llvm_target_data = LLVMCreateTargetDataLayout(llvm_target_machine);
	LLVMSetModuleDataLayout(llvm_module, llvm_target_data);

	dict.ready_llvm_types(llvm_context);
#endif

	compiler_stack.push_back(std::make_unique<compiler_globals_layer>(nullptr, *this));
}

class interpreter_stack : public state_stack {
public:
	std::vector<int64_t> main_datas;
	std::vector<int64_t> return_datas;

	interpreter_stack() { }

	virtual void pop_main() override {
		if(main_datas.empty())
			return;

		main_datas.pop_back();
		main_types.pop_back();

		min_main_depth = std::min(min_main_depth, main_types.size());
	}
	virtual void pop_return() override {
		if(return_datas.empty())
			return;

		return_datas.pop_back();
		return_types.pop_back();

		min_return_depth = std::min(min_return_depth, return_types.size());
	}
	virtual void resize(size_t main_sz, size_t return_sz) override {
		min_return_depth = std::min(min_return_depth, return_sz);
		min_main_depth = std::min(min_main_depth, main_sz);

		return_types.resize(return_sz);
		return_datas.resize(return_sz);
		main_types.resize(main_sz);
		main_datas.resize(main_sz);
	}
	virtual int64_t main_data(size_t index) const override {
		return main_datas[index];
	}
	virtual int64_t return_data(size_t index) const override {
		return return_datas[index];
	}
	virtual LLVMValueRef main_ex(size_t index) const override {
		return nullptr;
	}
	virtual LLVMValueRef return_ex(size_t index) const override {
		return nullptr;
	}
	virtual int64_t main_data_back(size_t index)const override {
		return main_datas[main_datas.size() - (index + 1)];
	}
	virtual int64_t return_data_back(size_t index)const override {
		return return_datas[return_datas.size() - (index + 1)];
	}
	virtual LLVMValueRef main_ex_back(size_t index)const override {
		return nullptr;
	}
	virtual LLVMValueRef return_ex_back(size_t index) const override {
		return nullptr;
	}
	virtual void set_main_data(size_t index, int64_t value) override {
		main_datas[index] = value;
		min_main_depth = std::min(min_main_depth, index);
	}
	virtual void set_return_data(size_t index, int64_t value) override {
		return_datas[index] = value;
		min_return_depth = std::min(min_return_depth, index);
	}
	virtual void set_main_ex(size_t index, LLVMValueRef value)override {
		min_main_depth = std::min(min_main_depth, index);
	}
	virtual void set_return_ex(size_t index, LLVMValueRef value)override {
		min_return_depth = std::min(min_return_depth, index);
	}
	virtual void set_main_data_back(size_t index, int64_t value) override {
		main_datas[main_datas.size() - (index + 1)] = value;
		min_main_depth = std::min(min_main_depth, main_datas.size() - (index + 1));
	}
	virtual void set_return_data_back(size_t index, int64_t value) override {
		return_datas[return_datas.size() - (index + 1)] = value;
		min_return_depth = std::min(min_return_depth, return_datas.size() - (index + 1));
	}
	virtual void set_main_ex_back(size_t index, LLVMValueRef value) override {
		min_main_depth = std::min(min_main_depth, main_datas.size() - (index + 1));
	}
	virtual void set_return_ex_back(size_t index, LLVMValueRef value) override {
		min_return_depth = std::min(min_return_depth, return_datas.size() - (index + 1));
	}
	virtual stack_type get_type() const override {
		return stack_type::interpreter_stack;
	}
	virtual void move_into(state_stack&& other) override {
		if(other.get_type() != stack_type::interpreter_stack)
			std::abort();

		interpreter_stack&& o = static_cast<interpreter_stack&&>(other);
		main_datas = std::move(o.main_datas);
		return_datas = std::move(o.return_datas);
		main_types = std::move(o.main_types);
		return_types = std::move(o.return_types);
		min_main_depth = std::min(min_main_depth, o.min_main_depth);
		min_return_depth = std::min(min_return_depth, o.min_return_depth);
		o.main_datas.clear();
		o.return_datas.clear();
		o.main_types.clear();
		o.return_types.clear();
	}
	virtual void copy_into(state_stack const& other) override {
		if(other.get_type() != stack_type::interpreter_stack)
			std::abort();
		interpreter_stack const& o = static_cast<interpreter_stack const&>(other);
		main_datas = o.main_datas;
		return_datas = o.return_datas;
		main_types = o.main_types;
		return_types = o.return_types;
		min_main_depth = std::min(min_main_depth, o.min_main_depth);
		min_return_depth = std::min(min_return_depth, o.min_return_depth);
	}
	virtual void push_back_main(int32_t type, int64_t data, LLVMValueRef expr) override {
		main_datas.push_back(data);
		main_types.push_back(type);
	}
	virtual void push_back_return(int32_t type, int64_t data, LLVMValueRef expr) override {
		return_datas.push_back(data);
		return_types.push_back(type);
	}
	virtual std::unique_ptr<state_stack> copy() const override {
		auto temp_new = std::make_unique< interpreter_stack>(*this);
		temp_new->min_main_depth = main_types.size();
		temp_new->min_return_depth = return_types.size();
		return temp_new;
	}
};

inline std::string word_name_from_id(int32_t w, environment const& e) {
	for(auto& p : e.dict.words) {
		if(p.second == w)
			return p.first;
	}
	return "@unknown (" + std::to_string(w)  + ")";
}

inline int32_t* illegal_interpretation(state_stack& s, int32_t* p, environment* e) {
	e->report_error("attempted to perform an illegal operation under interpretation");
	e->mode = fif_mode::error;
	return nullptr;
}

struct type_match_result {
	bool matched = false;
	int32_t stack_consumed = 0;
	int32_t ret_stack_consumed = 0;
};
struct type_match {
	int32_t type = 0;
	uint32_t end_match_pos = 0;
};


inline bool is_positive_integer(char const* start, char const* end) {
	if(start == end)
		return false;
	while(start < end) {
		if(!isdigit(*start))
			return false;
		++start;
	}
	return true;
}

inline bool is_integer(char const* start, char const* end) {
	if(start == end)
		return false;
	if(*start == '-')
		return is_positive_integer(start + 1, end);
	else
		return is_positive_integer(start, end);
}

inline bool is_positive_fp(char const* start, char const* end) {
	auto const decimal = std::find(start, end, '.');
	if(decimal == end) {
		return is_positive_integer(start, end);
	} else if(decimal == start) {
		return is_positive_integer(decimal + 1, end);
	} else {
		return is_positive_integer(start, decimal) && (decimal + 1 == end || is_positive_integer(decimal + 1, end));
	}
}

inline bool is_fp(char const* start, char const* end) {
	if(start == end)
		return false;
	if(*start == '-')
		return is_positive_fp(start + 1, end);
	else
		return is_positive_fp(start, end);
}

inline double pow_10(int n) {
	static double const e[] = {// 1e-0...1e308: 309 * 8 bytes = 2472 bytes
		1e+0, 1e+1, 1e+2, 1e+3, 1e+4, 1e+5, 1e+6, 1e+7, 1e+8, 1e+9, 1e+10, 1e+11, 1e+12, 1e+13, 1e+14, 1e+15, 1e+16, 1e+17, 1e+18,
		1e+19, 1e+20, 1e+21, 1e+22, 1e+23, 1e+24, 1e+25, 1e+26, 1e+27, 1e+28, 1e+29, 1e+30, 1e+31, 1e+32, 1e+33, 1e+34, 1e+35,
		1e+36, 1e+37, 1e+38, 1e+39, 1e+40, 1e+41, 1e+42, 1e+43, 1e+44, 1e+45, 1e+46, 1e+47, 1e+48, 1e+49, 1e+50, 1e+51, 1e+52,
		1e+53, 1e+54, 1e+55, 1e+56, 1e+57, 1e+58, 1e+59, 1e+60, 1e+61, 1e+62, 1e+63, 1e+64, 1e+65, 1e+66, 1e+67, 1e+68, 1e+69,
		1e+70, 1e+71, 1e+72, 1e+73, 1e+74, 1e+75, 1e+76, 1e+77, 1e+78, 1e+79, 1e+80, 1e+81, 1e+82, 1e+83, 1e+84, 1e+85, 1e+86,
		1e+87, 1e+88, 1e+89, 1e+90, 1e+91, 1e+92, 1e+93, 1e+94, 1e+95, 1e+96, 1e+97, 1e+98, 1e+99, 1e+100, 1e+101, 1e+102, 1e+103,
		1e+104, 1e+105, 1e+106, 1e+107, 1e+108, 1e+109, 1e+110, 1e+111, 1e+112, 1e+113, 1e+114, 1e+115, 1e+116, 1e+117, 1e+118,
		1e+119, 1e+120, 1e+121, 1e+122, 1e+123, 1e+124, 1e+125, 1e+126, 1e+127, 1e+128, 1e+129, 1e+130, 1e+131, 1e+132, 1e+133,
		1e+134, 1e+135, 1e+136, 1e+137, 1e+138, 1e+139, 1e+140, 1e+141, 1e+142, 1e+143, 1e+144, 1e+145, 1e+146, 1e+147, 1e+148,
		1e+149, 1e+150, 1e+151, 1e+152, 1e+153, 1e+154, 1e+155, 1e+156, 1e+157, 1e+158, 1e+159, 1e+160, 1e+161, 1e+162, 1e+163,
		1e+164, 1e+165, 1e+166, 1e+167, 1e+168, 1e+169, 1e+170, 1e+171, 1e+172, 1e+173, 1e+174, 1e+175, 1e+176, 1e+177, 1e+178,
		1e+179, 1e+180, 1e+181, 1e+182, 1e+183, 1e+184, 1e+185, 1e+186, 1e+187, 1e+188, 1e+189, 1e+190, 1e+191, 1e+192, 1e+193,
		1e+194, 1e+195, 1e+196, 1e+197, 1e+198, 1e+199, 1e+200, 1e+201, 1e+202, 1e+203, 1e+204, 1e+205, 1e+206, 1e+207, 1e+208,
		1e+209, 1e+210, 1e+211, 1e+212, 1e+213, 1e+214, 1e+215, 1e+216, 1e+217, 1e+218, 1e+219, 1e+220, 1e+221, 1e+222, 1e+223,
		1e+224, 1e+225, 1e+226, 1e+227, 1e+228, 1e+229, 1e+230, 1e+231, 1e+232, 1e+233, 1e+234, 1e+235, 1e+236, 1e+237, 1e+238,
		1e+239, 1e+240, 1e+241, 1e+242, 1e+243, 1e+244, 1e+245, 1e+246, 1e+247, 1e+248, 1e+249, 1e+250, 1e+251, 1e+252, 1e+253,
		1e+254, 1e+255, 1e+256, 1e+257, 1e+258, 1e+259, 1e+260, 1e+261, 1e+262, 1e+263, 1e+264, 1e+265, 1e+266, 1e+267, 1e+268,
		1e+269, 1e+270, 1e+271, 1e+272, 1e+273, 1e+274, 1e+275, 1e+276, 1e+277, 1e+278, 1e+279, 1e+280, 1e+281, 1e+282, 1e+283,
		1e+284, 1e+285, 1e+286, 1e+287, 1e+288, 1e+289, 1e+290, 1e+291, 1e+292, 1e+293, 1e+294, 1e+295, 1e+296, 1e+297, 1e+298,
		1e+299, 1e+300, 1e+301, 1e+302, 1e+303, 1e+304, 1e+305, 1e+306, 1e+307, 1e+308 };
	return e[n];
}

inline bool float_from_chars(char const* start, char const* end, float& float_out) { // returns true on success
	// first read the chars into an int, keeping track of the magnitude
	// multiply by a pow of 10

	int32_t magnitude = 0;
	int64_t accumulated = 0;
	bool after_decimal = false;

	if(start == end) {
		float_out = 0.0f;
		return true;
	}

	bool is_negative = false;
	if(*start == '-') {
		is_negative = true;
		++start;
	} else if(*start == '+') {
		++start;
	}

	for(; start < end; ++start) {
		if(*start >= '0' && *start <= '9') {
			accumulated = accumulated * 10 + (*start - '0');
			magnitude += int32_t(after_decimal);
		} else if(*start == '.') {
			after_decimal = true;
		} else {
			// maybe check for non space and throw an error?
		}
	}
	if(!is_negative) {
		if(magnitude > 0)
			float_out = float(double(accumulated) / pow_10(magnitude));
		else
			float_out = float(accumulated);
	} else {
		if(magnitude > 0)
			float_out = -float(double(accumulated) / pow_10(magnitude));
		else
			float_out = -float(accumulated);
	}
	return true;
}

inline int32_t parse_int(std::string_view content) {
	int32_t rvalue = 0;
	auto result = std::from_chars(content.data(), content.data() + content.length(), rvalue);
	if(result.ec == std::errc::invalid_argument) {
		return 0;
	}
	return rvalue;
}
inline float parse_float(std::string_view content) {
	float rvalue = 0.0f;
	if(!float_from_chars(content.data(), content.data() + content.length(), rvalue)) {
		return 0.0f;
	}
	return rvalue;
}
struct variable_match_result {
	bool match_result = false;
	uint32_t match_end = 0;
};

inline type_match resolve_span_type(std::span<int32_t const> tlist, std::vector<int32_t> const& type_subs, environment& env);
inline variable_match_result fill_in_variable_types(int32_t source_type, std::span<int32_t const> match_span, std::vector<int32_t>& type_subs, environment& env);

#ifdef USE_LLVM
inline LLVMValueRef struct_zero_constant(LLVMContextRef c, int32_t t, environment* e) {
	std::vector<LLVMValueRef> zvals;
	for(int32_t j = 1; j < e->dict.type_array[t].decomposed_types_count; ++j) {
		auto st = e->dict.all_stack_types[e->dict.type_array[t].decomposed_types_start + j];
		zvals.push_back(e->dict.type_array[st].zero_constant(c, st, e));
	}
	return LLVMConstNamedStruct(e->dict.type_array[t].llvm_type, zvals.data(), uint32_t(zvals.size()));
}
#endif

inline int32_t interepreter_size(int32_t type, environment& env) {
	if(env.dict.type_array[type].decomposed_types_count == 0)
		return 8;
	auto main_type = env.dict.all_stack_types[env.dict.type_array[type].decomposed_types_start];
	return 8 * (env.dict.type_array[type].decomposed_types_count - 1 - env.dict.type_array[main_type].non_member_types);
}

inline int32_t struct_child_count(int32_t type, environment& env) {
	if(env.dict.type_array[type].decomposed_types_count == 0)
		return 8;
	auto main_type = env.dict.all_stack_types[env.dict.type_array[type].decomposed_types_start];
	return (env.dict.type_array[type].decomposed_types_count - 1 - env.dict.type_array[main_type].non_member_types);
}

inline int64_t interpreter_struct_zero_constant(int32_t t, environment* e) {
	if(e->dict.type_array[t].single_member_struct()) {
		auto child_index = e->dict.type_array[t].decomposed_types_start + 1 + 0;
		auto child_type = e->dict.all_stack_types[child_index];
		if(e->dict.type_array[child_type].interpreter_zero)
			return e->dict.type_array[child_type].interpreter_zero(child_type, e);
		else
			return 0;
	} else {
		auto bytes = interepreter_size(t, *e);
		auto mem = (char*)malloc(bytes);
		memset(mem, 0, bytes);
		return (int64_t)mem;
	}
}

inline uint32_t next_encoded_stack_type(std::span<int32_t const> desc) {
	if(desc.size() == 0)
		return 0;
	if(desc.size() == 1)
		return 1;
	if(desc[1] != std::numeric_limits<int32_t>::max())
		return 1;
	uint32_t i = 2;
	while(i < desc.size() && desc[i] != -1) {
		i += next_encoded_stack_type(desc.subspan(i));
	}
	if(i < desc.size() && desc[i] == -1)
		++i;
	return i;
}

inline int32_t instantiate_templated_struct_full(int32_t template_base, std::vector<int32_t> const& final_subtype_list, environment& env) {

	auto desc = std::span<int32_t const>(env.dict.all_stack_types.data() + env.dict.type_array[template_base].decomposed_types_start, size_t(env.dict.type_array[template_base].decomposed_types_count));

	uint32_t match_pos = 0;
	uint32_t mem_count = 0;
	std::vector<int32_t> type_subs_out;
	while(match_pos < desc.size()) {
		if(mem_count > final_subtype_list.size()) {
			env.report_error("attempted to instantiate a struct template with the wrong number of members");
			env.mode = fif_mode::error;
			return -1;
		}
		
		auto mr = fill_in_variable_types(final_subtype_list[mem_count], desc.subspan(match_pos), type_subs_out, env);
		if(!mr.match_result) {
			env.report_error("attempted to instantiate a struct template with types that do not match its definition");
			env.mode = fif_mode::error;
			return -1;
		}

		match_pos += next_encoded_stack_type(desc.subspan(match_pos));
		++mem_count;
	}
	if(mem_count < final_subtype_list.size()) {
		env.report_error("attempted to instantiate a struct template with the wrong number of members");
		env.mode = fif_mode::error;
		return -1;
	}

	for(uint32_t i = 0; i < env.dict.type_array.size(); ++i) {
		if(env.dict.type_array[i].decomposed_types_count == int32_t(final_subtype_list.size() + 1)) {
			auto ta_start = env.dict.type_array[i].decomposed_types_start;
			bool match = env.dict.all_stack_types[ta_start] == template_base;

			for(uint32_t j = 0; match && j < final_subtype_list.size(); ++j) {
				if(env.dict.all_stack_types[ta_start + j + 1] != final_subtype_list[j]) {
					match = false;
				}
			}

			if(match) 
				return  int32_t(i);
		}
	}
	
	int32_t new_type = int32_t(env.dict.type_array.size());
	env.dict.type_array.emplace_back();

	std::vector<LLVMTypeRef> ctypes;
	for(uint32_t j = 0; j < final_subtype_list.size(); ++j) {
		ctypes.push_back(env.dict.type_array[final_subtype_list[j]].llvm_type);
	}
	for(int32_t j = 0; j < env.dict.type_array[template_base].non_member_types; ++j)
		ctypes.pop_back();

	if(ctypes.size() != 1) {
		std::string autoname = "struct#" + std::to_string(env.dict.type_array.size());
#ifdef USE_LLVM
		auto ty = LLVMStructCreateNamed(env.llvm_context, autoname.c_str());
		LLVMStructSetBody(ty, ctypes.data(), uint32_t(ctypes.size()), false);
		env.dict.type_array.back().llvm_type = ty;
		env.dict.type_array.back().zero_constant = struct_zero_constant;
#endif
		env.dict.type_array.back().interpreter_zero = interpreter_struct_zero_constant;
	} else {
#ifdef USE_LLVM
		env.dict.type_array.back().llvm_type = ctypes[0];
		env.dict.type_array.back().zero_constant = env.dict.type_array[final_subtype_list[0]].zero_constant;
#endif
		env.dict.type_array.back().interpreter_zero = env.dict.type_array[final_subtype_list[0]].interpreter_zero;
	}

	env.dict.type_array.back().decomposed_types_count = uint32_t(final_subtype_list.size() + 1);
	env.dict.type_array.back().decomposed_types_start = uint32_t(env.dict.all_stack_types.size());
	
	for(uint32_t j = 0; j < final_subtype_list.size(); ++j) {
		env.dict.type_array.back().flags |= env.dict.type_array[final_subtype_list[j]].flags;
	}
	if(ctypes.size() == 1)
		env.dict.type_array.back().flags |= type::FLAG_SINGLE_MEMBER;

	env.dict.type_array.back().flags &= ~(type::FLAG_TEMPLATE);
	env.dict.type_array.back().flags |= type::FLAG_REFCOUNTED;
	env.dict.type_array.back().type_slots = 0;
	env.dict.all_stack_types.push_back(template_base);
	env.dict.all_stack_types.insert(env.dict.all_stack_types.end(), final_subtype_list.begin(), final_subtype_list.end());

	return new_type;
}

inline int32_t instantiate_templated_struct(int32_t template_base, std::vector<int32_t> const& subtypes, environment& env) {
	std::vector<int32_t> final_subtype_list;
	final_subtype_list.push_back(template_base);

	auto desc = std::span<int32_t const>(env.dict.all_stack_types.data() + env.dict.type_array[template_base].decomposed_types_start, size_t(env.dict.type_array[template_base].decomposed_types_count));

	uint32_t match_pos = 0;
	while(match_pos < desc.size()) {
		auto mresult = resolve_span_type(desc.subspan(match_pos), subtypes, env);
		match_pos += mresult.end_match_pos;
		final_subtype_list.push_back(mresult.type);
	}

	for(uint32_t i = 0; i < env.dict.type_array.size(); ++i) {
		if(env.dict.type_array[i].decomposed_types_count == int32_t(final_subtype_list.size())) {
			auto ta_start = env.dict.type_array[i].decomposed_types_start;
			bool match = true;

			for(uint32_t j = 0; j < final_subtype_list.size(); ++j) {
				if(env.dict.all_stack_types[ta_start + j] != final_subtype_list[j]) {
					match = false;
					break;
				}
			}

			if(match)
				return  int32_t(i);
		}
	}

	int32_t new_type = int32_t(env.dict.type_array.size());
	env.dict.type_array.emplace_back();

	std::vector<LLVMTypeRef> ctypes;
	for(uint32_t j = 1; j < final_subtype_list.size(); ++j) {
		ctypes.push_back(env.dict.type_array[final_subtype_list[j]].llvm_type);
	}
	for(int32_t j = 0; j < env.dict.type_array[template_base].non_member_types; ++j)
		ctypes.pop_back();

	if(ctypes.size() != 1) {
		std::string autoname = "struct#" + std::to_string(env.dict.type_array.size());
#ifdef USE_LLVM
		auto ty = LLVMStructCreateNamed(env.llvm_context, autoname.c_str());
		LLVMStructSetBody(ty, ctypes.data(), uint32_t(ctypes.size()), false);
		env.dict.type_array.back().llvm_type = ty;
		env.dict.type_array.back().zero_constant = struct_zero_constant;
#endif
		env.dict.type_array.back().interpreter_zero = interpreter_struct_zero_constant;
	} else {
#ifdef USE_LLVM
		env.dict.type_array.back().llvm_type = ctypes[0];
		env.dict.type_array.back().zero_constant = env.dict.type_array[final_subtype_list[0]].zero_constant;
#endif
		env.dict.type_array.back().interpreter_zero = env.dict.type_array[final_subtype_list[0]].interpreter_zero;
	}
	env.dict.type_array.back().decomposed_types_count = uint32_t(final_subtype_list.size());
	env.dict.type_array.back().decomposed_types_start = uint32_t(env.dict.all_stack_types.size());

	for(uint32_t j = 1; j < final_subtype_list.size(); ++j) {
		env.dict.type_array.back().flags |= env.dict.type_array[final_subtype_list[j]].flags;
	}
	env.dict.type_array.back().flags &= ~(type::FLAG_TEMPLATE);
	env.dict.type_array.back().flags |= type::FLAG_REFCOUNTED;
	if(ctypes.size() == 1)
		env.dict.type_array.back().flags |= type::FLAG_SINGLE_MEMBER;

	env.dict.type_array.back().type_slots = 0;
	env.dict.all_stack_types.insert(env.dict.all_stack_types.end(), final_subtype_list.begin(), final_subtype_list.end());

	return new_type;
}

inline int32_t make_struct_type(std::string_view name, std::span<int32_t const> subtypes, std::vector<std::string_view> const& member_names, environment& env, int32_t template_types, int32_t extra_count) {

	int32_t new_type = int32_t(env.dict.type_array.size());
	env.dict.type_array.emplace_back();
	env.dict.types.insert_or_assign(std::string(name), new_type);

	if(template_types == 0 && extra_count == 0) {
		std::vector<LLVMTypeRef> ctypes;
		for(uint32_t j = 0; j < subtypes.size(); ++j) {
			ctypes.push_back(env.dict.type_array[subtypes[j]].llvm_type);
		}
		if(ctypes.size() != 1) {
			std::string autoname = "struct#" + std::to_string(env.dict.type_array.size());
#ifdef USE_LLVM
			auto ty = LLVMStructCreateNamed(env.llvm_context, autoname.c_str());
			LLVMStructSetBody(ty, ctypes.data(), uint32_t(ctypes.size()), false);
			env.dict.type_array.back().llvm_type = ty;
			env.dict.type_array.back().zero_constant = struct_zero_constant;
#endif
			env.dict.type_array.back().interpreter_zero = interpreter_struct_zero_constant;
		} else {
#ifdef USE_LLVM
			env.dict.type_array.back().llvm_type = ctypes[0];
			env.dict.type_array.back().zero_constant = env.dict.type_array[subtypes[0]].zero_constant;
#endif
			env.dict.type_array.back().interpreter_zero = env.dict.type_array[subtypes[0]].interpreter_zero;
		}
	} else {
#ifdef USE_LLVM
		env.dict.type_array.back().llvm_type = LLVMVoidTypeInContext(env.llvm_context);
		env.dict.type_array.back().zero_constant = struct_zero_constant;
#endif
		env.dict.type_array.back().interpreter_zero = interpreter_struct_zero_constant;
		env.dict.type_array.back().non_member_types = extra_count;
	}

	env.dict.type_array.back().decomposed_types_count = uint32_t(subtypes.size() + (template_types + extra_count == 0 ? 1 : extra_count));
	env.dict.type_array.back().decomposed_types_start = uint32_t(env.dict.all_stack_types.size());

	auto add_child_types = [&]() {
		if(template_types + extra_count > 0) {
			env.dict.all_stack_types.push_back(std::numeric_limits<int32_t>::max());
			env.dict.all_stack_types.insert(env.dict.all_stack_types.end(), subtypes.begin(), subtypes.end());
			for(int32_t e = 0; e < extra_count; ++e) {
				env.dict.all_stack_types.push_back(-(template_types + e + 2));
			}
			env.dict.all_stack_types.push_back(-1);
		}
	};


	if(template_types + extra_count == 0) {
		for(uint32_t j = 1; j < subtypes.size(); ++j) {
			env.dict.type_array.back().flags |= env.dict.type_array[subtypes[j]].flags;
		}
	}
	if(template_types + extra_count > 0)
		env.dict.type_array.back().flags |= type::FLAG_TEMPLATE;
	else
		env.dict.type_array.back().flags &= ~(type::FLAG_TEMPLATE);

	if(subtypes.size() == 1)
		env.dict.type_array.back().flags |= type::FLAG_SINGLE_MEMBER;

	env.dict.type_array.back().flags |= type::FLAG_REFCOUNTED;
	env.dict.type_array.back().type_slots = template_types + extra_count;
	if(template_types + extra_count == 0)
		env.dict.all_stack_types.push_back(fif_struct);
	env.dict.all_stack_types.insert(env.dict.all_stack_types.end(), subtypes.begin(), subtypes.end());
	for(int32_t e = 0; e < extra_count; ++e) {
		env.dict.all_stack_types.push_back(-(template_types + e + 2));
	}

	int32_t start_types = int32_t(env.dict.all_stack_types.size());
	env.dict.all_stack_types.push_back(new_type);
	add_child_types();
	int32_t end_zero = int32_t(env.dict.all_stack_types.size());
	env.dict.all_stack_types.push_back(-1);
	env.dict.all_stack_types.push_back(new_type);
	add_child_types();
	int32_t end_one = int32_t(env.dict.all_stack_types.size());
	env.dict.all_stack_types.push_back(new_type);
	add_child_types();

	int32_t end_two = int32_t(env.dict.all_stack_types.size());

	auto bury_word = [&](std::string const& name, int32_t id) { 
		int32_t start_word = 0;
		int32_t preceding_word = -1;
		if(auto it = env.dict.words.find(name); it != env.dict.words.end()) {
			start_word = it->second;
		} else {
			env.dict.words.insert_or_assign(name, id);
			return;
		}
		while(env.dict.word_array[start_word].specialization_of != -1) {
			preceding_word = start_word;
			start_word = env.dict.word_array[start_word].specialization_of;
		}
		if(preceding_word == -1) {
			env.dict.words.insert_or_assign(name, id);
			env.dict.word_array[id].specialization_of = start_word;
			assert(id != start_word);
		} else {
			env.dict.word_array[preceding_word].specialization_of = id;
			env.dict.word_array[id].specialization_of = start_word;
			assert(id != start_word);
		}
	};

	{
		env.dict.word_array.emplace_back();
		env.dict.word_array.back().source = std::string("struct-map2 copy");
		env.dict.word_array.back().stack_types_start = start_types;
		env.dict.word_array.back().stack_types_count = end_two - start_types;
		env.dict.word_array.back().treat_as_base = true;
		bury_word("copy", int32_t(env.dict.word_array.size() - 1));
	}
	{
		env.dict.word_array.emplace_back();
		env.dict.word_array.back().source = std::string("struct-map2 dup");
		env.dict.word_array.back().stack_types_start = start_types;
		env.dict.word_array.back().stack_types_count = end_two - start_types;
		env.dict.word_array.back().treat_as_base = true;
		bury_word("dup", int32_t(env.dict.word_array.size() - 1));
	}
	{
		env.dict.word_array.emplace_back();
		env.dict.word_array.back().source = std::string("struct-map1 init");
		env.dict.word_array.back().stack_types_start = start_types;
		env.dict.word_array.back().stack_types_count = end_one - start_types;
		env.dict.word_array.back().treat_as_base = true;
		bury_word("init", int32_t(env.dict.word_array.size() - 1));
	}
	{
		env.dict.word_array.emplace_back();
		env.dict.word_array.back().source = std::string("struct-map0 drop");
		env.dict.word_array.back().stack_types_start = start_types;
		env.dict.word_array.back().stack_types_count = end_zero - start_types;
		env.dict.word_array.back().treat_as_base = true;
		bury_word("drop", int32_t(env.dict.word_array.size() - 1));
	}

	uint32_t index = 0;
	auto desc = subtypes;
	while(desc.size() > 0) {
		auto next = next_encoded_stack_type(desc);

		{
			int32_t start_types_i = int32_t(env.dict.all_stack_types.size());
			env.dict.all_stack_types.push_back(new_type);
			add_child_types();
			env.dict.all_stack_types.push_back(-1);
			for(uint32_t j = 0; j < next; ++j)
				env.dict.all_stack_types.push_back(desc[j]);
			int32_t end_types_i = int32_t(env.dict.all_stack_types.size());

			env.dict.word_array.emplace_back();
			env.dict.word_array.back().source = std::string("forth.extract ") + std::to_string(index);
			env.dict.word_array.back().stack_types_start = start_types_i;
			env.dict.word_array.back().stack_types_count = end_types_i - start_types_i;
			env.dict.word_array.back().treat_as_base = true;

			bury_word(std::string(".") + std::string{ member_names[index] }, int32_t(env.dict.word_array.size() - 1));
		}
		{
			int32_t start_types_i = int32_t(env.dict.all_stack_types.size());
			env.dict.all_stack_types.push_back(new_type);
			add_child_types();
			env.dict.all_stack_types.push_back(-1);
			env.dict.all_stack_types.push_back(new_type);
			add_child_types();
			for(uint32_t j = 0; j < next; ++j)
				env.dict.all_stack_types.push_back(desc[j]);
			int32_t end_types_i = int32_t(env.dict.all_stack_types.size());

			env.dict.word_array.emplace_back();
			env.dict.word_array.back().source = std::string("forth.extract-copy ") + std::to_string(index);
			env.dict.word_array.back().stack_types_start = start_types_i;
			env.dict.word_array.back().stack_types_count = end_types_i - start_types_i;
			env.dict.word_array.back().treat_as_base = true;

			bury_word(std::string(".") + std::string{ member_names[index] } + "@", int32_t(env.dict.word_array.size() - 1));
		}
		{
			int32_t start_types_i = int32_t(env.dict.all_stack_types.size());
			env.dict.all_stack_types.push_back(new_type);
			add_child_types();
			for(uint32_t j = 0; j < next; ++j)
				env.dict.all_stack_types.push_back(desc[j]);
			env.dict.all_stack_types.push_back(-1);
			env.dict.all_stack_types.push_back(new_type);
			add_child_types();
			int32_t end_types_i = int32_t(env.dict.all_stack_types.size());

			env.dict.word_array.emplace_back();
			env.dict.word_array.back().source = std::string("forth.insert ") + std::to_string(index);
			env.dict.word_array.back().stack_types_start = start_types_i;
			env.dict.word_array.back().stack_types_count = end_types_i - start_types_i;
			env.dict.word_array.back().treat_as_base = true;

			bury_word(std::string(".") + std::string{ member_names[index] } + "!", int32_t(env.dict.word_array.size() - 1));
		}
		{
			int32_t start_types_i = int32_t(env.dict.all_stack_types.size());
			env.dict.all_stack_types.push_back(fif_ptr);
			env.dict.all_stack_types.push_back(std::numeric_limits<int32_t>::max());
			env.dict.all_stack_types.push_back(new_type);
			add_child_types();
			env.dict.all_stack_types.push_back(-1);
			env.dict.all_stack_types.push_back(-1);
			env.dict.all_stack_types.push_back(fif_ptr);
			env.dict.all_stack_types.push_back(std::numeric_limits<int32_t>::max());
			for(uint32_t j = 0; j < next; ++j)
				env.dict.all_stack_types.push_back(desc[j]);
			env.dict.all_stack_types.push_back(-1);
			int32_t end_types_i = int32_t(env.dict.all_stack_types.size());

			env.dict.word_array.emplace_back();
			env.dict.word_array.back().source = std::string("forth.gep ") + std::to_string(index);
			env.dict.word_array.back().stack_types_start = start_types_i;
			env.dict.word_array.back().stack_types_count = end_types_i - start_types_i;
			env.dict.word_array.back().treat_as_base = true;

			bury_word(std::string(".") + std::string{ member_names[index] }, int32_t(env.dict.word_array.size() - 1));
		}

		++index;
		desc = desc.subspan(next);
	}

	return new_type;
}

inline type_match internal_resolve_type(std::string_view text, environment& env, std::vector<int32_t>* type_subs) {
	uint32_t mt_end = 0;
	if(text.starts_with("ptr(nil)")) {
		return type_match{ fif_opaque_ptr, 8 };
	}
	for(; mt_end < text.size(); ++mt_end) {
		if(text[mt_end] == '(' || text[mt_end] == ')' || text[mt_end] == ',')
			break;
	}

	if(text.size() > 0 && text[0] == '$') {
		if(is_integer(text.data() + 1, text.data() + mt_end)) {
			auto v = parse_int(text.substr(1));
			if(type_subs && v >= 0 && size_t(v) < type_subs->size()) {
				return type_match{ (*type_subs)[v], mt_end };
			}
			return type_match{ fif_nil, mt_end };
		}
	}
	if(text.substr(0, mt_end) == "struct") { // prevent "generic" struct matching
		return type_match{ -1, uint32_t(text.length()) };
	}
	if(auto it = env.dict.types.find(std::string(text.substr(0, mt_end))); it != env.dict.types.end()) {
		if(mt_end >= text.size() || text[mt_end] == ',' ||  text[mt_end] == ')') {	// case: plain type
			return type_match{ it->second, mt_end };
		}
		//followed by type list
		++mt_end;
		std::vector<int32_t> subtypes;
		while(mt_end < text.size() && text[mt_end] != ')') {
			auto sub_match = internal_resolve_type(text.substr(mt_end), env, type_subs);
			subtypes.push_back(sub_match.type);
			mt_end += sub_match.end_match_pos;
			if(mt_end < text.size() && text[mt_end] == ',')
				++mt_end;
		}
		if(mt_end < text.size() && text[mt_end] == ')')
			++mt_end;

		if(env.dict.type_array[it->second].type_slots != int32_t(subtypes.size())) {
			env.report_error("attempted to instantiate a type with the wrong number of parameters");
			env.mode = fif_mode::error;
			return type_match{ -1, mt_end };
		} else if(env.dict.type_array[it->second].is_struct_template()) {
			return type_match{ instantiate_templated_struct(it->second, subtypes, env), mt_end };
		} else {
			for(uint32_t i = 0; i < env.dict.type_array.size(); ++i) {
				if(env.dict.type_array[i].decomposed_types_count == int32_t(1 + subtypes.size())) {
					auto ta_start = env.dict.type_array[i].decomposed_types_start;
					if(env.dict.all_stack_types[ta_start] == it->second) {
						bool match = true;
						for(uint32_t j = 0; j < subtypes.size(); ++j) {
							if(env.dict.all_stack_types[ta_start + 1 + j] != subtypes[j]) {
								match = false;
								break;
							}
						}

						if(match) {
							return type_match{ int32_t(i), mt_end };
						}
					}
				}
			}

			int32_t new_type = int32_t(env.dict.type_array.size());
			env.dict.type_array.emplace_back();
			env.dict.type_array.back().llvm_type = env.dict.type_array[it->second].llvm_type;
			env.dict.type_array.back().decomposed_types_count = uint32_t(1 + subtypes.size());
			env.dict.type_array.back().decomposed_types_start = uint32_t(env.dict.all_stack_types.size());
			env.dict.type_array.back().flags = env.dict.type_array[it->second].flags;
			env.dict.type_array.back().zero_constant = env.dict.type_array[it->second].zero_constant;
			env.dict.type_array.back().type_slots = 0;

			env.dict.all_stack_types.push_back(it->second);
			for(auto t : subtypes) {
				env.dict.all_stack_types.push_back(t);
			}

			return type_match{ new_type, mt_end };
		}
	}

	return type_match{ -1, mt_end };
}

inline int32_t resolve_type(std::string_view text, environment& env, std::vector<int32_t>* type_subs) {
	return internal_resolve_type(text, env, type_subs).type;
}

struct type_span_gen_result {
	std::vector<int32_t> type_array;
	uint32_t end_match_pos = 0;
	int32_t max_variable = -1;
};

inline type_span_gen_result internal_generate_type(std::string_view text, environment& env) {
	uint32_t mt_end = 0;
	type_span_gen_result r;

	if(text.starts_with("ptr(nil)")) {
		r.type_array.push_back(fif_opaque_ptr);
		r.end_match_pos = 8;
		return r;
	}
	for(; mt_end < text.size(); ++mt_end) {
		if(text[mt_end] == '(' || text[mt_end] == ')' || text[mt_end] == ',')
			break;
	}

	if(text.size() > 0 && text[0] == '$') {
		if(is_integer(text.data() + 1, text.data() + mt_end)) {
			auto v = parse_int(text.substr(1));
			r.max_variable = std::max(r.max_variable, v);
			r.type_array.push_back( -(v + 2) );
			r.end_match_pos = mt_end;
			return r;
		}
	}
	if(auto it = env.dict.types.find(std::string(text.substr(0, mt_end))); it != env.dict.types.end()) {
		r.type_array.push_back(it->second);
		if(mt_end >= text.size() || text[mt_end] == ',' || text[mt_end] == ')') {	// case: plain type
			r.end_match_pos = mt_end;
			return r;
		}
		//followed by type list
		++mt_end;
		r.type_array.push_back(std::numeric_limits<int32_t>::max());
		if((env.dict.type_array[it->second].flags & type::FLAG_TEMPLATE) != 0) {
			std::vector<type_span_gen_result> sub_matches;
			while(mt_end < text.size() && text[mt_end] != ')') {
				auto sub_match = internal_generate_type(text.substr(mt_end), env);
				r.max_variable = std::max(r.max_variable, sub_match.max_variable);
				sub_matches.push_back(std::move(sub_match));
				mt_end += sub_match.end_match_pos;
				if(mt_end < text.size() && text[mt_end] == ',')
					++mt_end;
			}
			auto desc = std::span<int32_t const>(env.dict.all_stack_types.data() + env.dict.type_array[it->second].decomposed_types_start, size_t(env.dict.type_array[it->second].decomposed_types_count));
			for(auto v : desc) {
				if(v < -1) {
					auto index = -(v + 2);
					if(size_t(index) < sub_matches.size()) {
						r.type_array.insert(r.type_array.end(), sub_matches[index].type_array.begin(), sub_matches[index].type_array.end());
					} else {
						r.type_array.push_back(fif_nil);
					}
				} else {
					r.type_array.push_back(v);
				}
			}

		} else {
			while(mt_end < text.size() && text[mt_end] != ')') {
				auto sub_match = internal_generate_type(text.substr(mt_end), env);
				r.type_array.insert(r.type_array.end(), sub_match.type_array.begin(), sub_match.type_array.end());
				r.max_variable = std::max(r.max_variable, sub_match.max_variable);
				mt_end += sub_match.end_match_pos;
				if(mt_end < text.size() && text[mt_end] == ',')
					++mt_end;
			}
		}
		r.type_array.push_back(-1);
		if(mt_end < text.size() && text[mt_end] == ')')
			++mt_end;
	}
	r.end_match_pos = mt_end;
	if(r.max_variable == -1) {
		std::vector<int32_t> type_subs;
		auto resolved_type = resolve_span_type(std::span<int32_t const>(r.type_array.begin(), r.type_array.end()), type_subs, env);
		r.type_array.clear();
		r.type_array.push_back(resolved_type.type);
	}
	return r;
}

inline type_match resolve_span_type(std::span<int32_t const> tlist, std::vector<int32_t> const& type_subs, environment& env) {

	auto make_sub = [&](int32_t type_in) {
		if(type_in < -1) {
			auto slot = -(type_in + 2);
			if(slot < int32_t(type_subs.size()))
				return type_subs[slot];
			return -1;
		}
		return type_in;
	};

	if(tlist.size() == 0) {
		return type_match{ -1, 0 };
	}
	if(tlist.size() == 1) {
		return type_match{ make_sub(tlist[0]), 1 };
	}
	if(tlist[1] != std::numeric_limits<int32_t>::max()) {
		return type_match{ make_sub(tlist[0]), 1 };
	}
	int32_t base_type = tlist[0];
	assert(base_type >= 0);
	uint32_t mt_end = 2;

	//followed by type list
	std::vector<int32_t> subtypes;
	while(mt_end < tlist.size() && tlist[mt_end] != -1) {
		auto sub_match = resolve_span_type(tlist.subspan(mt_end), type_subs, env);
		subtypes.push_back(sub_match.type);
		mt_end += sub_match.end_match_pos;
	}
	if(mt_end < tlist.size() && tlist[mt_end] == -1)
		++mt_end;

	
	 if(base_type == fif_ptr && !subtypes.empty() && subtypes[0] == fif_nil) {
		return type_match{ fif_opaque_ptr, mt_end };
	} else if(env.dict.type_array[base_type].is_struct_template()) {
		return type_match{ instantiate_templated_struct_full(base_type, subtypes, env), mt_end };
	} else {
		for(uint32_t i = 0; i < env.dict.type_array.size(); ++i) {
			if(env.dict.type_array[i].decomposed_types_count == int32_t(1 + subtypes.size())) {
				auto ta_start = env.dict.type_array[i].decomposed_types_start;
				if(env.dict.all_stack_types[ta_start] == base_type) {
					bool match = true;
					for(uint32_t j = 0; j < subtypes.size(); ++j) {
						if(env.dict.all_stack_types[ta_start + 1 + j] != subtypes[j]) {
							match = false;
							break;
						}
					}

					if(match) {
						return type_match{ int32_t(i), mt_end };
					}
				}
			}
		}

		int32_t new_type = int32_t(env.dict.type_array.size());
		env.dict.type_array.emplace_back();

		env.dict.type_array.back().llvm_type = env.dict.type_array[base_type].llvm_type;
		env.dict.type_array.back().decomposed_types_count = uint32_t(1 + subtypes.size());
		env.dict.type_array.back().decomposed_types_start = uint32_t(env.dict.all_stack_types.size());
		env.dict.type_array.back().zero_constant = env.dict.type_array[base_type].zero_constant;
		env.dict.type_array.back().flags = env.dict.type_array[base_type].flags;
		env.dict.type_array.back().type_slots = 0;

		env.dict.all_stack_types.push_back(base_type);
		for(auto t : subtypes) {
			env.dict.all_stack_types.push_back(t);
		}

		return type_match{ new_type, mt_end };
	}
}

inline variable_match_result fill_in_variable_types(int32_t source_type, std::span<int32_t const> match_span, std::vector<int32_t>& type_subs, environment& env) { 
	if(match_span.size() == 0)
		return variable_match_result{ true, 0 };

	auto match_to_slot = [&](int32_t matching_type, int32_t matching_against) { 
		if(matching_type < -1) {
			auto slot = -(matching_type + 2);
			if(slot >= int32_t(type_subs.size())) {
				type_subs.resize(size_t(slot + 1), -2);
				type_subs[slot] = matching_against;
				return true;
			} else if(type_subs[slot] == -2 || type_subs[slot] == matching_against) {
				type_subs[slot] = matching_against;
				return true;
			} else {
				return false;
			}
		} else {
			return matching_type == matching_against;
		}
	};

	if(match_span.size() == 1) {
		return variable_match_result{ match_to_slot(match_span[0], source_type), 1 };
	}

	if(match_span[1] != std::numeric_limits<int32_t>::max()) {
		return variable_match_result{ match_to_slot(match_span[0], source_type), 1 };
	}

	uint32_t sub_offset = 2;
	
	std::span<const int32_t> destructured_source{ };
	if(source_type >= 0) {
		auto const& t = env.dict.type_array[source_type];
		destructured_source = std::span<const int32_t>(env.dict.all_stack_types.data() + t.decomposed_types_start, size_t(t.decomposed_types_count));
	}

	bool result = destructured_source.size() > 0 ? match_to_slot(match_span[0], destructured_source[0]) : false;
	uint32_t dest_offset = 1;
	while(sub_offset < match_span.size() && match_span[sub_offset] != -1) {
		auto dmatch = dest_offset < destructured_source.size() ? destructured_source[dest_offset] : -1;
		auto sub_result = fill_in_variable_types(dmatch, match_span.subspan(sub_offset), type_subs, env);
		sub_offset += sub_result.match_end;
		result &= sub_result.match_result;
		++dest_offset;
	}
	if(dest_offset < destructured_source.size())
		result = false;
	if(sub_offset < match_span.size() && match_span[sub_offset] == -1)
		++sub_offset;

	return variable_match_result{ result, sub_offset };
}

inline type_match_result match_stack_description(std::span<int32_t const> desc, state_stack& ts, environment& env, std::vector<int32_t>& type_subs) { // ret: true if function matched
	int32_t match_position = 0;
	// stack matching

	auto const ssize = int32_t(ts.main_size());
	int32_t consumed_stack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		if(consumed_stack_cells >= ssize)
			return type_match_result{ false, 0, 0 };
		auto match_result = fill_in_variable_types(ts.main_type_back(size_t(consumed_stack_cells)), desc.subspan(match_position), type_subs, env);
		if(!match_result.match_result)
			return type_match_result{ false, 0, 0 };
		match_position += match_result.match_end;
		++consumed_stack_cells;
	}

	++match_position; // skip -1

	// skip over output stack
	int32_t first_output_stack = match_position;
	//int32_t output_stack_types = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		match_position += next_encoded_stack_type(desc.subspan(match_position));
		//++output_stack_types;
	}
	//int32_t last_output_stack = match_position;
	++match_position; // skip -1

	// return stack matching
	auto const rsize = int32_t(ts.return_size());
	int32_t consumed_rstack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		if(consumed_rstack_cells >= rsize)
			return type_match_result{ false, 0, 0 }; 
		auto match_result = fill_in_variable_types(ts.return_type_back(size_t(consumed_rstack_cells)), desc.subspan(match_position), type_subs, env);
		if(!match_result.match_result)
			return type_match_result{ false, 0, 0 };
		match_position += match_result.match_end;
		++consumed_rstack_cells;
	}

	++match_position; // skip -1

	// output ret stack
	//int32_t ret_added = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		auto result = resolve_span_type(desc.subspan(match_position), type_subs, env);
		match_position += result.end_match_pos;
		//++ret_added;
	}

	return type_match_result{ true, consumed_stack_cells, consumed_rstack_cells };
}


inline void apply_stack_description(std::span<int32_t const> desc, state_stack& ts, environment& env) { // ret: true if function matched
	int32_t match_position = 0;
	// stack matching

	std::vector<int32_t> type_subs;

	auto const ssize = int32_t(ts.main_size());
	int32_t consumed_stack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		if(consumed_stack_cells >= ssize)
			return;
		auto match_result = fill_in_variable_types(ts.main_type_back(consumed_stack_cells), desc.subspan(match_position), type_subs, env);
		if(!match_result.match_result)
			return;
		match_position += match_result.match_end;
		++consumed_stack_cells;
	}

	++match_position; // skip -1

	// skip over output stack
	int32_t first_output_stack = match_position;
	//int32_t output_stack_types = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		match_position += next_encoded_stack_type(desc.subspan(match_position));
		//++output_stack_types;
	}
	//int32_t last_output_stack = match_position;
	++match_position; // skip -1

	// return stack matching
	auto const rsize = int32_t(ts.return_size());
	int32_t consumed_rstack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		if(consumed_rstack_cells >= rsize)
			return;
		auto match_result = fill_in_variable_types(ts.return_type_back(consumed_rstack_cells), desc.subspan(match_position), type_subs, env);
		if(!match_result.match_result)
			return;
		match_position += match_result.match_end;
		++consumed_rstack_cells;
	}

	++match_position; // skip -1

	// drop consumed types
	ts.resize(ssize - consumed_stack_cells, rsize - consumed_rstack_cells);

	// add returned stack types
	while(first_output_stack < int32_t(desc.size()) && desc[first_output_stack] != -1) {
		auto result = resolve_span_type(desc.subspan(first_output_stack), type_subs, env);
		first_output_stack += result.end_match_pos;
		ts.push_back_main(result.type, 0, nullptr);
	}

	// output ret stack
	// add new types
	//int32_t ret_added = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		auto result = resolve_span_type(desc.subspan(match_position), type_subs, env);
		ts.push_back_return(result.type, 0, nullptr);
		match_position += result.end_match_pos;
		//++ret_added;
	}
}


struct stack_consumption {
	int32_t stack;
	int32_t rstack;
};
inline stack_consumption get_stack_consumption(int32_t word, int32_t alternative, environment& env) {
	int32_t count_desc = 0;
	int32_t desc_offset = 0;
	if(std::holds_alternative<interpreted_word_instance>(env.dict.all_instances[alternative])) {
		count_desc = std::get<interpreted_word_instance>(env.dict.all_instances[alternative]).stack_types_count;
		desc_offset = std::get<interpreted_word_instance>(env.dict.all_instances[alternative]).stack_types_start;
	} else if(std::holds_alternative<compiled_word_instance>(env.dict.all_instances[alternative])) {
		count_desc = std::get<compiled_word_instance>(env.dict.all_instances[alternative]).stack_types_count;
		desc_offset = std::get<compiled_word_instance>(env.dict.all_instances[alternative]).stack_types_start;
	}

	std::span<const int> desc = std::span<const int>(env.dict.all_stack_types.data() + desc_offset, size_t(count_desc));

	int32_t match_position = 0;
	// stack matching

	int32_t consumed_stack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		match_position += next_encoded_stack_type(desc.subspan(match_position));
		++consumed_stack_cells;
	}

	++match_position; // skip -1

	// skip over output stack
	int32_t first_output_stack = match_position;
	//int32_t output_stack_types = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		match_position += next_encoded_stack_type(desc.subspan(match_position));
		//++output_stack_types;
	}

	++match_position; // skip -1

	// return stack matching
	
	int32_t consumed_rstack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		match_position += next_encoded_stack_type(desc.subspan(match_position));
		++consumed_rstack_cells;
	}

	return stack_consumption{ consumed_stack_cells, consumed_rstack_cells };
}

struct word_match_result {
	bool matched = false;
	int32_t stack_consumed = 0;
	int32_t ret_stack_consumed = 0;
	int32_t word_index = 0;
	int32_t substitution_version = 0;
};

inline word_match_result match_word(word const& w, state_stack& ts, std::vector<word_types> const& all_instances, std::vector<int32_t> const& all_stack_types, environment& env) {
	std::vector<int32_t> type_subs;
	for(uint32_t i = 0; i < w.instances.size(); ++i) {
		if(std::holds_alternative<interpreted_word_instance>(all_instances[w.instances[i]])) {
			interpreted_word_instance const& s = std::get<interpreted_word_instance>(all_instances[w.instances[i]]);
			auto mr = match_stack_description(std::span<int32_t const>(all_stack_types.data() + s.stack_types_start, size_t(s.stack_types_count)), ts, env, type_subs);
			if(mr.matched) {
				return word_match_result{ mr.matched, mr.stack_consumed, mr.ret_stack_consumed, w.instances[i], 0 };
			}
			type_subs.clear();
		} else if(std::holds_alternative<compiled_word_instance>(all_instances[w.instances[i]])) {
			compiled_word_instance const& s = std::get<compiled_word_instance>(all_instances[w.instances[i]]);
			auto mr = match_stack_description(std::span<int32_t const>(all_stack_types.data() + s.stack_types_start, size_t(s.stack_types_count)), ts, env, type_subs);
			if(mr.matched) {
				return word_match_result{ mr.matched, mr.stack_consumed, mr.ret_stack_consumed, w.instances[i], 0 };
			}
			type_subs.clear();
		}
	}
	return word_match_result{ false, 0, 0, 0, 0 };
}

inline std::string_view read_word(std::string_view source, uint32_t& read_position) {
	uint32_t first_non_space = read_position;
	while(first_non_space < source.length() && (source[first_non_space] == ' ' || source[first_non_space] == '\t'))
		++first_non_space;

	auto word_end = first_non_space;
	while(word_end < source.length() && source[word_end] != ' ' && source[word_end] != '\t')
		++word_end;

	read_position = word_end;

	return source.substr(first_non_space, word_end - first_non_space);
}

inline void execute_fif_word(interpreted_word_instance& wi, state_stack& ss, environment& env) {
	int32_t* ptr = wi.compiled_bytecode.data();
	while(ptr) {
		fif_call fn = nullptr;
		memcpy(&fn, ptr, 8);
		ptr = fn(ss, ptr, &env);
#ifdef HEAP_CHECKS
		assert(_CrtCheckMemory());
#endif
	}
}
inline void execute_fif_word(compiled_word_instance& wi, state_stack& ss, environment& env) {
	wi.implementation(ss, nullptr, &env);
}
inline void execute_fif_word(state_stack& ss, int32_t* ptr, environment& env) {
	while(ptr) {
		fif_call fn = nullptr;
		memcpy(&fn, ptr, 8);
		ptr = fn(ss, ptr, &env);
#ifdef HEAP_CHECKS
		assert(_CrtCheckMemory());
#endif
	}
}

inline int32_t* immediate_i32(state_stack& s, int32_t* p, environment*) {
	int32_t data = 0;
	memcpy(&data, p + 2, 4);
	s.push_back_main(fif_i32, data, nullptr);
	return p + 3;
}
inline int32_t* immediate_f32(state_stack& s, int32_t* p, environment*) {
	int64_t data = 0;
	memcpy(&data, p + 2, 4);
	s.push_back_main(fif_f32, data, nullptr);
	return p + 3;
}
inline int32_t* immediate_bool(state_stack& s, int32_t* p, environment*) {
	int32_t data = 0;
	memcpy(&data, p + 2, 4);
	s.push_back_main(fif_bool, data, nullptr);
	return p + 3;
}
inline void do_immediate_i32(state_stack& s, int32_t value, environment* e) {
	if(typechecking_failed(e->mode))
		return;

	auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
	if(compile_bytes && e->mode == fif_mode::compiling_bytecode) {
		fif_call imm = immediate_i32;
		uint64_t imm_bytes = 0;
		memcpy(&imm_bytes, &imm, 8);
		compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
		compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
		compile_bytes->push_back(value);
	}
	LLVMValueRef val = nullptr;
#ifdef USE_LLVM
	if(e->mode == fif_mode::compiling_llvm) {
		val = LLVMConstInt(LLVMInt32TypeInContext(e->llvm_context), uint32_t(value), true);
	}
#endif
	s.push_back_main(fif_i32, value, val);
}
inline int32_t* immediate_type(state_stack& s, int32_t* p, environment*) {
	int32_t data = 0;
	memcpy(&data, p + 2, 4);
	s.push_back_main(fif_type, data, nullptr);
	return p + 3;
}
inline void do_immediate_type(state_stack& s, int32_t value, environment* e) {
	if(typechecking_failed(e->mode))
		return;

	auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
	if(compile_bytes && e->mode == fif_mode::compiling_bytecode) {
		fif_call imm = immediate_type;
		uint64_t imm_bytes = 0;
		memcpy(&imm_bytes, &imm, 8);
		compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
		compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
		compile_bytes->push_back(value);
	}
	LLVMValueRef val = nullptr;
#ifdef USE_LLVM
	if(e->mode == fif_mode::compiling_llvm) {
		val = LLVMConstInt(LLVMInt32TypeInContext(e->llvm_context), uint32_t(value), true);
	}
#endif
	s.push_back_main(fif_type, value, val);
}
inline void do_immediate_bool(state_stack& s, bool value, environment* e) {
	if(typechecking_failed(e->mode))
		return;

	auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
	if(compile_bytes && e->mode == fif_mode::compiling_bytecode) {
		fif_call imm = immediate_bool;
		uint64_t imm_bytes = 0;
		memcpy(&imm_bytes, &imm, 8);
		compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
		compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
		compile_bytes->push_back(int32_t(value));
	}
	LLVMValueRef val = nullptr;
#ifdef USE_LLVM
	if(e->mode == fif_mode::compiling_llvm) {
		val = LLVMConstInt(LLVMInt1TypeInContext(e->llvm_context), uint32_t(value), true);
	}
#endif
	s.push_back_main(fif_bool, int64_t(value), val);
}
inline void do_immediate_f32(state_stack& s, float value, environment* e) {
	if(typechecking_failed(e->mode))
		return;

	int32_t v4 = 0;
	int64_t v8 = 0;
	memcpy(&v4, &value, 4);
	memcpy(&v8, &value, 4);

	auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
	if(compile_bytes && e->mode == fif_mode::compiling_bytecode) {
		fif_call imm = immediate_f32;
		uint64_t imm_bytes = 0;
		memcpy(&imm_bytes, &imm, 8);
		compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
		compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
		compile_bytes->push_back(v4);
	}
	LLVMValueRef val = nullptr;
#ifdef USE_LLVM
	if(e->mode == fif_mode::compiling_llvm) {
		val = LLVMConstReal(LLVMFloatTypeInContext(e->llvm_context), value);
	}
#endif
	s.push_back_main(fif_f32, v8, val);
}
inline int32_t* function_return(state_stack& s, int32_t* p, environment*) {
	return nullptr;
}

struct parse_result {
	std::string_view content;
	bool is_string = false;
};

inline void execute_fif_word(parse_result word, environment& env, bool ignore_specializations);

#ifdef USE_LLVM
inline LLVMTypeRef llvm_function_type_from_desc(environment& env, std::span<int32_t const> desc) {
	std::vector<LLVMTypeRef> parameter_group;
	std::vector<LLVMTypeRef> returns_group;

	/*
	* NOTE: function assumes that description is fully resolved
	*/

	int32_t match_position = 0;
	// stack matching

	//int32_t consumed_stack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		parameter_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
		++match_position;
		//++consumed_stack_cells;
	}

	++match_position; // skip -1

	// output stack
	int32_t first_output_stack = match_position;
	//int32_t output_stack_types = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		returns_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
		++match_position;
		//++output_stack_types;
	}
	++match_position; // skip -1

	// return stack matching
	//int32_t consumed_rstack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		parameter_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
		++match_position;
		//++consumed_rstack_cells;
	}

	++match_position; // skip -1

	// output ret stack
	//int32_t ret_added = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		returns_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
		++match_position;
		//++ret_added;
	}

	LLVMTypeRef ret_type = nullptr;
	if(returns_group.size() == 0) {
		ret_type = LLVMVoidTypeInContext(env.llvm_context);
	} else if(returns_group.size() == 1) {
		ret_type = returns_group[0];
	} else {
		ret_type = LLVMStructTypeInContext(env.llvm_context, returns_group.data(), uint32_t(returns_group.size()), false);
	}
	return LLVMFunctionType(ret_type, parameter_group.data(), uint32_t(parameter_group.size()), false);
}

inline void llvm_make_function_parameters(environment& env, LLVMValueRef fn, state_stack& ws, std::span<int32_t const> desc) {

	/*
	* NOTE: function assumes that description is fully resolved
	*/

	int32_t match_position = 0;
	// stack matching

	int32_t consumed_stack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		ws.set_main_ex_back(consumed_stack_cells, LLVMGetParam(fn, uint32_t(consumed_stack_cells)));
		++match_position;
		++consumed_stack_cells;
	}

	++match_position; // skip -1

	// output stack
	int32_t first_output_stack = match_position;
	//int32_t output_stack_types = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		++match_position;
		//++output_stack_types;
	}
	++match_position; // skip -1

	// return stack matching
	int32_t consumed_rstack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		ws.set_return_ex_back(consumed_rstack_cells, LLVMGetParam(fn, uint32_t(consumed_stack_cells + consumed_stack_cells)));
		++match_position;
		++consumed_rstack_cells;
	}

	++match_position; // skip -1

	// output ret stack
	//int32_t ret_added = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		++match_position;
		//++ret_added;
	}
}

struct brief_fn_return {
	LLVMTypeRef composite_type = nullptr;
	int32_t num_stack_values;
	int32_t num_rstack_values;
	bool is_struct_type;
};

inline brief_fn_return llvm_function_return_type_from_desc(environment& env, std::span<int32_t const> desc) {
	std::vector<LLVMTypeRef> returns_group;

	/*
	* NOTE: function assumes that description is fully resolved
	*/

	int32_t match_position = 0;
	// stack matching

	//int32_t consumed_stack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		//parameter_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
		++match_position;
		//++consumed_stack_cells;
	}

	++match_position; // skip -1

	// output stack
	int32_t first_output_stack = match_position;
	int32_t output_stack_types = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		returns_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
		++match_position;
		++output_stack_types;
	}
	++match_position; // skip -1

	// return stack matching
	//int32_t consumed_rstack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		//parameter_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
		++match_position;
		//++consumed_rstack_cells;
	}

	++match_position; // skip -1

	// output ret stack
	int32_t ret_added = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		returns_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
		++match_position;
		++ret_added;
	}

	LLVMTypeRef ret_type = nullptr;
	if(returns_group.size() == 0) {
		// leave as nullptr
	} else if(returns_group.size() == 1) {
		ret_type = returns_group[0];
	} else {
		ret_type = LLVMStructTypeInContext(env.llvm_context, returns_group.data(), uint32_t(returns_group.size()), false);
	}
	return brief_fn_return{ ret_type , output_stack_types, ret_added, returns_group.size()  > 1};
}

inline void llvm_make_function_return(environment& env, std::span<int32_t const> desc) {
	auto rsummary = llvm_function_return_type_from_desc( env, desc);
	
	if(rsummary.composite_type == nullptr) {
		LLVMBuildRetVoid(env.llvm_builder);
		return;
	}
	if(rsummary.is_struct_type == false) {
		if(rsummary.num_stack_values == 0) {
			LLVMBuildRet(env.llvm_builder, env.compiler_stack.back()->working_state()->return_ex_back(0));
			return;
		} else if(rsummary.num_rstack_values == 0) {
			LLVMBuildRet(env.llvm_builder, env.compiler_stack.back()->working_state()->main_ex_back(0));
			return;
		} else {
			assert(false);
		}
	}

	auto rstruct = LLVMGetUndef(rsummary.composite_type);
	uint32_t insert_index = 0;

	for(int32_t i = rsummary.num_stack_values - 1; i >= 0; --i) {
		rstruct = LLVMBuildInsertValue(env.llvm_builder, rstruct, env.compiler_stack.back()->working_state()->main_ex_back(i), insert_index, "");
		++insert_index;
	}
	for(int32_t i = rsummary.num_rstack_values - 1; i >= 0;  --i) {
		rstruct = LLVMBuildInsertValue(env.llvm_builder, rstruct, env.compiler_stack.back()->working_state()->return_ex_back(i), insert_index, "");
		++insert_index;
	}
	LLVMBuildRet(env.llvm_builder, rstruct);
	// LLVMBuildAggregateRet(env.llvm_builder, LLVMValueRef * RetVals, unsigned N);
}

inline void llvm_make_function_call(environment& env, LLVMValueRef fn, std::span<int32_t const> desc) {
	std::vector<LLVMValueRef> params;
	
	{
		int32_t match_position = 0;
		// stack matching

		//int32_t consumed_stack_cells = 0;
		while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
			params.push_back(env.compiler_stack.back()->working_state()->main_ex_back(0));
			env.compiler_stack.back()->working_state()->pop_main();
			++match_position;
			//++consumed_stack_cells;
		}

		++match_position; // skip -1

		// output stack
		int32_t first_output_stack = match_position;
		//int32_t output_stack_types = 0;
		while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
			//returns_group.push_back(env.dict.type_array[desc[match_position]].llvm_type.llvm_type);
			++match_position;
			//++output_stack_types;
		}
		++match_position; // skip -1

		// return stack matching
		//int32_t consumed_rstack_cells = 0;
		while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
			params.push_back(env.compiler_stack.back()->working_state()->return_ex_back(0));
			env.compiler_stack.back()->working_state()->pop_return();
			++match_position;
			//++consumed_rstack_cells;
		}
	}

	assert(fn);
	auto retvalue = LLVMBuildCall2(env.llvm_builder, llvm_function_type_from_desc(env, desc), fn, params.data(), uint32_t(params.size()), "");
	LLVMSetInstructionCallConv(retvalue, LLVMCallConv::LLVMFastCallConv);
	auto rsummary = llvm_function_return_type_from_desc(env, desc);

	if(rsummary.composite_type == nullptr) {
		return;
	}

	{
		uint32_t extract_index = 0;
		int32_t match_position = 0;
		// stack matching

		//int32_t consumed_stack_cells = 0;
		while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
			//parameter_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
			++match_position;
			//++consumed_stack_cells;
		}

		++match_position; // skip -1

		// output stack
		int32_t first_output_stack = match_position;
		//int32_t output_stack_types = 0;
		while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
			if(rsummary.is_struct_type == false) { // single return value
				env.compiler_stack.back()->working_state()->push_back_main(desc[match_position], 0, retvalue);
			} else {
				env.compiler_stack.back()->working_state()->push_back_main(desc[match_position], 0, LLVMBuildExtractValue(env.llvm_builder, retvalue, extract_index, ""));
			}
			++extract_index;
			++match_position;
			//++output_stack_types;
		}
		++match_position; // skip -1

		// return stack matching
		//int32_t consumed_rstack_cells = 0;
		while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
			//parameter_group.push_back(env.dict.type_array[desc[match_position]].llvm_type);
			++match_position;
			//++consumed_rstack_cells;
		}

		++match_position; // skip -1

		// output ret stack
		//int32_t ret_added = 0;
		while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
			if(rsummary.is_struct_type == false) { // single return value
				env.compiler_stack.back()->working_state()->push_back_return(desc[match_position], 0, retvalue);
			} else {
				env.compiler_stack.back()->working_state()->push_back_return(desc[match_position], 0, LLVMBuildExtractValue(env.llvm_builder, retvalue, extract_index, ""));
			}
			++extract_index;
			++match_position;
			//++ret_added;
		}
	}
}
#endif

inline std::vector<int32_t> expand_stack_description(state_stack& initial_stack_state, std::span<const int32_t> desc, int32_t stack_consumed, int32_t rstack_consumed);
inline bool compare_stack_description(std::span<const int32_t> a, std::span<const int32_t> b);

class locals_holder : public opaque_compiler_data {
public:
	ankerl::unordered_dense::map<std::string, std::unique_ptr<var_data>> vars;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<let_data>> lets;
	environment& env;
	const bool llvm_mode;

	locals_holder(opaque_compiler_data* p, environment& env) : opaque_compiler_data(p), env(env), llvm_mode(env.mode == fif_mode::compiling_llvm) {
	}

	virtual var_data* get_var(std::string const& name)override {
		if(auto it = vars.find(name); it != vars.end()) {
			return it->second.get();
		}
		return parent->get_var(name);
	}
	virtual var_data* create_var(std::string const& name, int32_t type)override {
		if(auto it = lets.find(name); it != lets.end()) {
			return nullptr;
		}
		if(auto it = vars.find(name); it != vars.end()) {
			return nullptr;
		}

		auto added = vars.insert_or_assign(name, std::make_unique<var_data>());
		added.first->second->type = type;
		added.first->second->data = 0;
#ifdef USE_LLVM
		added.first->second->alloc = llvm_mode ? LLVMBuildAlloca(env.llvm_builder, env.dict.type_array[type].llvm_type, name.c_str()) : nullptr;
#endif
		return added.first->second.get();
	}
	virtual let_data* get_let(std::string const& name) override {
		if(auto it = lets.find(name); it != lets.end()) {
			return it->second.get();
		}
		return parent->get_let(name);
	}
	virtual let_data* create_let(std::string const& name, int32_t type, int64_t data, LLVMValueRef expression)override {
		if(auto it = lets.find(name); it != lets.end()) {
			return nullptr;
		}
		if(auto it = vars.find(name); it != vars.end()) {
			return nullptr;
		}
		auto added = lets.insert_or_assign(name, std::make_unique<let_data>());
		added.first->second->type = type;
		added.first->second->data = data;
		added.first->second->expression = expression;
		if(auto it = lets.find(name); it != lets.end()) {
			return  it->second.get();
		}
		return nullptr;
	}

	void release_locals() {
		auto* ws = env.compiler_stack.back()->working_state();
		for(auto& l : vars) {
			if(env.dict.type_array[l.second->type].flags != 0) {
#ifdef USE_LLVM
				auto iresult = LLVMBuildLoad2(env.llvm_builder, env.dict.type_array[l.second->type].llvm_type, l.second->alloc, "");
#else
				void* iresult = nullptr;
#endif
				ws->push_back_main(l.second->type, l.second->data, iresult);
				execute_fif_word(fif::parse_result{ "drop", false }, env, false);
			}
		}
		for(auto& l : lets) {
			if(env.dict.type_array[l.second->type].flags != 0) {
				ws->push_back_main(l.second->type, l.second->data, l.second->expression);
				execute_fif_word(fif::parse_result{ "drop", false }, env, false);
			}
		}
		vars.clear();
		lets.clear();
	}
};

class function_scope : public locals_holder {
public:
	std::unique_ptr<state_stack> initial_state;
	std::unique_ptr<state_stack> iworking_state;
	std::vector<int32_t> compiled_bytes;
	std::vector<int32_t> type_subs;
	LLVMValueRef llvm_fn = nullptr;
	LLVMBasicBlockRef current_block = nullptr;
	int32_t for_word = -1;
	int32_t for_instance = -1;


	function_scope(opaque_compiler_data* p, environment& e, state_stack& entry_state, int32_t for_word, int32_t for_instance) : locals_holder(p, e), for_word(for_word), for_instance(for_instance) {
		
		initial_state = entry_state.copy();
		iworking_state = entry_state.copy();

		if(for_word == -1) {
			env.report_error("attempted to compile a function for an anonymous word");
			env.mode = fif_mode::error;
			return;
		}

		
			if(typechecking_mode(env.mode))
				env.dict.word_array[for_word].being_typechecked = true;
			else if(env.mode == fif_mode::compiling_bytecode || env.mode == fif_mode::compiling_llvm) {
				assert(for_instance != -1);
				std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).being_compiled = true;
			}

			if(env.mode == fif_mode::compiling_llvm) {
				assert(for_instance != -1);
				auto fn_desc = std::span<int32_t const>(env.dict.all_stack_types.data() + std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).stack_types_start, std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).stack_types_count);

#ifdef USE_LLVM
				if(!std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).llvm_function) {
					auto fn_string_name = word_name_from_id(for_word, env) + "#" + std::to_string(for_instance);
					auto fn_type = llvm_function_type_from_desc(env, fn_desc);
					std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).llvm_function = LLVMAddFunction(env.llvm_module, fn_string_name.c_str(), fn_type);
				}
#endif
				auto compiled_fn = std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).llvm_function;
				llvm_fn = compiled_fn;

#ifdef USE_LLVM
				if(std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).is_imported_function)
					LLVMSetFunctionCallConv(compiled_fn, NATIVE_CC);
				else
					LLVMSetFunctionCallConv(compiled_fn, LLVMCallConv::LLVMFastCallConv);

				LLVMSetLinkage(compiled_fn, LLVMLinkage::LLVMPrivateLinkage);
				auto entry_block = LLVMAppendBasicBlockInContext(env.llvm_context, compiled_fn, "fn_entry_point");
				LLVMPositionBuilderAtEnd(env.llvm_builder, entry_block);
				llvm_make_function_parameters(env, compiled_fn, *iworking_state, fn_desc);

				current_block = entry_block;
#endif
			}
		
	}
	virtual std::vector<int32_t>* type_substitutions()override {
		return &type_subs;
	}
	virtual control_structure get_type()override {
		return control_structure::function;
	}
	virtual LLVMValueRef llvm_function()override {
		return llvm_fn;
	}
	virtual LLVMBasicBlockRef* llvm_block() override {
		return &current_block;
	}
	virtual int32_t word_id()override {
		return for_word;
	}
	virtual int32_t instance_id()override {
		return for_instance;
	}
	virtual std::vector<int32_t>* bytecode_compilation_progress()override {
		return &compiled_bytes;
	}
	virtual ankerl::unordered_dense::map<uint64_t, typecheck_3_record>* typecheck_record()override {
		return parent ? parent->typecheck_record() : nullptr;
	}
	virtual state_stack* working_state()override {
		return iworking_state.get();
	}
	virtual void set_working_state(std::unique_ptr<state_stack> p) override {
		iworking_state = std::move(p);
	}
	virtual bool finish(environment&)override {
		int32_t stack_consumed = int32_t(initial_state->main_size()) - int32_t(iworking_state->min_main_depth);
		int32_t rstack_consumed = int32_t(initial_state->return_size()) - int32_t(iworking_state->min_return_depth);
		int32_t stack_added = int32_t(iworking_state->main_size()) - int32_t(iworking_state->min_main_depth);
		int32_t rstack_added = int32_t(iworking_state->return_size()) - int32_t(iworking_state->min_return_depth);
		assert(stack_added >= 0);
		assert(rstack_added >= 0);
		assert(stack_consumed >= 0);
		assert(rstack_consumed >= 0);

		if(env.mode == fif_mode::typechecking_lvl_1) {
			word& w = env.dict.word_array[for_word];

			interpreted_word_instance temp;
			auto& current_stack = *iworking_state;

			temp.stack_types_start = int32_t(env.dict.all_stack_types.size());
			{
				for(int32_t i = 0; i < stack_consumed; ++i) {
					env.dict.all_stack_types.push_back(initial_state->main_type_back(i));
				}
			}
			int32_t skipped = 0;
			if(stack_added > 0) {
				env.dict.all_stack_types.push_back(-1);
				for(int32_t i = 1; i <= stack_added; ++i) {
					env.dict.all_stack_types.push_back(current_stack.main_type_back(stack_added - i));
				}
			} else {
				++skipped;
			}
			if(rstack_consumed > 0) {
				for(; skipped > 0; --skipped)
					env.dict.all_stack_types.push_back(-1);
				env.dict.all_stack_types.push_back(-1);
				for(int32_t i = 0; i < rstack_consumed; ++i) {
					env.dict.all_stack_types.push_back(initial_state->return_type_back(i));
				}
			} else {
				++skipped;
			}
			if(rstack_added > 0) {
				for(; skipped > 0; --skipped)
					env.dict.all_stack_types.push_back(-1);
				env.dict.all_stack_types.push_back(-1);
				for(int32_t i = 1; i <= rstack_added; ++i) {
					env.dict.all_stack_types.push_back(current_stack.return_type_back(rstack_added - i));
				}
			}
			temp.stack_types_count = int32_t(env.dict.all_stack_types.size()) - temp.stack_types_start;
			temp.typechecking_level = 1;

			if(for_instance == -1) {
				w.instances.push_back(int32_t(env.dict.all_instances.size()));
				env.dict.all_instances.push_back(std::move(temp));
			} else if(std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).typechecking_level == 0) {
				env.dict.all_instances[for_instance] = std::move(temp);
			}

			env.dict.word_array[for_word].being_typechecked = false;
		} else if(env.mode == fif_mode::typechecking_lvl_2) {
			if(for_instance == -1) {
				env.report_error("tried to level 2 typecheck a word without an instance");
				env.mode = fif_mode::error;
				return true;
			}
			interpreted_word_instance& wi = std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]);
			std::span<const int32_t> existing_description = std::span<const int32_t>(env.dict.all_stack_types.data() + wi.stack_types_start, size_t(wi.stack_types_count));
			auto revised_description = expand_stack_description(*initial_state, existing_description, stack_consumed, rstack_consumed);

			if(!compare_stack_description(existing_description, std::span<const int32_t>(revised_description.data(), revised_description.size()))) {
				wi.stack_types_start = int32_t(env.dict.all_stack_types.size());
				wi.stack_types_count = int32_t(revised_description.size());
				env.dict.all_stack_types.insert(env.dict.all_stack_types.end(), revised_description.begin(), revised_description.end());
			}

			wi.typechecking_level = std::max(wi.typechecking_level, 2);
			env.dict.word_array[for_word].being_typechecked = false;
		} else if(env.mode == fif_mode::typechecking_lvl_3) {
			// no alterations -- handled explicitly by invoking lvl 3
			env.dict.word_array[for_word].being_typechecked = false;
		} else if(env.mode == fif_mode::compiling_bytecode) {
			if(for_instance == -1) {
				env.report_error("tried to compile a word without an instance");
				env.mode = fif_mode::error;
				return true;
			}

			fif_call imm = function_return;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compiled_bytes.push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compiled_bytes.push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));

			interpreted_word_instance& wi = std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]);
			wi.compiled_bytecode = std::move(compiled_bytes);

			if(for_instance != -1)
				std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).being_compiled = false;
		} else if(env.mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
			if(for_instance == -1) {
				env.report_error("tried to compile a word without an instance");
				env.mode = fif_mode::error;
				return true;
			}

			release_locals(); 

			interpreted_word_instance& wi = std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]);
			std::span<const int32_t> existing_description = std::span<const int32_t>(env.dict.all_stack_types.data() + wi.stack_types_start, size_t(wi.stack_types_count));
			llvm_make_function_return(env, existing_description);
			wi.llvm_compilation_finished = true;

			if(LLVMVerifyFunction(wi.llvm_function, LLVMVerifierFailureAction::LLVMPrintMessageAction)) {
				env.report_error("LLVM verification of function failed");
				env.mode = fif_mode::error;
				return true;
			}

			if(for_instance != -1)
				std::get<interpreted_word_instance>(env.dict.all_instances[for_instance]).being_compiled = false;
#endif
		} else if(typechecking_failed(env.mode)) {
			env.dict.word_array[for_word].being_typechecked = false;
			return true;
		} else {
			env.report_error("reached end of compilation in unexpected mode");
			env.mode = fif_mode::error;
			return true;
		}
		return true;
	}
};

class runtime_function_scope : public locals_holder {
public:
	runtime_function_scope(opaque_compiler_data* p, environment& e) : locals_holder(p, e) {
	}
	virtual bool finish(environment&) override {
		release_locals();
		return true;
	}
	virtual control_structure get_type() override {
		return control_structure::function;
	}
};
inline int32_t* call_function(state_stack& s, int32_t* p, environment* env) {
	int32_t* jump_target = nullptr;
	memcpy(&jump_target, p + 2, 8);
	env->compiler_stack.push_back(std::make_unique<runtime_function_scope>(env->compiler_stack.back().get(), *env));
	execute_fif_word(s, jump_target, *env);
	env->compiler_stack.back()->finish(*env);
	env->compiler_stack.pop_back();
	return p + 4;
}
inline int32_t* call_function_indirect(state_stack& s, int32_t* p, environment* env) {
	auto instance = *(p + 2);
	env->compiler_stack.push_back(std::make_unique<runtime_function_scope>(env->compiler_stack.back().get(), *env));
	execute_fif_word(std::get<interpreted_word_instance>(env->dict.all_instances[instance]), s, *env);
	env->compiler_stack.back()->finish(*env);
	env->compiler_stack.pop_back();
	return p + 3;
}
inline int32_t* enter_scope(state_stack& s, int32_t* p, environment* env) {
	env->compiler_stack.push_back(std::make_unique<runtime_function_scope>(env->compiler_stack.back().get(), *env));
	return p + 2;
}
inline int32_t* leave_scope(state_stack& s, int32_t* p, environment* env) {
	env->compiler_stack.back()->finish(*env);
	env->compiler_stack.pop_back();
	return p + 2;
}

class mode_switch_scope : public opaque_compiler_data {
public:
	opaque_compiler_data* interpreted_link = nullptr;
	fif_mode entry_mode;
	fif_mode running_mode;

	mode_switch_scope(opaque_compiler_data* p, environment& env, fif_mode running_mode) : opaque_compiler_data(p), running_mode(running_mode) {
		[&]() {
			for(auto i = env.compiler_stack.size(); i-- > 0; ) {
				if(env.compiler_stack[i]->get_type() == control_structure::mode_switch) {
					mode_switch_scope* p = static_cast<mode_switch_scope*>(env.compiler_stack[i].get());
					if(p->running_mode == running_mode) {
						++i;
						for(; i < env.compiler_stack.size(); ++i) {
							if(env.compiler_stack[i]->get_type() == control_structure::mode_switch) {
								interpreted_link = env.compiler_stack[i - 1].get();
								return;
							}
						}
						interpreted_link = env.compiler_stack[env.compiler_stack.size() - 1].get();
						return;
					}
				}
			}
		}();
		entry_mode = env.mode;
		env.mode = running_mode;
	}

	virtual control_structure get_type()override {
		return control_structure::mode_switch;
	}
	virtual ankerl::unordered_dense::map<uint64_t, typecheck_3_record>* typecheck_record() override {
		return parent ? parent->typecheck_record() : nullptr;
	}
	virtual state_stack* working_state() override {
		return interpreted_link ? interpreted_link->working_state() : nullptr;
	}
	virtual var_data* get_var(std::string const& name) override {
		return interpreted_link ? interpreted_link->get_var(name) : nullptr;
	}
	virtual var_data* create_var(std::string const& name, int32_t type) override {
		return interpreted_link ? interpreted_link->create_var(name, type) : nullptr;
	}
	virtual let_data* get_let(std::string const& name) override {
		return interpreted_link ? interpreted_link->get_let(name) : nullptr;
	}
	virtual let_data* create_let(std::string const& name, int32_t type, int64_t data, LLVMValueRef expression)override {
		return interpreted_link ? interpreted_link->create_let(name, type, data, expression) : nullptr;
	}
	virtual void set_working_state(std::unique_ptr<state_stack> p)override {
		if(interpreted_link)
			interpreted_link->set_working_state(std::move(p));
	}
	virtual bool finish(environment& env)override {
		if(env.mode != fif_mode::error) {
			env.mode = entry_mode;
		}
		return true;
	}
};
inline void switch_compiler_stack_mode(environment& env, fif_mode new_mode) {
	auto m = std::make_unique<mode_switch_scope>(env.compiler_stack.back().get(), env, new_mode);
	env.compiler_stack.emplace_back(std::move(m));
}
inline void restore_compiler_stack_mode(environment& env) {
	if(env.compiler_stack.empty() || env.compiler_stack.back()->get_type() != control_structure::mode_switch) {
		env.report_error("attempt to switch mode back revealed an unbalanced compiler stack");
		env.mode = fif_mode::error;
		return;
	}
	env.compiler_stack.back()->finish(env);
	env.compiler_stack.pop_back();
}
class outer_interpreter : public opaque_compiler_data {
public:
	std::unique_ptr<state_stack> interpreter_state;

	ankerl::unordered_dense::map<std::string, std::unique_ptr<var_data>> global_vars;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<let_data>> global_lets;

	virtual var_data* get_var(std::string const& name) override {
		if(auto it = global_vars.find(name); it != global_vars.end()) {
			return it->second.get();
		}
		return nullptr;
	}
	virtual var_data* create_var(std::string const& name, int32_t type) override {
		if(auto it = global_lets.find(name); it != global_lets.end()) {
			return nullptr;
		}
		if(auto it = global_vars.find(name); it != global_vars.end()) {
			return nullptr;
		}
		auto added = global_vars.insert_or_assign(name, std::make_unique<var_data>());
		added.first->second->type = type;
		added.first->second->data = 0;
		added.first->second->alloc = nullptr;
		return added.first->second.get();
	}
	virtual let_data* get_let(std::string const& name) override {
		if(auto it = global_lets.find(name); it != global_lets.end()) {
			return it->second.get();
		}
		return nullptr;
	}
	virtual let_data* create_let(std::string const& name, int32_t type, int64_t data, LLVMValueRef expression) override {
		if(auto it = global_lets.find(name); it != global_lets.end()) {
			return nullptr;
		}
		if(auto it = global_vars.find(name); it != global_vars.end()) {
			return nullptr;
		}
		auto added = global_lets.insert_or_assign(name, std::make_unique<let_data>());
		added.first->second->type = type;
		added.first->second->data = data;
		added.first->second->expression = expression;
		if(auto it = global_lets.find(name); it != global_lets.end()) {
			return it->second.get();
		}
		return nullptr;
	}

	outer_interpreter(environment& env) : opaque_compiler_data(nullptr), interpreter_state(std::make_unique<interpreter_stack>()) {
	}
	virtual control_structure get_type() override {
		return control_structure::none;
	}
	virtual state_stack* working_state()override {
		return interpreter_state.get();
	}
	virtual void set_working_state(std::unique_ptr<state_stack> p) override {
		interpreter_state = std::move(p);
	}
	virtual bool finish(environment& env)override {
		return true;
	}
};
class typecheck3_record_holder : public opaque_compiler_data {
public:
	ankerl::unordered_dense::map<uint64_t, typecheck_3_record> tr;

	typecheck3_record_holder(opaque_compiler_data* p, environment& env) : opaque_compiler_data(p) {
	}

	virtual control_structure get_type() override {
		return control_structure::none;
	}
	virtual ankerl::unordered_dense::map<uint64_t, typecheck_3_record>* typecheck_record()override {
		return &tr;
	}
	virtual bool finish(environment& env)override {
		return true;
	}
};


inline std::vector<int32_t> expand_stack_description(state_stack& initial_stack_state, std::span<const int32_t> desc, int32_t stack_consumed, int32_t rstack_consumed) {
	int32_t match_position = 0;
	// stack matching

	std::vector<int32_t> result;

	int32_t consumed_stack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		auto match_size = next_encoded_stack_type(desc.subspan(match_position));
		result.insert(result.end(), desc.data() + match_position, desc.data() + match_position + match_size);
		match_position += match_size;
		++consumed_stack_cells;
	}
	if(consumed_stack_cells < stack_consumed) {
		for(int32_t i = consumed_stack_cells; i < stack_consumed; ++i) {
			result.push_back(initial_stack_state.main_type_back(i));
		}
	}

	if(consumed_stack_cells < stack_consumed || (match_position < int32_t(desc.size()) && desc[match_position] == -1)) {
		result.push_back(-1);
	}
	++match_position; // skip -1

	if(consumed_stack_cells < stack_consumed) { // add additional touched inputs to the end
		for(int32_t i = stack_consumed - 1; i >= consumed_stack_cells; --i) {
			result.push_back(initial_stack_state.main_type_back(i));
		}
	}

	int32_t first_output_stack = match_position;
	//int32_t output_stack_types = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		auto match_size = next_encoded_stack_type(desc.subspan(match_position));
		result.insert(result.end(), desc.data() + match_position, desc.data() + match_position + match_size);
		match_position += match_size;
		//++output_stack_types;
	}

	if(rstack_consumed > 0 || (match_position < int32_t(desc.size()) && desc[match_position] == -1)) {
		result.push_back(-1);
	}
	++match_position; // skip -1

	// return stack matching
	int32_t consumed_rstack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		auto match_size = next_encoded_stack_type(desc.subspan(match_position));
		result.insert(result.end(), desc.data() + match_position, desc.data() + match_position + match_size);
		match_position += match_size;
		++consumed_rstack_cells;
	}
	if(consumed_rstack_cells < rstack_consumed) {
		for(int32_t i = consumed_rstack_cells; i < rstack_consumed; ++i) {
			result.push_back(initial_stack_state.return_type_back(i));
		}
	}

	if(consumed_rstack_cells < rstack_consumed || (match_position < int32_t(desc.size()) && desc[match_position] == -1)) {
		result.push_back(-1);
	}
	++match_position; // skip -1

	if(consumed_rstack_cells < rstack_consumed) { // add additional touched inputs to the end
		for(int32_t i = rstack_consumed - 1; i >= consumed_rstack_cells; --i) {
			result.push_back(initial_stack_state.return_type_back(i));
		}
	}

	// add new types
	int32_t ret_added = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		auto match_size = next_encoded_stack_type(desc.subspan(match_position));
		result.insert(result.end(), desc.data() + match_position, desc.data() + match_position + match_size);
		match_position += match_size;
	}

	return result;
}

inline bool compare_stack_description(std::span<const int32_t> a, std::span<const int32_t> b) {
	if(a.size() != b.size())
		return false;
	for(auto i = a.size(); i-- > 0; ) {
		if(a[i] != b[i])
			return false;
	}
	return true;
}

inline bool stack_types_match(state_stack& a, state_stack& b) {
	if(a.main_size() != b.main_size() || a.return_size() != b.return_size()) {
		return false;
	}
	for(size_t i = a.main_size();  i-- > 0; ) {
		if(a.main_type(i) != b.main_type(i))
			return false;
	}
	for(size_t i = a.return_size();  i-- > 0; ) {
		if(a.return_type(i) != b.return_type(i))
			return false;
	}
	return true;
}

inline int32_t* conditional_jump(state_stack& s, int32_t* p, environment* env) {
	auto value = s.main_data_back(0);
	s.pop_main();
	if(value == 0)
		return p + *(p + 2);
	else
		return p + 3;
}
inline int32_t* reverse_conditional_jump(state_stack& s, int32_t* p, environment* env) {
	auto value = s.main_data_back(0);
	s.pop_main();
	if(value != 0)
		return p + *(p + 2);
	else
		return p + 3;
}
inline int32_t* unconditional_jump(state_stack& s, int32_t* p, environment* env) {
	return p + *(p + 2);
}

class conditional_scope : public locals_holder {
public:
	std::unique_ptr<state_stack> initial_state;
	std::unique_ptr<state_stack> iworking_state;
	std::unique_ptr<state_stack> first_branch_state;
	LLVMValueRef branch_condition = nullptr;
	LLVMBasicBlockRef parent_block = nullptr;
	LLVMBasicBlockRef true_block = nullptr;
	LLVMBasicBlockRef false_block = nullptr;
	bool interpreter_skipped_first_branch = false;
	bool interpreter_skipped_second_branch = false;
	bool typechecking_failed_on_first_branch = false;
	bool initial_typechecking_failed = false;
	size_t bytecode_first_branch_point = 0;
	size_t bytecode_second_branch_point = 0;

	conditional_scope(opaque_compiler_data* p, environment& e, state_stack& entry_state) : locals_holder(p, e) {	
		if(entry_state.main_size() == 0 || entry_state.main_type_back(0) != fif_bool) {
			env.report_error("attempted to start an if without a boolean value on the stack");
			env.mode = fif_mode::error;
		}
		if(env.mode == fif_mode::interpreting) {
			if(entry_state.main_data_back(0) != 0) {
				interpreter_skipped_second_branch = true;
			} else {
				interpreter_skipped_first_branch = true;
				env.mode = fif_mode::typechecking_lvl1_failed;
				entry_state.pop_main();
			}
		}
		if(env.mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
			branch_condition = entry_state.main_ex_back(0);
			if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
				parent_block = *pb;
				*pb = LLVMCreateBasicBlockInContext(env.llvm_context, "if-first-branch");
				LLVMAppendExistingBasicBlock(env.compiler_stack.back()->llvm_function(), *pb);
				LLVMPositionBuilderAtEnd(env.llvm_builder, *pb);
			}
#endif
		}
		if(env.mode == fif_mode::compiling_bytecode) {
			auto bcode = parent->bytecode_compilation_progress();

			fif_call imm = conditional_jump;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			bcode->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			bcode->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			bytecode_first_branch_point = bcode->size();
			bcode->push_back(0);

			fif_call imm2 = enter_scope;
			uint64_t imm2_bytes = 0;
			memcpy(&imm2_bytes, &imm2, 8);
			bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
			bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
		}
		initial_typechecking_failed = typechecking_failed(env.mode);
		if(!typechecking_failed(env.mode))
			entry_state.pop_main();

		initial_state = entry_state.copy();
		initial_state->min_main_depth = entry_state.min_main_depth;
		initial_state->min_return_depth = entry_state.min_return_depth;

		iworking_state = entry_state.copy();
	}
	void commit_first_branch(environment&) {
		if(first_branch_state) {
			env.report_error("attempted to compile multiple else conditions");
			env.mode = fif_mode::error;
			return;
		}

		if(interpreter_skipped_first_branch) {
			env.mode = fif_mode::interpreting;
		} else if(interpreter_skipped_second_branch) {
			env.mode = fif_mode::typechecking_lvl1_failed;
		} else { // not interpreted mode
			first_branch_state = std::move(iworking_state);
			iworking_state = initial_state->copy();
			release_locals();

			if(env.mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
				if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
					true_block = *pb;
					*pb = LLVMCreateBasicBlockInContext(env.llvm_context, "if-else-branch");
					false_block = *pb;
					LLVMAppendExistingBasicBlock(parent->llvm_function(), *pb);
					LLVMPositionBuilderAtEnd(env.llvm_builder, *pb);
				}
#endif
			}
			if(env.mode == fif_mode::compiling_bytecode) {
				auto bcode = parent->bytecode_compilation_progress();
				{
					fif_call imm2 = leave_scope;
					uint64_t imm2_bytes = 0;
					memcpy(&imm2_bytes, &imm2, 8);
					bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
					bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
				}
				fif_call imm = unconditional_jump;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				bcode->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				bcode->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
				bytecode_second_branch_point = bcode->size();
				bcode->push_back(0);
				(*bcode)[bytecode_first_branch_point] = int32_t(bcode->size() - (bytecode_first_branch_point - 2));
				{
					fif_call imm2 = enter_scope;
					uint64_t imm2_bytes = 0;
					memcpy(&imm2_bytes, &imm2, 8);
					bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
					bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
				}
			}
			if(typechecking_failed(env.mode)) {
				typechecking_failed_on_first_branch = true;
				env.mode = reset_typechecking(env.mode);
			}
		}
	}
	virtual control_structure get_type()override {
		return control_structure::str_if;
	}
	virtual state_stack* working_state() override {
		return iworking_state.get();
	}
	virtual void set_working_state(std::unique_ptr<state_stack> p)override {
		iworking_state = std::move(p);
	}
	virtual bool finish(environment&)override {
		if(interpreter_skipped_second_branch) {
			env.mode = fif_mode::interpreting;
		}
		if(env.mode == fif_mode::interpreting) {
			release_locals();
			parent->set_working_state(std::move(iworking_state));
			return true;
		}

		bool final_types_match = true;
		if(first_branch_state) {
			if(!initial_typechecking_failed && !typechecking_failed_on_first_branch && !typechecking_failed(env.mode)) {
				final_types_match = stack_types_match(*first_branch_state, *iworking_state);
			}
		} else {
			if(!typechecking_failed(env.mode))
				final_types_match = stack_types_match(*initial_state , *iworking_state);
		}

		if(!final_types_match) {
			if(typechecking_mode(env.mode)) {
				env.mode = fail_typechecking(env.mode);
				return true;
			} else {
				env.report_error("inconsistent stack types over a conditional branch");
				env.mode = fif_mode::error;
				return true;
			}
		}

		if(typechecking_mode(env.mode)) {
			if(typechecking_failed_on_first_branch && typechecking_failed(env.mode))
				return true; // failed to typecheck on both branches
			if(initial_typechecking_failed) {
				env.mode = fail_typechecking(env.mode);
				return true;
			}
		}

		release_locals();

		if(env.mode == fif_mode::compiling_bytecode) {
			auto bcode = parent->bytecode_compilation_progress();

			{
				fif_call imm2 = leave_scope;
				uint64_t imm2_bytes = 0;
				memcpy(&imm2_bytes, &imm2, 8);
				bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
				bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
			}

			if(!first_branch_state)
				(*bcode)[bytecode_first_branch_point] = int32_t(bcode->size() - (bytecode_first_branch_point - 2));
			else
				(*bcode)[bytecode_second_branch_point] = int32_t(bcode->size() - (bytecode_second_branch_point - 2));
		}

		if(env.mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
			auto pb = env.compiler_stack.back()->llvm_block();
			auto current_block = *pb;

			if(first_branch_state) {

				LLVMPositionBuilderAtEnd(env.llvm_builder, parent_block);
				LLVMBuildCondBr(env.llvm_builder, branch_condition, true_block, false_block);
				auto in_fn = env.compiler_stack.back()->llvm_function();

				auto post_if = LLVMCreateBasicBlockInContext(env.llvm_context, "then-branch");
				*pb = post_if;

				LLVMAppendExistingBasicBlock(in_fn, post_if);

				LLVMPositionBuilderAtEnd(env.llvm_builder, current_block);
				LLVMBuildBr(env.llvm_builder, post_if);
				LLVMPositionBuilderAtEnd(env.llvm_builder, true_block);
				LLVMBuildBr(env.llvm_builder, post_if);

				/*
				* build phi nodes
				*/

				LLVMPositionBuilderAtEnd(env.llvm_builder, post_if);

				for(auto i = iworking_state->main_size(); i-- > 0; ) {
					if(iworking_state->main_ex(i) != first_branch_state->main_ex(i)) {
						auto phi_ref = LLVMBuildPhi(env.llvm_builder, env.dict.type_array[iworking_state->main_type(i)].llvm_type, "if-phi");
						LLVMValueRef inc_vals[2] = { iworking_state->main_ex(i), first_branch_state->main_ex(i) };
						LLVMBasicBlockRef inc_blocks[2] = { current_block, true_block };
						LLVMAddIncoming(phi_ref, inc_vals, inc_blocks, 2);
						iworking_state->set_main_ex(i, phi_ref);
					} else {
						// leave working state as is
					}
				}
				for(auto i = iworking_state->return_size(); i-- > 0; ) {
					if(iworking_state->return_ex(i) != first_branch_state->return_ex(i)) {
						auto phi_ref = LLVMBuildPhi(env.llvm_builder, env.dict.type_array[iworking_state->return_type(i)].llvm_type, "if-phi");
						LLVMValueRef inc_vals[2] = { iworking_state->return_ex(i), first_branch_state->return_ex(i) };
						LLVMBasicBlockRef inc_blocks[2] = { current_block, true_block };
						LLVMAddIncoming(phi_ref, inc_vals, inc_blocks, 2);
						iworking_state->set_return_ex(i, phi_ref);
					} else {
						// leave working state as is
					}
				}
			} else {
				auto in_fn = env.compiler_stack.back()->llvm_function();
				auto post_if = LLVMCreateBasicBlockInContext(env.llvm_context, "then-branch");
				*pb = post_if;

				LLVMAppendExistingBasicBlock(in_fn, post_if);

				LLVMPositionBuilderAtEnd(env.llvm_builder, parent_block);
				LLVMBuildCondBr(env.llvm_builder, branch_condition, current_block, post_if);

				LLVMPositionBuilderAtEnd(env.llvm_builder, current_block);
				LLVMBuildBr(env.llvm_builder, post_if);

				/*
				* build phi nodes
				*/

				LLVMPositionBuilderAtEnd(env.llvm_builder, post_if);

				for(auto i = iworking_state->main_size(); i-- > 0; ) {
					if(iworking_state->main_ex(i) != initial_state->main_ex(i)) {
						auto phi_ref = LLVMBuildPhi(env.llvm_builder, env.dict.type_array[iworking_state->main_type(i)].llvm_type, "if-phi");
						LLVMValueRef inc_vals[2] = { iworking_state->main_ex(i), initial_state->main_ex(i) };
						LLVMBasicBlockRef inc_blocks[2] = { current_block, parent_block };
						LLVMAddIncoming(phi_ref, inc_vals, inc_blocks, 2);
						iworking_state->set_main_ex(i, phi_ref);
					} else {
						// leave working state as is
					}
				}
				for(auto i = iworking_state->return_size(); i-- > 0; ) {
					if(iworking_state->return_ex(i) != initial_state->return_ex(i)) {
						auto phi_ref = LLVMBuildPhi(env.llvm_builder, env.dict.type_array[iworking_state->return_type(i)].llvm_type, "if-phi");
						LLVMValueRef inc_vals[2] = { iworking_state->return_ex(i), initial_state->return_ex(i) };
						LLVMBasicBlockRef inc_blocks[2] = { current_block, parent_block };
						LLVMAddIncoming(phi_ref, inc_vals, inc_blocks, 2);
						iworking_state->set_return_ex(i, phi_ref);
					} else {
						// leave working state as is
					}
				}
			}
#endif
		}

		// branch reconciliation
		if(first_branch_state && !initial_typechecking_failed && !typechecking_failed_on_first_branch && typechecking_failed(env.mode)) {
			// case: second branch failed to typecheck, but first branch passed
			iworking_state = std::move(first_branch_state);
			iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
			iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);

		} else if(first_branch_state && !initial_typechecking_failed && typechecking_failed_on_first_branch &&!typechecking_failed(env.mode)) {
			// case: first branch failed to typecheck, but second branch passed
			iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
			iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);

		} else if(first_branch_state && !initial_typechecking_failed && !typechecking_failed_on_first_branch && !typechecking_failed(env.mode)) {
			// case both branches typechecked
			iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, first_branch_state->min_main_depth);
			iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, first_branch_state->min_return_depth);
			iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
			iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);
		} else if(first_branch_state && !initial_typechecking_failed && typechecking_failed_on_first_branch && typechecking_failed(env.mode)) {
			// case both branches failed to typechecked
			// don't set working state
			return true;

		} else if(!first_branch_state && typechecking_failed(env.mode) && !initial_typechecking_failed) {
			// case: single branch failed to typecheck
			iworking_state = std::move(initial_state);

		} else if(!first_branch_state && !initial_typechecking_failed && !typechecking_failed(env.mode)) {
			// case: single branch typechecked
			iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
			iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);

		} else if(initial_typechecking_failed) {
			// case: typechecking failed going into the conditional
			// dont set working state
			env.mode = fail_typechecking(env.mode);
			return true;

		}

		parent->set_working_state(std::move(iworking_state));
		env.mode = reset_typechecking(env.mode);
		return true;
	}
};

class while_loop_scope : public locals_holder {
public:
	std::unique_ptr<state_stack> initial_state;
	std::unique_ptr<state_stack> iworking_state;
	std::unique_ptr<state_stack> loop_entry_copy;

	std::string_view entry_source;
	size_t loop_entry_point = 0;
	size_t end_of_loop_branch = 0;
	bool phi_pass = false;
	bool intepreter_skip_body = false;
	bool saw_conditional = false;
	bool initial_typechecking_failed = false;
	LLVMValueRef conditional_expr = nullptr;
	LLVMBasicBlockRef pre_block = nullptr;
	LLVMBasicBlockRef entry_block = nullptr;
	LLVMBasicBlockRef body_block = nullptr;
	std::vector<LLVMValueRef> phinodes;

	while_loop_scope(opaque_compiler_data* p, environment& e, state_stack& entry_state) : locals_holder(p, e) {
		initial_state = entry_state.copy();
		iworking_state = entry_state.copy();
		initial_state->min_main_depth = entry_state.min_main_depth;
		initial_state->min_return_depth = entry_state.min_return_depth;
		initial_typechecking_failed = typechecking_failed(env.mode);

		if(!env.source_stack.empty()) {
			entry_source = env.source_stack.back();
		}
		if(env.mode == fif_mode::compiling_bytecode) {
			auto bcode = parent->bytecode_compilation_progress();
			loop_entry_point = bcode->size();

			{
				fif_call imm2 = enter_scope;
				uint64_t imm2_bytes = 0;
				memcpy(&imm2_bytes, &imm2, 8);
				bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
				bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
			}
		}
		if(env.mode == fif_mode::compiling_llvm) {
			phi_pass = true;
			env.mode = fif_mode::typechecking_lvl_2;
			if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
				pre_block = *pb;
			}
		}
	}

	virtual control_structure get_type()override {
		return control_structure::str_while_loop;
	}
	virtual state_stack* working_state() override {
		return iworking_state.get();
	}
	virtual void set_working_state(std::unique_ptr<state_stack> p) override {
		iworking_state = std::move(p);
	}
	void end_condition(environment&) {
		if(saw_conditional) {
			env.report_error("multiple conditions in while loop");
			env.mode = fif_mode::error;
			return;
		}
		saw_conditional = true;

		if(!typechecking_failed(env.mode)) {
			if(iworking_state->main_type_back(0) != fif_bool) {
				if(!typechecking_mode(env.mode)) {
					env.report_error("while loop conditional expression did not produce a boolean");
					env.mode = fif_mode::error;
					return;
				} else {
					env.mode = fail_typechecking(env.mode);
					return;
				}
			}

			release_locals();

			if(env.mode == fif_mode::interpreting) {
				if(iworking_state->main_data_back(0) == 0) {
					intepreter_skip_body = true;
					env.mode = fif_mode::typechecking_lvl1_failed;
				}
				iworking_state->pop_main();
			} else if(env.mode == fif_mode::compiling_bytecode) {
				auto bcode = parent->bytecode_compilation_progress();

				{
					fif_call imm2 = leave_scope;
					uint64_t imm2_bytes = 0;
					memcpy(&imm2_bytes, &imm2, 8);
					bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
					bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
				}

				fif_call imm = conditional_jump;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				bcode->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				bcode->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
				end_of_loop_branch = bcode->size();
				bcode->push_back(0);
				iworking_state->pop_main();

				{
					fif_call imm2 = enter_scope;
					uint64_t imm2_bytes = 0;
					memcpy(&imm2_bytes, &imm2, 8);
					bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
					bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
				}
			} else if(env.mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
				if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
					conditional_expr = iworking_state->main_ex_back(0);
					iworking_state->pop_main();

					*pb = LLVMCreateBasicBlockInContext(env.llvm_context, "loop_body");
					body_block = *pb;
					LLVMAppendExistingBasicBlock(parent->llvm_function(), *pb);
					LLVMPositionBuilderAtEnd(env.llvm_builder, *pb);
				}
#endif
			} else if(typechecking_mode(env.mode)) {
				iworking_state->pop_main();
				if(!stack_types_match(*initial_state, *iworking_state)) {
					env.mode = fail_typechecking(env.mode);
				}
			}
		}
	}
	virtual bool finish(environment&) override {
		if(env.mode == fif_mode::typechecking_lvl1_failed && intepreter_skip_body) {
			env.mode = fif_mode::interpreting;
			
			release_locals();

			parent->set_working_state(std::move(iworking_state));
			return true;
		}

		release_locals();

		if(env.mode == fif_mode::interpreting) {
			if(!saw_conditional && iworking_state->main_type_back(0) == fif_bool) {
				if(iworking_state->main_data_back(0) == 0) {
					iworking_state->pop_main();
					parent->set_working_state(std::move(iworking_state));
					return true;
				}
				iworking_state->pop_main();
			}
			saw_conditional = false;
			if(!env.source_stack.empty())
				env.source_stack.back() = entry_source;
			return false;
		}

		if(env.mode == fif_mode::compiling_bytecode) {
			if(!saw_conditional) {
				if(iworking_state->main_type_back(0) == fif_bool) {
					iworking_state->pop_main();
				} else {
					env.report_error("while loop terminated with an appropriate conditional");
					env.mode = fif_mode::error;
					return true;
				}

				auto bcode = parent->bytecode_compilation_progress();

				{
					fif_call imm2 = leave_scope;
					uint64_t imm2_bytes = 0;
					memcpy(&imm2_bytes, &imm2, 8);
					bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
					bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
				}

				fif_call imm = reverse_conditional_jump;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				bcode->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				bcode->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
				end_of_loop_branch = bcode->size();
				bcode->push_back(int32_t(int64_t(loop_entry_point) - int64_t(bcode->size() - 2)));
			}

			bool final_types_match = stack_types_match(*initial_state, *iworking_state);
			if(!final_types_match) {
				env.report_error("while loop had a net stack effect");
				env.mode = fif_mode::error;
				return true;
			}

			if(saw_conditional) {
				auto bcode = parent->bytecode_compilation_progress();

				{
					fif_call imm2 = leave_scope;
					uint64_t imm2_bytes = 0;
					memcpy(&imm2_bytes, &imm2, 8);
					bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
					bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
				}

				fif_call imm = unconditional_jump;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				bcode->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				bcode->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
				bcode->push_back(int32_t(int64_t(loop_entry_point) - int64_t(bcode->size() - 2)));
				(*bcode)[end_of_loop_branch] = int32_t(bcode->size() - (end_of_loop_branch - 2));;
			}

			iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
			iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);
			parent->set_working_state(std::move(iworking_state));

			return true;
		} else if(env.mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
			if(!saw_conditional) {
				if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
					conditional_expr = iworking_state->main_ex_back(0);
					iworking_state->pop_main();

					*pb = LLVMCreateBasicBlockInContext(env.llvm_context, "loop_body");
					body_block = *pb;
					LLVMAppendExistingBasicBlock(parent->llvm_function(), *pb);
					LLVMPositionBuilderAtEnd(env.llvm_builder, *pb);
				}
			}

			bool final_types_match = stack_types_match(*initial_state, *iworking_state);
			if(!final_types_match) {
				env.report_error("while loop had a net stack effect");
				env.mode = fif_mode::error;
				return true;
			}

			if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
				LLVMBuildBr(env.llvm_builder, entry_block);

				*pb = LLVMCreateBasicBlockInContext(env.llvm_context, "post_loop");
				LLVMAppendExistingBasicBlock(parent->llvm_function(), *pb);

				LLVMPositionBuilderAtEnd(env.llvm_builder, entry_block);
				LLVMBuildCondBr(env.llvm_builder, conditional_expr, body_block, *pb);

				LLVMPositionBuilderAtEnd(env.llvm_builder, *pb);

				/*
				set phi inputs in entry block
				*/
				uint32_t node_index = 0;
				for(auto i = iworking_state->main_size(); i-- > iworking_state->min_main_depth; ) {
					if(phinodes[node_index] == iworking_state->main_ex(i)) { // phi node unchanged
						LLVMValueRef inc_vals[1] = { initial_state->main_ex(i) };
						LLVMBasicBlockRef inc_blocks[1] = { pre_block };
						LLVMAddIncoming(phinodes[node_index], inc_vals, inc_blocks, 1);
					} else { 
						LLVMValueRef inc_vals[2] = { iworking_state->main_ex(i), initial_state->main_ex(i) };
						LLVMBasicBlockRef inc_blocks[2] = { body_block, pre_block };
						LLVMAddIncoming(phinodes[node_index], inc_vals, inc_blocks, 2);
					}
					++node_index;
				}
				for(auto i = iworking_state->return_size(); i-- > iworking_state->min_return_depth; ) {
					if(phinodes[node_index] == iworking_state->return_ex(i)) { // phi node unchanged
						LLVMValueRef inc_vals[1] = { initial_state->return_ex(i) };
						LLVMBasicBlockRef inc_blocks[1] = { pre_block };
						LLVMAddIncoming(phinodes[node_index], inc_vals, inc_blocks, 1);
					} else {
						LLVMValueRef inc_vals[2] = { iworking_state->return_ex(i), initial_state->return_ex(i) };
						LLVMBasicBlockRef inc_blocks[2] = { body_block, pre_block };
						LLVMAddIncoming(phinodes[node_index], inc_vals, inc_blocks, 2);
					}
					++node_index;
				}
			}

			loop_entry_copy->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
			loop_entry_copy->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);
			parent->set_working_state(std::move(loop_entry_copy));

			return true;
#endif
		} else if(env.mode == fif_mode::typechecking_lvl_2 && phi_pass == true) {
#ifdef USE_LLVM
			phi_pass = false;
			if(!saw_conditional) {
				iworking_state->pop_main();
			}


			if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
				*pb = LLVMCreateBasicBlockInContext(env.llvm_context, "while_entry");
				entry_block = *pb;
				LLVMAppendExistingBasicBlock(parent->llvm_function(), *pb);

				LLVMBuildBr(env.llvm_builder, entry_block);
				LLVMPositionBuilderAtEnd(env.llvm_builder, *pb);

				/*
				build entry phi_nodes
				*/
				for(auto i = iworking_state->main_size(); i-- > iworking_state->min_main_depth; ) {
					phinodes.push_back(LLVMBuildPhi(env.llvm_builder, env.dict.type_array[iworking_state->main_type(i)].llvm_type, ""));
					iworking_state->set_main_ex(i, phinodes.back());
				}
				for(auto i = iworking_state->return_size(); i-- > iworking_state->min_return_depth; ) {
					phinodes.push_back(LLVMBuildPhi(env.llvm_builder, env.dict.type_array[iworking_state->return_type(i)].llvm_type, ""));
					iworking_state->set_return_ex(i, phinodes.back());
				}
			}

			loop_entry_copy = iworking_state->copy();
			saw_conditional = false;
			env.mode = fif_mode::compiling_llvm;
			if(!env.source_stack.empty())	
				env.source_stack.back() = entry_source;
#endif
			return false;
		} else if(typechecking_mode(env.mode)) {
			if(!saw_conditional) {
				if(iworking_state->main_type_back(0) != fif_bool) {
					env.mode = reset_typechecking(env.mode);
					if(initial_typechecking_failed || env.mode != fif_mode::typechecking_lvl_1)
						env.mode = fail_typechecking(env.mode);

					return true;
				}
				iworking_state->pop_main();
			}
			bool final_types_match = stack_types_match(*initial_state, *iworking_state);
			if(!final_types_match) {
				env.mode = reset_typechecking(env.mode);
				if(initial_typechecking_failed || env.mode != fif_mode::typechecking_lvl_1)
					env.mode = fail_typechecking(env.mode);

				return true;
			}
			if(!typechecking_failed(env.mode)) {
				iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
				iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);
				parent->set_working_state(std::move(iworking_state));
			} else if(env.mode == fif_mode::typechecking_lvl1_failed) {
				env.mode = reset_typechecking(env.mode);
			}

			return true;
		}
		return true;
	}
};

class do_loop_scope : public locals_holder {
public:
	std::unique_ptr<state_stack> initial_state;
	std::unique_ptr<state_stack> iworking_state;

	std::string_view entry_source;
	size_t loop_entry_point = 0;
	size_t end_of_loop_branch = 0;
	bool phi_pass = false;
	bool initial_typechecking_failed = false;
	
	LLVMBasicBlockRef pre_block = nullptr;
	LLVMBasicBlockRef body_block = nullptr;
	std::vector<LLVMValueRef> phinodes;

	do_loop_scope(opaque_compiler_data* p, environment& e, state_stack& entry_state) : locals_holder(p, e) {
		initial_state = entry_state.copy();
		iworking_state = entry_state.copy();
		initial_state->min_main_depth = entry_state.min_main_depth;
		initial_state->min_return_depth = entry_state.min_return_depth;
		initial_typechecking_failed = typechecking_failed(env.mode);

		if(!env.source_stack.empty()) {
			entry_source = env.source_stack.back();
		}
		if(env.mode == fif_mode::compiling_bytecode) {
			auto bcode = parent->bytecode_compilation_progress();
			loop_entry_point = bcode->size();

			{
				fif_call imm2 = enter_scope;
				uint64_t imm2_bytes = 0;
				memcpy(&imm2_bytes, &imm2, 8);
				bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
				bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
			}
		}
		if(env.mode == fif_mode::compiling_llvm) {
			phi_pass = true;
			env.mode = fif_mode::typechecking_lvl_2;
			if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
				pre_block = *pb;
			}
		}
	}

	virtual control_structure get_type() override {
		return control_structure::str_do_loop;
	}
	virtual state_stack* working_state() override {
		return iworking_state.get();
	}
	virtual void set_working_state(std::unique_ptr<state_stack> p) override {
		iworking_state = std::move(p);
	}
	void at_until(environment&) {
		// don't actually need to do anything here ...
	}
	virtual bool finish(environment&)override {
		release_locals();

		if(env.mode == fif_mode::interpreting) {
			if(iworking_state->main_type_back(0) == fif_bool) {
				if(iworking_state->main_data_back(0) != 0) {
					iworking_state->pop_main();

					parent->set_working_state(std::move(iworking_state));
					return true;
				}
				iworking_state->pop_main();
			}
			if(!env.source_stack.empty())
				env.source_stack.back() = entry_source;
			return false;
		}

		if(env.mode == fif_mode::compiling_bytecode) {
			
			if(iworking_state->main_type_back(0) == fif_bool) {
				iworking_state->pop_main();
			} else {
				env.report_error("do loop not terminated with an appropriate conditional");
				env.mode = fif_mode::error;
				return true;
			}

			auto bcode = parent->bytecode_compilation_progress();
			{
				fif_call imm2 = leave_scope;
				uint64_t imm2_bytes = 0;
				memcpy(&imm2_bytes, &imm2, 8);
				bcode->push_back(int32_t(imm2_bytes & 0xFFFFFFFF));
				bcode->push_back(int32_t((imm2_bytes >> 32) & 0xFFFFFFFF));
			}

			fif_call imm = conditional_jump;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			bcode->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			bcode->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			end_of_loop_branch = bcode->size();
			bcode->push_back(int32_t(int64_t(loop_entry_point) - int64_t(bcode->size() - 2)));
			
			bool final_types_match = stack_types_match(*initial_state, *iworking_state);
			if(!final_types_match) {
				env.report_error("do loop had a net stack effect");
				env.mode = fif_mode::error;
				return true;
			}

			iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
			iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);
			parent->set_working_state(std::move(iworking_state));

			return true;
		} else if(env.mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
			auto conditional_expr = iworking_state->main_ex_back(0);
			iworking_state->pop_main();


			bool final_types_match = stack_types_match(*initial_state, *iworking_state);
			if(!final_types_match) {
				env.report_error("do loop had a net stack effect");
				env.mode = fif_mode::error;
				return true;
			}

			if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
				auto in_block = *pb;
				*pb = LLVMCreateBasicBlockInContext(env.llvm_context, "post_loop");
				LLVMAppendExistingBasicBlock(parent->llvm_function(), *pb);

				LLVMPositionBuilderAtEnd(env.llvm_builder, in_block);
				LLVMBuildCondBr(env.llvm_builder, conditional_expr, *pb, body_block);

				LLVMPositionBuilderAtEnd(env.llvm_builder, *pb);

				/*
				set phi inputs in entry block
				*/
				uint32_t node_index = 0;
				for(auto i = iworking_state->main_size(); i-- > iworking_state->min_main_depth; ) {
					if(phinodes[node_index] == iworking_state->main_ex(i)) { // phi node unchanged
						LLVMValueRef inc_vals[1] = { initial_state->main_ex(i) };
						LLVMBasicBlockRef inc_blocks[1] = { pre_block };
						LLVMAddIncoming(phinodes[node_index], inc_vals, inc_blocks, 1);
					} else {
						LLVMValueRef inc_vals[2] = { iworking_state->main_ex(i), initial_state->main_ex(i) };
						LLVMBasicBlockRef inc_blocks[2] = { in_block, pre_block };
						LLVMAddIncoming(phinodes[node_index], inc_vals, inc_blocks, 2);
					}
					++node_index;
				}
				for(auto i = iworking_state->return_size(); i-- > iworking_state->min_return_depth; ) {
					if(phinodes[node_index] == iworking_state->return_ex(i)) { // phi node unchanged
						LLVMValueRef inc_vals[1] = { initial_state->return_ex(i) };
						LLVMBasicBlockRef inc_blocks[1] = { pre_block };
						LLVMAddIncoming(phinodes[node_index], inc_vals, inc_blocks, 1);
					} else {
						LLVMValueRef inc_vals[2] = { iworking_state->return_ex(i), initial_state->return_ex(i) };
						LLVMBasicBlockRef inc_blocks[2] = { in_block, pre_block };
						LLVMAddIncoming(phinodes[node_index], inc_vals, inc_blocks, 2);
					}
					++node_index;
				}
			}

			iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
			iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);
			parent->set_working_state(std::move(iworking_state));
#endif
			return true;
		} else if(env.mode == fif_mode::typechecking_lvl_2 && phi_pass == true) {
#ifdef USE_LLVM
			phi_pass = false;

			if(iworking_state->main_type_back(0) != fif_bool) {
				env.mode = fif_mode::error;
				env.report_error("do loop did not finish with a boolean value");
				return true;
			}
			iworking_state->pop_main();

			if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
				*pb = LLVMCreateBasicBlockInContext(env.llvm_context, "do_body");
				body_block = *pb;
				LLVMAppendExistingBasicBlock(parent->llvm_function(), *pb);

				LLVMBuildBr(env.llvm_builder, body_block);
				LLVMPositionBuilderAtEnd(env.llvm_builder, *pb);

				/*
				build entry phi_nodes
				*/
				for(auto i = iworking_state->main_size(); i-- > iworking_state->min_main_depth; ) {
					phinodes.push_back(LLVMBuildPhi(env.llvm_builder, env.dict.type_array[iworking_state->main_type(i)].llvm_type, ""));
					iworking_state->set_main_ex(i, phinodes.back());
				}
				for(auto i = iworking_state->return_size(); i-- > iworking_state->min_return_depth; ) {
					phinodes.push_back(LLVMBuildPhi(env.llvm_builder, env.dict.type_array[iworking_state->return_type(i)].llvm_type, ""));
					iworking_state->set_return_ex(i, phinodes.back());
				}
			}

			env.mode = fif_mode::compiling_llvm;
			if(!env.source_stack.empty())
				env.source_stack.back() = entry_source;
#endif
			return false;
		} else if(typechecking_mode(env.mode)) {
			if(iworking_state->main_type_back(0) != fif_bool) {
				env.mode = reset_typechecking(env.mode);
				if(initial_typechecking_failed || env.mode != fif_mode::typechecking_lvl_1)
					env.mode = fail_typechecking(env.mode);
				
				return true;
			}
			iworking_state->pop_main();

			bool final_types_match = stack_types_match(*initial_state, *iworking_state);
			if(!final_types_match) {
				env.mode = reset_typechecking(env.mode);
				if(initial_typechecking_failed || env.mode != fif_mode::typechecking_lvl_1)
					env.mode = fail_typechecking(env.mode);

				parent->set_working_state(std::move(initial_state));

				return true;
			}

			if(!typechecking_failed(env.mode)) {
				iworking_state->min_main_depth = std::min(iworking_state->min_main_depth, initial_state->min_main_depth);
				iworking_state->min_return_depth = std::min(iworking_state->min_return_depth, initial_state->min_return_depth);
				parent->set_working_state(std::move(iworking_state));
			} else if(!initial_typechecking_failed && env.mode == fif_mode::typechecking_lvl_1) {
				env.mode = reset_typechecking(env.mode);
			}

			return true;
		}

		return true;
	}
};

inline constexpr size_t inlining_cells_limit = 512;

inline uint32_t codepoint_from_utf8(char const* start, char const* end) {
	uint8_t byte1 = uint8_t(start + 0 < end ? start[0] : 0);
	uint8_t byte2 = uint8_t(start + 1 < end ? start[1] : 0);
	uint8_t byte3 = uint8_t(start + 2 < end ? start[2] : 0);
	uint8_t byte4 = uint8_t(start + 3 < end ? start[3] : 0);
	if((byte1 & 0x80) == 0) {
		return uint32_t(byte1);
	} else if((byte1 & 0xE0) == 0xC0) {
		return uint32_t(byte2 & 0x3F) | (uint32_t(byte1 & 0x1F) << 6);
	} else  if((byte1 & 0xF0) == 0xE0) {
		return uint32_t(byte3 & 0x3F) | (uint32_t(byte2 & 0x3F) << 6) | (uint32_t(byte1 & 0x0F) << 12);
	} else if((byte1 & 0xF8) == 0xF0) {
		return uint32_t(byte4 & 0x3F) | (uint32_t(byte3 & 0x3F) << 6) | (uint32_t(byte2 & 0x3F) << 12) | (uint32_t(byte1 & 0x07) << 18);
	}
	return 0;
}
inline size_t size_from_utf8(char const* start) {
	uint8_t b = uint8_t(start[0]);
	return ((b & 0x80) == 0) ? 1 : ((b & 0xE0) == 0xC0) ? 2
		: ((b & 0xF0) == 0xE0) ? 3 : ((b & 0xF8) == 0xF0) ? 4
		: 1;
}
inline bool codepoint_is_space(uint32_t c) noexcept {
	return (c == 0x3000 || c == 0x205F || c == 0x202F || c == 0x2029 || c == 0x2028 || c == 0x00A0
		|| c == 0x0085 || c <= 0x0020 || (0x2000 <= c && c <= 0x200A));
}
inline bool codepoint_is_line_break(uint32_t c) noexcept {
	return  c == 0x2029 || c == 0x2028 || c == uint32_t('\n') || c == uint32_t('\r');
}

struct string_start {
	int32_t eq_match = 0;
	bool starts_string = false;
};

inline string_start match_string_start(std::string_view source) {
	if(source.length() < 2)
		return string_start{ 0, false };
	if(source[0] != '<')
		return string_start{ 0, false };
	int32_t eq_count = 0;
	uint32_t pos = 1;
	while(pos < source.length()) {
		if(source[pos] == '<') {
			return string_start{ eq_count, true };
		} else if(source[pos] == '=') {
			++eq_count;
		} else {
			break;
		}
		++pos;
	}
	return string_start{ 0, false };
}
inline bool match_string_end(std::string_view source, int32_t eq_count_in) {
	if(source.length() < 2)
		return false ;
	if(source[0] != '>')
		return  false;
	int32_t eq_count = 0;
	uint32_t pos = 1;
	while(pos < source.length()) {
		if(source[pos] == '>') {
			return eq_count == eq_count_in;
		} else if(source[pos] == '=') {
			++eq_count;
		} else {
			break;
		}
		++pos;
	}
	return false;
}

// string format: <<xxxxx>> or <=<xxxxx>=> or <==<xxxxx>==> etc

inline parse_result read_token(std::string_view& source, environment& env) {
	size_t first_non_space = 0;
	while(first_non_space < source.length()) {
		auto codepoint = codepoint_from_utf8(source.data() + first_non_space, source.data() + source.length());
		if(codepoint_is_space(codepoint) || codepoint_is_line_break(codepoint)) 
			first_non_space += size_from_utf8(source.data() + first_non_space);
		 else
			break;
	}

	if(auto sm = match_string_start(source.substr(first_non_space)); sm.starts_string) {
		auto str_start = first_non_space + sm.eq_match + 2;
		auto str_pos = str_start;
		while(str_pos < source.length()) {
			if(match_string_end(source.substr(str_pos), sm.eq_match)) {
				auto result = source.substr(str_start, str_pos - str_start);
				source = source.substr(str_pos + sm.eq_match + 2);
				return parse_result{ result, true };
			}
			str_pos += size_from_utf8(source.data() + str_pos);
		}

		// invalid text, reached the end of source within a string without closing it
		env.report_error("unclosed string constant");
		env.mode = fif_mode::error;
		source = std::string_view{ };
		return parse_result{ std::string_view{ }, false };
	}

	auto word_end = first_non_space;
	while(word_end < source.length()) {
		auto codepoint = codepoint_from_utf8(source.data() + word_end, source.data() + source.length());
		if(codepoint_is_space(codepoint) || codepoint_is_line_break(codepoint))
			 break;
		else
			word_end += size_from_utf8(source.data() + word_end);
	}

	auto result = source.substr(first_non_space, word_end - first_non_space);
	source = source.substr(word_end);

	return parse_result{ result, false };

}

inline void execute_fif_word(parse_result word, environment& env, bool ignore_specialization);


inline void run_to_scope_end(environment& env) { // execute/compile source code until current function is completed
	parse_result word;
	auto scope_depth = env.compiler_stack.size();

	if(!env.compiler_stack.empty()) {
		do {
			if(!env.source_stack.empty())
				word = read_token(env.source_stack.back(), env);
			else
				word.content = std::string_view{ };

			if(word.content.length() > 0 && env.mode != fif_mode::error) {
				execute_fif_word(word, env, false);
			}
		} while(word.content.length() > 0 && !env.compiler_stack.empty() && env.mode != fif_mode::error);
	}
}

inline void run_to_function_end(environment& env) { // execute/compile source code until current function is completed
	run_to_scope_end(env);
	if(!env.compiler_stack.empty() && env.compiler_stack.back()->finish(env)) {
		env.compiler_stack.pop_back();
	}
}

inline word_match_result get_basic_type_match(int32_t word_index, state_stack& current_type_state, environment& env, std::vector<int32_t>& specialize_t_subs, bool ignore_specializations) {
	while(word_index != -1) {
		specialize_t_subs.clear();
		bool specialization_matches = [&]() { 
			if(env.dict.word_array[word_index].stack_types_count == 0)
				return true;
			return match_stack_description(std::span<int32_t const>{env.dict.all_stack_types.data() + env.dict.word_array[word_index].stack_types_start, size_t(env.dict.word_array[word_index].stack_types_count)}, current_type_state, env, specialize_t_subs).matched;
		}();
		if(specialization_matches && (ignore_specializations == false || env.dict.word_array[word_index].treat_as_base || env.dict.word_array[word_index].specialization_of == -1)) {
			word_match_result match = match_word(env.dict.word_array[word_index], current_type_state, env.dict.all_instances, env.dict.all_stack_types, env);
			int32_t w = word_index;
			match.substitution_version = word_index;

			if(!match.matched) {
				if(typechecking_failed(env.mode)) {
					// ignore match failure and pass on through
					return word_match_result{ false, 0, 0, 0, 0 };
				} else if(env.mode == fif_mode::typechecking_lvl_1 || env.mode == fif_mode::typechecking_lvl_2) {
					if(env.dict.word_array[w].being_typechecked) { // already being typechecked -- mark as un checkable recursive branch
						env.mode = fail_typechecking(env.mode);
					} else if(env.dict.word_array[w].source.length() > 0) { // try to typecheck word as level 1
						env.dict.word_array[w].being_typechecked = true;

						switch_compiler_stack_mode(env, fif_mode::typechecking_lvl_1);

						env.source_stack.push_back(std::string_view(env.dict.word_array[w].source));
						auto fnscope = std::make_unique<function_scope>(env.compiler_stack.back().get(), env, current_type_state, w, -1);
						fnscope->type_subs = specialize_t_subs;
						env.compiler_stack.emplace_back(std::move(fnscope));

						run_to_function_end(env);
						env.source_stack.pop_back();

						env.dict.word_array[w].being_typechecked = false;
						match = match_word(env.dict.word_array[w], current_type_state, env.dict.all_instances, env.dict.all_stack_types, env);
						match.substitution_version = word_index;

						restore_compiler_stack_mode(env);

						if(match.matched) {
							std::get<interpreted_word_instance>(env.dict.all_instances[match.word_index]).typechecking_level = 1;
							return match;
						} else {
							env.mode = fail_typechecking(env.mode);
							return word_match_result{ false, 0, 0, 0, 0 };
						}
					}
				} else if(env.dict.word_array[w].source.length() > 0) { // either compiling or interpreting or level 3 typecheck-- switch to typechecking to get a type definition
					switch_compiler_stack_mode(env, fif_mode::typechecking_lvl_1);
					env.dict.word_array[w].being_typechecked = true;

					env.source_stack.push_back(std::string_view(env.dict.word_array[w].source));
					auto fnscope = std::make_unique<function_scope>(env.compiler_stack.back().get(), env, current_type_state, w, -1);
					fnscope->type_subs = specialize_t_subs;
					env.compiler_stack.emplace_back(std::move(fnscope));

					run_to_function_end(env);
					env.source_stack.pop_back();

					env.dict.word_array[w].being_typechecked = false;

					restore_compiler_stack_mode(env);

					match = match_word(env.dict.word_array[w], current_type_state, env.dict.all_instances, env.dict.all_stack_types, env);
					match.substitution_version = word_index;

					if(!match.matched) {
						env.report_error("typechecking failure for " + word_name_from_id(w, env));
						env.mode = fif_mode::error;
						return word_match_result{ false, 0, 0, 0, 0 };
					}
					return match;
				}
			}

			return match;
		} else {
			word_index = env.dict.word_array[word_index].specialization_of;
		}
	}
	
	return word_match_result{ false, 0, 0, 0, 0 };
}

inline bool fully_typecheck_word(int32_t w, int32_t word_index, interpreted_word_instance& wi, state_stack& current_type_state, environment& env, std::vector<int32_t>& tsubs) {
	if(wi.typechecking_level == 1) {
		// perform level 2 typechecking

		switch_compiler_stack_mode(env, fif_mode::typechecking_lvl_2);

		env.source_stack.push_back(std::string_view(env.dict.word_array[w].source));
		auto fnscope = std::make_unique<function_scope>(env.compiler_stack.back().get(), env, current_type_state, w, word_index);
		fnscope->type_subs = tsubs;
		env.compiler_stack.emplace_back(std::move(fnscope));

		run_to_function_end(env);
		env.source_stack.pop_back();

		bool failed = typechecking_failed(env.mode);

		restore_compiler_stack_mode(env);

		if(failed) {
			env.report_error("level 2 typecheck failed");
			env.mode = fif_mode::error;
			return false;
		}

		wi.typechecking_level = 2;
	}

	if(wi.typechecking_level == 2) {
		// perform level 3 typechecking

		env.compiler_stack.emplace_back(std::make_unique< typecheck3_record_holder>(env.compiler_stack.back().get(), env));
		auto record_holder = static_cast<typecheck3_record_holder*>(env.compiler_stack.back().get());

		switch_compiler_stack_mode(env, fif_mode::typechecking_lvl_3);
		env.source_stack.push_back(std::string_view(env.dict.word_array[w].source));
		auto fnscope = std::make_unique<function_scope>(env.compiler_stack.back().get(), env, current_type_state, w, word_index);
		fnscope->type_subs = tsubs;
		env.compiler_stack.emplace_back(std::move(fnscope));

		run_to_function_end(env);
		env.source_stack.pop_back();

		bool failed = typechecking_failed(env.mode);

		restore_compiler_stack_mode(env);

		if(failed) {
			env.report_error("level 3 typecheck failed");
			env.mode = fif_mode::error;
			return false;
		}

		wi.typechecking_level = 3;

		if(auto erb = record_holder->tr.find((uint64_t(w) << 32) | uint64_t(word_index)); erb != record_holder->tr.end()) {
			typecheck_3_record r;
			r.stack_height_added_at = int32_t(current_type_state.main_size());
			r.rstack_height_added_at = int32_t(current_type_state.return_size());

			auto c = get_stack_consumption(w, word_index, env);
			r.stack_consumed = std::max(c.stack, erb->second.stack_consumed);
			r.rstack_consumed = std::max(c.rstack, erb->second.rstack_consumed);

			record_holder->tr.insert_or_assign((uint64_t(w) << 32) | uint64_t(word_index), r);
			} else {
				typecheck_3_record r;
				r.stack_height_added_at = int32_t(current_type_state.main_size());
				r.rstack_height_added_at = int32_t(current_type_state.return_size());

				auto c = get_stack_consumption(w, word_index, env);
				r.stack_consumed = c.stack;
				r.rstack_consumed = c.rstack;

				record_holder->tr.insert_or_assign((uint64_t(w) << 32) | uint64_t(word_index), r);
		}

		/*
		process results of typechecking
		*/
		auto min_stack_depth = int32_t(current_type_state.main_size());
		auto min_rstack_depth = int32_t(current_type_state.return_size());
		for(auto& s : record_holder->tr) {
			min_stack_depth = std::min(min_stack_depth, s.second.stack_height_added_at - s.second.stack_consumed);
			min_rstack_depth = std::min(min_rstack_depth, s.second.rstack_height_added_at - s.second.rstack_consumed);
		}

		auto existing_description = std::span<int32_t const>(env.dict.all_stack_types.data() + wi.stack_types_start, wi.stack_types_count);
		auto revised_description = expand_stack_description(current_type_state, existing_description, int32_t(current_type_state.main_size()) - min_stack_depth, int32_t(current_type_state.return_size()) - min_rstack_depth);

		if(!compare_stack_description(existing_description, std::span<int32_t const>(revised_description.data(), revised_description.size()))) {
			wi.stack_types_start = int32_t(env.dict.all_stack_types.size());
			wi.stack_types_count = int32_t(revised_description.size());
			env.dict.all_stack_types.insert(env.dict.all_stack_types.end(), revised_description.begin(), revised_description.end());
		}

		env.compiler_stack.pop_back(); // for typecheck3_record_holder
	}

	return true;
}

inline bool compile_word(int32_t w, int32_t word_index, state_stack& state, fif_mode compile_mode, environment& env, std::vector<int32_t>& tsubs) {
	if(!std::holds_alternative<interpreted_word_instance>(env.dict.all_instances[word_index]))
		return true;

	interpreted_word_instance& wi = std::get<interpreted_word_instance>(env.dict.all_instances[word_index]);
	if(!fully_typecheck_word(w, word_index, wi, state, env, tsubs)) {
		return false;
	}

	switch_compiler_stack_mode(env, compile_mode);

	if(env.mode != fif_mode::compiling_llvm && wi.compiled_bytecode.size() == 0) {
		// typed but uncompiled word
		if(!std::get<interpreted_word_instance>(env.dict.all_instances[word_index]).being_compiled) {
			env.source_stack.push_back(std::string_view(env.dict.word_array[w].source));
			auto fnscope = std::make_unique<function_scope>(env.compiler_stack.back().get(), env, state, w, word_index);
			fnscope->type_subs = tsubs;
			env.compiler_stack.emplace_back(std::move(fnscope));
			run_to_function_end(env);
			env.source_stack.pop_back();
		}
	}

	// case: reached an uncompiled llvm definition
	if(env.mode == fif_mode::compiling_llvm && !wi.llvm_compilation_finished) {
#ifdef USE_LLVM
		// typed but uncompiled word
		if(!std::get<interpreted_word_instance>(env.dict.all_instances[word_index]).being_compiled) {
			LLVMBasicBlockRef stored_block = nullptr;
			if(auto pb = env.compiler_stack.back()->llvm_block(); pb)
				stored_block = *pb;

			
			env.source_stack.push_back(std::string_view(env.dict.word_array[w].source));
			auto fnscope = std::make_unique<function_scope>(env.compiler_stack.back().get(), env, state, w, word_index);
			fnscope->type_subs = tsubs;
			env.compiler_stack.emplace_back(std::move(fnscope));
			run_to_function_end(env);
			env.source_stack.pop_back();

			if(auto pb = env.compiler_stack.back()->llvm_block(); pb) {
				*pb = stored_block;
				LLVMPositionBuilderAtEnd(env.llvm_builder, stored_block);
			}
			
		}
#endif
	}

	restore_compiler_stack_mode(env);
	return true;
}

inline let_data* get_global_let(environment& env, std::string const& name) {
	for(auto& p : env.compiler_stack) {
		if(p->get_type() == control_structure::globals)
			return static_cast<compiler_globals_layer*>(p.get())->get_global_let(name);
	}
	return nullptr;
}
inline var_data* get_global_var(environment& env, std::string const& name) {
	for(auto& p : env.compiler_stack) {
		if(p->get_type() == control_structure::globals)
			return static_cast<compiler_globals_layer*>(p.get())->get_global_var(name);
	}
	return nullptr;
}

inline int32_t* immediate_local(state_stack& s, int32_t* p, environment* e) {
	char* name = 0;
	memcpy(&name, p + 2, 8);

	var_data* l = e->compiler_stack.back()->get_var(std::string(name));
	if(!l) {
		e->report_error("unable to find local var");
		return nullptr;
	}
	int32_t type = 0;
	memcpy(&type, p + 4, 4);
	s.push_back_main(type, (int64_t)l, nullptr);
	return p + 5;
}

inline int32_t* immediate_let(state_stack& s, int32_t* p, environment* e) {
	char* name = 0;
	memcpy(&name, p + 2, 8);
	let_data* l = e->compiler_stack.back()->get_let(std::string(name));
	if(!l) {
		e->report_error("unable to find local let");
		return nullptr;
	}
	int32_t type = 0;
	memcpy(&type, p + 4, 4);
	s.push_back_main(type, l->data, nullptr);
	return p + 5;
}

inline int32_t* immediate_global(state_stack& s, int32_t* p, environment* e) {
	char* name = 0;
	memcpy(&name, p + 2, 8);
	var_data* v = get_global_var(*e, std::string(name));
	if(!v) {
		e->report_error("unable to find local let");
		return nullptr;
	}
	int32_t type = 0;
	memcpy(&type, p + 4, 4);
	s.push_back_main(type, (int64_t)v, nullptr);
	return p + 5;
}

inline void execute_fif_word(parse_result word, environment& env, bool ignore_specializations) {
	auto ws = env.compiler_stack.back()->working_state();
	if(!ws) {
		env.report_error("tried to execute in a scope with no working state");
		env.mode = fif_mode::error;
		return;
	}

	// TODO: string constant case
	// TODO: restore constructor word
	//if(word.content.length() > 0 && word.content[0] == '@') {
	//	do_construct_type(*ws, resolve_type(word.content.substr(1), env), &env);
	//} else 
	auto content_string = std::string{ word.content };
	if(is_integer(word.content.data(), word.content.data() + word.content.length())) {
		do_immediate_i32(*ws, parse_int(word.content), &env);
	} else if(is_fp(word.content.data(), word.content.data() + word.content.length())) {
		do_immediate_f32(*ws, parse_float(word.content), &env);
	} else if(word.content == "true" || word.content == "false") {
		do_immediate_bool(*ws, word.content == "true", &env);
	} else if(auto it = env.dict.words.find(std::string(word.content)); it != env.dict.words.end()) {
		auto w = it->second;
		// execute / compile word

		// IMMEDIATE words with source
		if(env.dict.word_array[w].immediate) {
			if(env.dict.word_array[w].source.length() > 0) {
				env.source_stack.push_back(std::string_view{ env.dict.word_array[w].source });
				switch_compiler_stack_mode(env, fif_mode::interpreting);

				run_to_scope_end(env);

				restore_compiler_stack_mode(env);
				env.source_stack.pop_back(); // remove source replacement

				return;
			}
		}

		std::vector<int32_t> called_tsub_types;
		auto match = get_basic_type_match(w, *ws, env, called_tsub_types, ignore_specializations);
		w = match.substitution_version;

		if(!match.matched) {
			if(typechecking_failed(env.mode)) {
				return;
			} else { // critical failure
				env.report_error("could not match word to stack types");
				env.mode = fif_mode::error;
				return;
			}
		}

		word_types* wi = &(env.dict.all_instances[match.word_index]);

		// IMMEDIATE words (source not available)
		if(env.dict.word_array[w].immediate) {
			if(std::holds_alternative<interpreted_word_instance>(*wi)) {
				switch_compiler_stack_mode(env, fif_mode::interpreting);
				execute_fif_word(std::get<interpreted_word_instance>(*wi), *ws, env);
				restore_compiler_stack_mode(env);
			} else if(std::holds_alternative<compiled_word_instance>(*wi)) {
				execute_fif_word(std::get<compiled_word_instance>(*wi), *ws, env);
			}
			return;
		}

		//
		// level 1 typechecking -- should be able to trace at least one path through each word using only known words
		//
		// level 2 typechecking -- should be able to trace through every branch to produce an accurate stack picture for all known words present in the definition
		//

		if(env.mode == fif_mode::typechecking_lvl_1 || env.mode == fif_mode::typechecking_lvl1_failed
			|| env.mode == fif_mode::typechecking_lvl_2 || env.mode == fif_mode::typechecking_lvl2_failed) {

			if(std::holds_alternative<interpreted_word_instance>(*wi)) {
				if(!typechecking_failed(env.mode)) {
					apply_stack_description(std::span<int32_t const>(env.dict.all_stack_types.data() + std::get<interpreted_word_instance>(*wi).stack_types_start, size_t(std::get<interpreted_word_instance>(*wi).stack_types_count)), *ws, env);
				}
			} else if(std::holds_alternative<compiled_word_instance>(*wi)) {
				// no special logic, compiled words assumed to always typecheck
				if(!typechecking_failed(env.mode)) {
					apply_stack_description(std::span<int32_t const>(env.dict.all_stack_types.data() + std::get<compiled_word_instance>(*wi).stack_types_start, size_t(std::get<compiled_word_instance>(*wi).stack_types_count)), *ws, env);
				}
			}
			return;
		}

		//
		// level 3 typechecking -- recursively determine the minimum stack position of all dependencies
		//

		if(env.mode == fif_mode::typechecking_lvl_3 || env.mode == fif_mode::typechecking_lvl3_failed) {
			if(std::holds_alternative<interpreted_word_instance>(*wi)) {
				if(!typechecking_failed(env.mode)) {
					// add typecheck info
					if(std::get<interpreted_word_instance>(*wi).typechecking_level < 3) {
						// this word also hasn't been compiled yet

						auto rword = env.compiler_stack.back()->word_id();
						auto rinst = env.compiler_stack.back()->instance_id();
						auto& dep = *(env.compiler_stack.back()->typecheck_record());
						auto existing_record = dep.find((uint64_t(w) << 32) | uint64_t(match.word_index));
						if(existing_record != dep.end()
							&& existing_record->second.stack_height_added_at <= int32_t(ws->main_size())
							&& existing_record->second.rstack_height_added_at <= int32_t(ws->return_size())) {

							// already added all dependencies of this word at this stack height or less
						} else { // word is occurring deeper in the stack or not yet seen
							// recurse and typecheck through it
							// first, make sure that it is typechecked to at least level 2

							if(env.dict.word_array[w].source.length() == 0) {
								env.report_error(std::string("Word ") + std::to_string(w) + " is undefined.");
								env.mode = fif_mode::error;
								return;
							}

							if(std::get<interpreted_word_instance>(*wi).typechecking_level < 2) {
								switch_compiler_stack_mode(env, fif_mode::typechecking_lvl_2);

								env.source_stack.push_back(std::string_view(env.dict.word_array[w].source));
								auto fnscope = std::make_unique<function_scope>(env.compiler_stack.back().get(), env, *ws, w, match.word_index);
								fnscope->type_subs = called_tsub_types;
								env.compiler_stack.emplace_back(std::move(fnscope));

								run_to_function_end(env);
								env.source_stack.pop_back();

								bool failed = typechecking_failed(env.mode);
								restore_compiler_stack_mode(env);

								if(failed) {
									env.report_error("level 2 typecheck failed");
									env.mode = fif_mode::error;
									return;
								}

								wi = &(env.dict.all_instances[match.word_index]);
								std::get<interpreted_word_instance>(*wi).typechecking_level = 2;
							}

							env.mode = fif_mode::typechecking_lvl_3;
							if(auto erb = dep.find((uint64_t(w) << 32) | uint64_t(match.word_index)); erb != dep.end()) {
								typecheck_3_record r;
								r.stack_height_added_at = int32_t(ws->main_size());
								r.rstack_height_added_at = int32_t(ws->return_size());

								auto c = get_stack_consumption(w, match.word_index, env);
								r.stack_consumed = std::max(c.stack, erb->second.stack_consumed);
								r.rstack_consumed = std::max(c.rstack, erb->second.rstack_consumed);

								dep.insert_or_assign((uint64_t(w) << 32) | uint64_t(match.word_index), r);
							} else {
								typecheck_3_record r;
								r.stack_height_added_at = int32_t(ws->main_size());
								r.rstack_height_added_at = int32_t(ws->return_size());

								auto c = get_stack_consumption(w, match.word_index, env);
								r.stack_consumed = c.stack;
								r.rstack_consumed = c.rstack;

								dep.insert_or_assign((uint64_t(w) << 32) | uint64_t(match.word_index), r);
							}
						}
					}

					apply_stack_description(std::span<int32_t const>(env.dict.all_stack_types.data() + std::get<interpreted_word_instance>(*wi).stack_types_start, size_t(std::get<interpreted_word_instance>(*wi).stack_types_count)), *ws, env);
				}
			} else if(std::holds_alternative<compiled_word_instance>(*wi)) {
				if(!typechecking_failed(env.mode)) {
					// no special logic, compiled words assumed to always typecheck
					apply_stack_description(std::span<int32_t const>(env.dict.all_stack_types.data() + std::get<compiled_word_instance>(*wi).stack_types_start, size_t(std::get<compiled_word_instance>(*wi).stack_types_count)), *ws, env);
				}
			}

			return;
		}

		if(env.mode == fif_mode::interpreting || env.mode == fif_mode::compiling_llvm || env.mode == fif_mode::compiling_bytecode) {
			if(!compile_word(w, match.word_index, *ws, env.mode != fif_mode::compiling_llvm ? fif_mode::compiling_bytecode : fif_mode::compiling_llvm, env, called_tsub_types)) {
				env.report_error("failed to compile word");
				env.mode = fif_mode::error;
				return;
			}
			wi = &(env.dict.all_instances[match.word_index]);

			if(std::holds_alternative<interpreted_word_instance>(*wi)) {
				if(env.mode == fif_mode::interpreting) {
					env.compiler_stack.push_back(std::make_unique<runtime_function_scope>(env.compiler_stack.back().get(), env));
					execute_fif_word(std::get<interpreted_word_instance>(*wi), *ws, env);
					env.compiler_stack.back()->finish(env);
					env.compiler_stack.pop_back();
				} else if(env.mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
					std::span<int32_t const> desc{ env.dict.all_stack_types.data() + std::get<interpreted_word_instance>(*wi).stack_types_start, size_t(std::get<interpreted_word_instance>(*wi).stack_types_count) };
					llvm_make_function_call(env, std::get<interpreted_word_instance>(*wi).llvm_function, desc);
#endif
				} else if(env.mode == fif_mode::compiling_bytecode) {
					auto cbytes = env.compiler_stack.back()->bytecode_compilation_progress();
					if(cbytes) {
						if(std::get<interpreted_word_instance>(*wi).being_compiled) {
							fif_call imm = call_function_indirect;
							uint64_t imm_bytes = 0;
							memcpy(&imm_bytes, &imm, 8);
							cbytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
							cbytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
							cbytes->push_back(match.word_index);
						} else {
							{
								fif_call imm = call_function;
								uint64_t imm_bytes = 0;
								memcpy(&imm_bytes, &imm, 8);
								cbytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
								cbytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
							}
							{
								int32_t* bcode = std::get<interpreted_word_instance>(*wi).compiled_bytecode.data();
								uint64_t imm_bytes = 0;
								memcpy(&imm_bytes, &bcode, 8);
								cbytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
								cbytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
							}
						}
						apply_stack_description(std::span<int32_t const>(env.dict.all_stack_types.data() + std::get<interpreted_word_instance>(*wi).stack_types_start, size_t(std::get<interpreted_word_instance>(*wi).stack_types_count)), *ws, env);
					}
				}
			} else if(std::holds_alternative<compiled_word_instance>(*wi)) {
				if(env.mode == fif_mode::compiling_bytecode) {
					auto cbytes = env.compiler_stack.back()->bytecode_compilation_progress();
					if(cbytes) {
						fif_call imm = std::get<compiled_word_instance>(*wi).implementation;
						uint64_t imm_bytes = 0;
						memcpy(&imm_bytes, &imm, 8);
						cbytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
						cbytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));

						apply_stack_description(std::span<int32_t const>(env.dict.all_stack_types.data() + std::get<compiled_word_instance>(*wi).stack_types_start, size_t(std::get<compiled_word_instance>(*wi).stack_types_count)), *ws, env);
					}
				} else {
					std::get<compiled_word_instance>(*wi).implementation(*ws, nullptr, &env);
				}
			}
		}
	} else if(auto let = env.compiler_stack.back()->get_let(content_string); word.is_string == false && let) {
		if(typechecking_mode(env.mode)) {
			ws->push_back_main(let->type, let->data, let->expression);
		} else if(env.mode == fif_mode::compiling_llvm) {
			ws->push_back_main(let->type, let->data, let->expression);
		} else if(env.mode == fif_mode::interpreting) {
			ws->push_back_main(let->type, let->data, let->expression);
		} else if(env.mode == fif_mode::compiling_bytecode) {
			// implement let
			auto cbytes = env.compiler_stack.back()->bytecode_compilation_progress();
			if(cbytes) {
				fif_call imm = immediate_let;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				cbytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				cbytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));

				auto string_constant = env.get_string_constant(word.content);
				char const* cptr = string_constant.data();

				uint64_t let_addr = 0;
				memcpy(&let_addr, &cptr, 8);
				cbytes->push_back(int32_t(let_addr & 0xFFFFFFFF));
				cbytes->push_back(int32_t((let_addr >> 32) & 0xFFFFFFFF));

				cbytes->push_back(let->type);
			}
			ws->push_back_main(let->type, 0, 0);
		}
	} else if(auto var = env.compiler_stack.back()->get_var(content_string); word.is_string == false && var) {
		int32_t ptr_type[] = { fif_ptr, std::numeric_limits<int32_t>::max(), var->type, -1 };
		std::vector<int32_t> subs;
		auto mem_type = resolve_span_type(std::span<int32_t const>(ptr_type, ptr_type + 4), subs, env);

		if(typechecking_mode(env.mode)) {
			ws->push_back_main(mem_type.type, 0, 0);
		} else if(env.mode == fif_mode::compiling_llvm) {
			ws->push_back_main(mem_type.type, 0, var->alloc);
		} else if(env.mode == fif_mode::interpreting) {
			ws->push_back_main(mem_type.type, (int64_t)(var), 0);
		} else if(env.mode == fif_mode::compiling_bytecode) {
			// implement let
			auto cbytes = env.compiler_stack.back()->bytecode_compilation_progress();
			if(cbytes) {
				fif_call imm = immediate_local;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				cbytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				cbytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));

				auto string_constant = env.get_string_constant(word.content);
				char const* cptr = string_constant.data();
				uint64_t let_addr = 0;
				memcpy(&let_addr, &cptr, 8);
				cbytes->push_back(int32_t(let_addr & 0xFFFFFFFF));
				cbytes->push_back(int32_t((let_addr >> 32) & 0xFFFFFFFF));

				cbytes->push_back(mem_type.type);
			}
			ws->push_back_main(mem_type.type, 0, 0);
		}
	} else if(auto letb = get_global_let(env, content_string); word.is_string == false && letb) {

	} else if(auto varb = get_global_var(env, content_string); word.is_string == false && varb) {
		int32_t ptr_type[] = { fif_ptr, std::numeric_limits<int32_t>::max(), varb->type, -1 };
		std::vector<int32_t> subs;
		auto mem_type = resolve_span_type(std::span<int32_t const>(ptr_type, ptr_type + 4), subs, env);

		if(typechecking_mode(env.mode)) {
			ws->push_back_main(mem_type.type, 0, 0);
		} else if(env.mode == fif_mode::compiling_llvm) {
			ws->push_back_main(mem_type.type, 0, varb->alloc);
		} else if(env.mode == fif_mode::interpreting) {
			ws->push_back_main(mem_type.type, (int64_t)(varb), 0);
		} else if(env.mode == fif_mode::compiling_bytecode) {
			// implement let
			auto cbytes = env.compiler_stack.back()->bytecode_compilation_progress();
			if(cbytes) {
				fif_call imm = immediate_global;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				cbytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				cbytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));

				auto string_constant = env.get_string_constant(word.content);
				char const* cptr = string_constant.data();
				uint64_t let_addr = 0;
				memcpy(&let_addr, &cptr, 8);
				cbytes->push_back(int32_t(let_addr & 0xFFFFFFFF));
				cbytes->push_back(int32_t((let_addr >> 32) & 0xFFFFFFFF));

				cbytes->push_back(mem_type.type);
			}
			ws->push_back_main(mem_type.type, 0, 0);
		}
	} else if(auto rtype = resolve_type(word.content, env, env.compiler_stack.back()->type_substitutions()); rtype != -1) {
		do_immediate_type(*ws, rtype, &env);
	} else {
		env.report_error(std::string("attempted to execute an unknown word: ") + std::string(word.content));
		env.mode = fif_mode::error;
	}
}

#ifdef USE_LLVM
inline void add_exportable_functions_to_globals(environment& env) {
	if(!env.exported_functions.empty()) {
		auto array_type = LLVMArrayType(LLVMPointerTypeInContext(env.llvm_context, 0), 1);
		auto used_array = LLVMAddGlobal(env.llvm_module, array_type, "llvm.used");
		LLVMSetLinkage(used_array, LLVMLinkage::LLVMAppendingLinkage);
		LLVMSetInitializer(used_array, LLVMConstArray(LLVMPointerTypeInContext(env.llvm_context, 0), env.exported_functions.data(), uint32_t(env.exported_functions.size())));
		LLVMSetSection(used_array, "llvm.metadata");
	}
}

inline LLVMValueRef make_exportable_function(std::string const& export_name, std::string const& word, std::vector<int32_t> param_stack, std::vector<int32_t> return_stack, environment& env) {

	env.compiler_stack.emplace_back(std::make_unique<outer_interpreter>(env));
	env.source_stack.push_back(std::string_view{ });
	outer_interpreter* o = static_cast<outer_interpreter*>(env.compiler_stack.back().get());
	switch_compiler_stack_mode(env, fif_mode::interpreting);
	static_cast<mode_switch_scope*>(env.compiler_stack.back().get())->interpreted_link = o;

	int32_t w = -1;
	if(auto it = env.dict.words.find(word); it != env.dict.words.end()) {
		w = it->second;
	} else {
		env.report_error("failed to export function (dictionary lookup failed)");
		return nullptr;
	}

	llvm_stack ts;
	ts.resize(param_stack.size(), return_stack.size());
	for(auto i = param_stack.size(); i-- > 0; ) {
		ts.set_main_type(i, param_stack[i]);
	}
	for(auto i = return_stack.size(); i-- > 0; ) {
		ts.set_return_type(i, return_stack[i]);
	}
	ts.min_main_depth = param_stack.size();
	ts.min_return_depth = param_stack.size();

	std::vector<int32_t> typevars;
	auto match = get_basic_type_match(w, ts, env, typevars, false);
	w = match.substitution_version;
	if(!match.matched) {
		env.report_error("failed to export function (typematch failed)");
		return nullptr;
	}

	if(!compile_word(w, match.word_index, ts, fif_mode::compiling_llvm, env, typevars)) {
		env.report_error("failed to export function (compilation failed)");
		return nullptr;
	}

	if(!std::holds_alternative<interpreted_word_instance>(env.dict.all_instances[match.word_index])) {
		env.report_error("failed to export function (can't export built-in)");
		return nullptr;
	}

	auto& wi = std::get<interpreted_word_instance>(env.dict.all_instances[match.word_index]);

	auto desc = std::span<int32_t const>(env.dict.all_stack_types.data() + wi.stack_types_start, wi.stack_types_count);
	auto fn_type = llvm_function_type_from_desc(env, desc);
	auto compiled_fn = LLVMAddFunction(env.llvm_module, export_name.c_str(), fn_type);

	LLVMSetFunctionCallConv(compiled_fn, NATIVE_CC);
	//LLVMSetLinkage(compiled_fn, LLVMLinkage::LLVMLinkOnceAnyLinkage);
	//LLVMSetVisibility(compiled_fn, LLVMVisibility::LLVMDefaultVisibility);

	auto entry_block = LLVMAppendBasicBlockInContext(env.llvm_context, compiled_fn, "fn_entry_point");
	LLVMPositionBuilderAtEnd(env.llvm_builder, entry_block);


	// add body
	std::vector<LLVMValueRef> params;

	int32_t match_position = 0;
	// stack matching

	int32_t consumed_stack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		params.push_back(LLVMGetParam(compiled_fn, uint32_t(consumed_stack_cells)));
		++match_position;
		++consumed_stack_cells;
	}

	++match_position; // skip -1

	// output stack
	int32_t first_output_stack = match_position;
	//int32_t output_stack_types = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		//returns_group.push_back(env.dict.type_array[desc[match_position]].llvm_type.llvm_type);
		++match_position;
		//++output_stack_types;
	}
	++match_position; // skip -1

	// return stack matching
	int32_t consumed_rstack_cells = 0;
	while(match_position < int32_t(desc.size()) && desc[match_position] != -1) {
		params.push_back(LLVMGetParam(compiled_fn, uint32_t(consumed_stack_cells + consumed_rstack_cells)));
		++match_position;
		++consumed_rstack_cells;
	}

	auto retvalue = LLVMBuildCall2(env.llvm_builder, llvm_function_type_from_desc(env, desc), wi.llvm_function, params.data(), uint32_t(params.size()), "");
	LLVMSetInstructionCallConv(retvalue, LLVMCallConv::LLVMFastCallConv);
	auto rsummary = llvm_function_return_type_from_desc(env, desc);

	// make return
	if(rsummary.composite_type == nullptr) {
		LLVMBuildRetVoid(env.llvm_builder);
	} else {
		LLVMBuildRet(env.llvm_builder, retvalue);
	}
	if(LLVMVerifyFunction(compiled_fn, LLVMVerifierFailureAction::LLVMPrintMessageAction))
		std::abort();

	env.exported_functions.push_back(compiled_fn);

	restore_compiler_stack_mode(env);
	env.compiler_stack.pop_back();
	env.source_stack.pop_back();

	return compiled_fn;
}
#endif

inline void run_fif_interpreter(environment& env, std::string on_text) {
	env.source_stack.push_back(std::string_view(on_text));
	env.compiler_stack.emplace_back(std::make_unique<outer_interpreter>(env));
	outer_interpreter* o = static_cast<outer_interpreter*>(env.compiler_stack.back().get());

	switch_compiler_stack_mode(env, fif_mode::interpreting);
	mode_switch_scope* m = static_cast<mode_switch_scope*>(env.compiler_stack.back().get());
	m->interpreted_link = o;

	run_to_scope_end(env);
	env.source_stack.pop_back();
	restore_compiler_stack_mode(env);
	env.compiler_stack.pop_back();
}

inline void run_fif_interpreter(environment& env, std::string on_text, interpreter_stack& s) {
	env.source_stack.push_back(std::string_view(on_text));
	env.compiler_stack.emplace_back(std::make_unique<outer_interpreter>(env));
	outer_interpreter* o = static_cast<outer_interpreter*>(env.compiler_stack.back().get());
	static_cast<interpreter_stack*>(o->interpreter_state.get())->move_into(std::move(s));
	
	switch_compiler_stack_mode(env, fif_mode::interpreting);
	mode_switch_scope* m = static_cast<mode_switch_scope*>(env.compiler_stack.back().get());
	m->interpreted_link = o;

	run_to_scope_end(env);
	env.source_stack.pop_back();
	restore_compiler_stack_mode(env);

	s.move_into(std::move(*(o->interpreter_state)));
	env.compiler_stack.pop_back();
}

#ifdef USE_LLVM
inline LLVMErrorRef module_transform(void* Ctx, LLVMModuleRef Mod) {
	LLVMPassBuilderOptionsRef pass_opts = LLVMCreatePassBuilderOptions();
	LLVMPassBuilderOptionsSetLoopInterleaving(pass_opts, true);
	LLVMPassBuilderOptionsSetLoopVectorization(pass_opts, true);
	LLVMPassBuilderOptionsSetSLPVectorization(pass_opts, true);
	LLVMPassBuilderOptionsSetLoopUnrolling(pass_opts, true);
	LLVMPassBuilderOptionsSetForgetAllSCEVInLoopUnroll(pass_opts, false);
	LLVMPassBuilderOptionsSetMergeFunctions(pass_opts, true);
	LLVMErrorRef E = LLVMRunPasses(Mod, "default<O3>", nullptr, pass_opts);
	//LLVMErrorRef E = LLVMRunPasses(Mod, "instcombine", nullptr, pass_opts);
	LLVMDisposePassBuilderOptions(pass_opts);
	if(E) {
		auto msg = LLVMGetErrorMessage(E);
		//std::cout << msg << std::endl;
		LLVMDisposeErrorMessage(msg);
		std::abort();
	}
	return E;
}

inline LLVMErrorRef perform_transform(void* Ctx, LLVMOrcThreadSafeModuleRef* the_module, LLVMOrcMaterializationResponsibilityRef MR) {
	return LLVMOrcThreadSafeModuleWithModuleDo(*the_module, module_transform, Ctx);
}
#endif
inline void add_import(std::string_view name, void* ptr, fif_call interpreter_implementation, std::vector<int32_t> const& params, std::vector<int32_t> const& returns, environment& env) {
	env.imported_functions.push_back(import_item{ std::string(name), ptr });

	std::vector<int32_t> itype_list;
	for(auto j = params.size(); j-->0;) {
		itype_list.push_back(params[j]);
	}
	auto pcount = itype_list.size();
	if(!returns.empty()) {
		itype_list.push_back(-1);
		for(auto r : returns) {
			itype_list.push_back(r);
		}
	}

	int32_t start_types = int32_t(env.dict.all_stack_types.size());
	env.dict.all_stack_types.insert(env.dict.all_stack_types.end(), itype_list.begin(), itype_list.end());
	int32_t count_types = int32_t(itype_list.size());

	auto nstr = std::string(name);
	int32_t old_word = -1;
	if(auto it = env.dict.words.find(nstr); it != env.dict.words.end()) {
		old_word = it->second;
	}

	env.dict.words.insert_or_assign(nstr, int32_t(env.dict.word_array.size()));
	env.dict.word_array.emplace_back();
	env.dict.word_array.back().specialization_of = old_word;
	env.dict.word_array.back().stack_types_start = start_types;
	env.dict.word_array.back().stack_types_count = int32_t(pcount);

	auto instance_num = int32_t(env.dict.all_instances.size());
	env.dict.word_array.back().instances.push_back(instance_num);

	interpreted_word_instance wi;
	{
		fif_call imm = interpreter_implementation;
		uint64_t imm_bytes = 0;
		memcpy(&imm_bytes, &imm, 8);
		wi.compiled_bytecode.push_back(int32_t(imm_bytes & 0xFFFFFFFF));
		wi.compiled_bytecode.push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
	}
	{
		fif_call imm = function_return;
		uint64_t imm_bytes = 0;
		memcpy(&imm_bytes, &imm, 8);
		wi.compiled_bytecode.push_back(int32_t(imm_bytes & 0xFFFFFFFF));
		wi.compiled_bytecode.push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
	}
	wi.llvm_compilation_finished = true;
	wi.stack_types_start = start_types;
	wi.stack_types_count = count_types;
	wi.typechecking_level = 3;
	wi.is_imported_function = true;

#ifdef USE_LLVM
	auto fn_desc = std::span<int32_t const>(itype_list.begin(), itype_list.end());
	auto fn_type = llvm_function_type_from_desc(env, fn_desc);
	wi.llvm_function = LLVMAddFunction(env.llvm_module, nstr.c_str(), fn_type);
	LLVMSetFunctionCallConv(wi.llvm_function, NATIVE_CC);
	LLVMSetLinkage(wi.llvm_function, LLVMLinkage::LLVMExternalLinkage);
#endif

	env.dict.all_instances.emplace_back(std::move(wi));
}

#ifdef USE_LLVM
inline void perform_jit(environment& e) {
	//add_exportable_functions_to_globals(e);

	char* out_message = nullptr;
	auto result = LLVMVerifyModule(e.llvm_module, LLVMVerifierFailureAction::LLVMPrintMessageAction, &out_message);
	if(result) {
		e.report_error(out_message);
		return;
	}
	if(out_message)
		LLVMDisposeMessage(out_message);
	
	LLVMDisposeBuilder(e.llvm_builder);
	e.llvm_builder = nullptr;

	// ORC JIT
	auto jit_builder = LLVMOrcCreateLLJITBuilder();
	assert(jit_builder);

	auto target_machine = LLVMOrcJITTargetMachineBuilderCreateFromTargetMachine(e.llvm_target_machine);
	e.llvm_target_machine = nullptr;
	LLVMOrcLLJITBuilderSetJITTargetMachineBuilder(jit_builder, target_machine);

	auto errora = LLVMOrcCreateLLJIT(&e.llvm_jit, jit_builder);
	if(errora) {
		auto msg = LLVMGetErrorMessage(errora);
		e.report_error(msg);
		LLVMDisposeErrorMessage(msg);
		return;
	}

	if(!e.llvm_jit) {
		e.report_error("failed to create jit");
		return;
	}


	LLVMOrcIRTransformLayerRef TL = LLVMOrcLLJITGetIRTransformLayer(e.llvm_jit);
	LLVMOrcIRTransformLayerSetTransform(TL, *perform_transform, nullptr);

	LLVMOrcThreadSafeModuleRef orc_mod = LLVMOrcCreateNewThreadSafeModule(e.llvm_module, e.llvm_ts_context);
	e.llvm_module = nullptr;

	LLVMOrcExecutionSessionRef execution_session = LLVMOrcLLJITGetExecutionSession(e.llvm_jit);
	LLVMOrcJITDylibRef main_dyn_lib = LLVMOrcLLJITGetMainJITDylib(e.llvm_jit);
	if(!main_dyn_lib) {
		e.report_error("failed to get main dylib");
		std::abort();
	}

	// add imports
	std::vector< LLVMOrcCSymbolMapPair> import_symbols;
	for(auto& i : e.imported_functions) {
		auto name = LLVMOrcLLJITMangleAndIntern(e.llvm_jit, i.name.c_str());
		LLVMJITEvaluatedSymbol sym;
		sym.Address = (LLVMOrcExecutorAddress)i.ptr;
		sym.Flags.GenericFlags = LLVMJITSymbolGenericFlags::LLVMJITSymbolGenericFlagsCallable;
		sym.Flags.TargetFlags = 0;
		import_symbols.push_back(LLVMOrcCSymbolMapPair{ name, sym });
	}

	if(import_symbols.size() > 0) {
		auto import_mr = LLVMOrcAbsoluteSymbols(import_symbols.data(), import_symbols.size());
		auto import_result = LLVMOrcJITDylibDefine(main_dyn_lib, import_mr);
		if(import_result) {
			auto msg = LLVMGetErrorMessage(import_result);
			e.report_error(msg);
			LLVMDisposeErrorMessage(msg);
			return;
		}
	}

	auto error = LLVMOrcLLJITAddLLVMIRModule(e.llvm_jit, main_dyn_lib, orc_mod);
	if(error) {
		auto msg = LLVMGetErrorMessage(error);
		e.report_error(msg);
		LLVMDisposeErrorMessage(msg);
		return;
	}
}
#endif

inline int32_t* colon_definition(fif::state_stack&, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode))
		return p + 2;
	if(e->mode != fif::fif_mode::interpreting) {
		e->report_error("attempted to compile a definition inside a definition");
		e->mode = fif::fif_mode::error;
		return p + 2;
	}
	
	if(e->source_stack.empty()) {
		e->report_error("attempted to compile a definition without a source");
		e->mode = fif::fif_mode::error;
		return p + 2;
	}

	auto name_token = fif::read_token(e->source_stack.back(), *e);

	auto string_start = e->source_stack.back().data();
	while(e->source_stack.back().length() > 0) {
		auto t = fif::read_token(e->source_stack.back(), *e);
		if(t.is_string == false && t.content == ";") { // end of definition
			auto string_end = t.content.data();

			auto nstr = std::string(name_token.content);
			if(e->dict.words.find(nstr) != e->dict.words.end()) {
				e->report_error("illegal word redefinition");
				e->mode = fif_mode::error;
				return p + 2;
			}

			e->dict.words.insert_or_assign(nstr, int32_t(e->dict.word_array.size()));
			e->dict.word_array.emplace_back();
			e->dict.word_array.back().source = std::string(string_start, string_end);
			
			return p + 2;
		}
	}

	e->report_error("reached the end of a definition source without a ; terminator");
	e->mode = fif::fif_mode::error;
	return p + 2;
}

inline int32_t* colon_specialization(fif::state_stack&, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode))
		return p + 2;
	if(e->mode != fif::fif_mode::interpreting) {
		e->report_error("attempted to compile a definition inside a definition");
		e->mode = fif::fif_mode::error;
		return p + 2;
	}

	if(e->source_stack.empty()) {
		e->report_error("attempted to compile a definition without a source");
		e->mode = fif::fif_mode::error;
		return p + 2;
	}

	auto name_token = fif::read_token(e->source_stack.back(), *e);

	std::vector<std::string_view> stack_types;
	while(true) {
		auto next_token = fif::read_token(e->source_stack.back(), *e);
		if(next_token.content.length() == 0 || next_token.content == "s:") {
			break;
		}
		stack_types.push_back(next_token.content);
	}
	
	std::vector<int32_t> acc_types;
	while(!stack_types.empty()) {
		auto result = internal_generate_type(stack_types.back(), *e);
		if(result.type_array.empty()) {
			e->mode = fif_mode::error;
			e->report_error("unable to resolve type from text");
			return nullptr;
		}
		acc_types.insert(acc_types.end(), result.type_array.begin(), result.type_array.end());
		stack_types.pop_back();
	}
	int32_t start_types = int32_t(e->dict.all_stack_types.size());
	e->dict.all_stack_types.insert(e->dict.all_stack_types.end(), acc_types.begin(), acc_types.end());
	int32_t count_types = int32_t(acc_types.size());

	auto string_start = e->source_stack.back().data();
	while(e->source_stack.back().length() > 0) {
		auto t = fif::read_token(e->source_stack.back(), *e);
		if(t.is_string == false && t.content == ";") { // end of definition
			auto string_end = t.content.data();

			auto nstr = std::string(name_token.content);
			int32_t old_word = -1;
			if(auto it = e->dict.words.find(nstr); it != e->dict.words.end()) {
				old_word = it->second;
			}

			e->dict.words.insert_or_assign(nstr, int32_t(e->dict.word_array.size()));
			e->dict.word_array.emplace_back();
			e->dict.word_array.back().source = std::string(string_start, string_end);
			e->dict.word_array.back().specialization_of = old_word;
			e->dict.word_array.back().stack_types_start = start_types;
			e->dict.word_array.back().stack_types_count = count_types;

			return p + 2;
		}
	}

	e->report_error("reached the end of a definition source without a ; terminator");
	e->mode = fif::fif_mode::error;
	return p + 2;
}

inline int32_t* iadd(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto add_result = LLVMBuildAdd(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, b + a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* f32_add(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFAdd(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f32, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &a, 4);
		memcpy(&fb, &b, 4);
		fa = fa + fb;
		memcpy(&a, &fa, 4);
		s.push_back_main(fif::fif_f32, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64_add(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFAdd(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f64, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &a, 8);
		memcpy(&fb, &b, 8);
		fa = fa + fb;
		memcpy(&a, &fa, 8);
		s.push_back_main(fif::fif_f64, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* isub(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto add_result = LLVMBuildSub(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, b - a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* f32_sub(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFSub(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f32, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &a, 4);
		memcpy(&fb, &b, 4);
		fa = fa - fb;
		memcpy(&a, &fa, 4);
		s.push_back_main(fif::fif_f32, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64_sub(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFSub(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f64, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &a, 8);
		memcpy(&fb, &b, 8);
		fa = fa - fb;
		memcpy(&a, &fa, 8);
		s.push_back_main(fif::fif_f64, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* imul(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto add_result = LLVMBuildMul(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, b * a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* f32_mul(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFMul(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f32, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &a, 4);
		memcpy(&fb, &b, 4);
		fa =  fb * fa;
		memcpy(&a, &fa, 4);
		s.push_back_main(fif::fif_f32, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64_mul(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFMul(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f64, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &a, 8);
		memcpy(&fb, &b, 8);
		fa = fb * fa;
		memcpy(&a, &fa, 8);
		s.push_back_main(fif::fif_f64, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sidiv(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto add_result = LLVMBuildSDiv(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, b / a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* uidiv(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto add_result = LLVMBuildUDiv(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = uint64_t(s.main_data_back(0));
		auto b = uint64_t(s.main_data_back(1));
		s.pop_main();
		s.set_main_data_back(0, b / a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* f32_div(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFDiv(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f32, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &a, 4);
		memcpy(&fb, &b, 4);
		fa = fb / fa;
		memcpy(&a, &fa, 4);
		s.push_back_main(fif::fif_f32, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64_div(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFDiv(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f64, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &a, 8);
		memcpy(&fb, &b, 8);
		fa = fb / fa;
		memcpy(&a, &fa, 8);
		s.push_back_main(fif::fif_f64, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* simod(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto add_result = LLVMBuildSRem(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, b % a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* uimod(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto add_result = LLVMBuildURem(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = uint64_t(s.main_data_back(0));
		auto b = uint64_t(s.main_data_back(1));
		s.pop_main();
		s.set_main_data_back(0, b % a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* f32_mod(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFRem(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f32, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &a, 4);
		memcpy(&fb, &b, 4);
		fa = fmodf(fb, fa);
		memcpy(&a, &fa, 4);
		s.push_back_main(fif::fif_f32, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64_mod(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto add_result = LLVMBuildFDiv(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_f64, 0, add_result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &a, 8);
		memcpy(&fb, &b, 8);
		fa = fmod(fb, fa);
		memcpy(&a, &fa, 8);
		s.push_back_main(fif::fif_f64, a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_f64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* dup(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		s.mark_used_from_main(1);
		auto type = s.main_type_back(0);
		auto expr = s.main_ex_back(0);
		s.push_back_main(type, 0, expr);
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		auto type = s.main_type_back(0);
		auto dat = s.main_data_back(0);
		s.push_back_main(type, dat, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
		auto type = s.main_type_back(0);
		s.push_back_main(type, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* init(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		s.mark_used_from_main(1);
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* copy(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		s.mark_used_from_main(1);
		auto type = s.main_type_back(0);
		auto expr = s.main_ex_back(0);
		s.push_back_main(type, 0, expr);
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		auto type = s.main_type_back(0);
		auto dat = s.main_data_back(0);
		s.push_back_main(type, dat, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
		auto type = s.main_type_back(0);
		s.push_back_main(type, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* drop(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		auto type = s.main_type_back(0);
		s.pop_main();
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto type = s.main_type_back(0);
		s.pop_main();
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* fif_swap(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		auto type_a = s.main_type_back(0);
		auto type_b = s.main_type_back(1);
		auto expr_a = s.main_ex_back(0);
		auto expr_b = s.main_ex_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(type_a, 0, expr_a);
		s.push_back_main(type_b, 0, expr_b);
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto type_a = s.main_type_back(0);
		auto type_b = s.main_type_back(1);
		auto dat_a = s.main_data_back(0);
		auto dat_b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(type_a, dat_a, nullptr);
		s.push_back_main(type_b, dat_b, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		auto type_a = s.main_type_back(0);
		auto type_b = s.main_type_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(type_a, 0, nullptr);
		s.push_back_main(type_b, 0, nullptr);
	}
	return p + 2;
}


inline int32_t* fif_if(fif::state_stack& s, int32_t* p, fif::environment* e) {
	e->compiler_stack.emplace_back(std::make_unique<fif::conditional_scope>(e->compiler_stack.back().get(), *e, s));
	return p + 2;
}
inline int32_t* fif_else(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->compiler_stack.empty() || e->compiler_stack.back()->get_type() != fif::control_structure::str_if) {
		e->report_error("invalid use of else");
		e->mode = fif::fif_mode::error;
	} else {
		fif::conditional_scope* c = static_cast<fif::conditional_scope*>(e->compiler_stack.back().get());
		c->commit_first_branch(*e);
	}
	return p + 2;
}
inline int32_t* fif_then(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->compiler_stack.empty() || e->compiler_stack.back()->get_type() != fif::control_structure::str_if) {
		e->report_error("invalid use of then/end-if");
		e->mode = fif::fif_mode::error;
	} else {
		if(e->compiler_stack.back()->finish(*e))
			e->compiler_stack.pop_back();
	}
	return p + 2;
}

inline int32_t* fif_while(fif::state_stack& s, int32_t* p, fif::environment* e) {
	e->compiler_stack.emplace_back(std::make_unique<fif::while_loop_scope>(e->compiler_stack.back().get(), *e, s));
	return p + 2;
}
inline int32_t* fif_loop(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->compiler_stack.empty() || e->compiler_stack.back()->get_type() != fif::control_structure::str_while_loop) {
		e->report_error("invalid use of loop");
		e->mode = fif::fif_mode::error;
	} else {
		fif::while_loop_scope* c = static_cast<fif::while_loop_scope*>(e->compiler_stack.back().get());
		c->end_condition(*e);
	}
	return p + 2;
}
inline int32_t* fif_end_while(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->compiler_stack.empty() || e->compiler_stack.back()->get_type() != fif::control_structure::str_while_loop) {
		e->report_error("invalid use of end-while");
		e->mode = fif::fif_mode::error;
	} else {
		if(e->compiler_stack.back()->finish(*e))
			e->compiler_stack.pop_back();
	}
	return p + 2;
}
inline int32_t* fif_do(fif::state_stack& s, int32_t* p, fif::environment* e) {
	e->compiler_stack.emplace_back(std::make_unique<fif::do_loop_scope>(e->compiler_stack.back().get(), *e, s));
	return p + 2;
}
inline int32_t* fif_until(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->compiler_stack.empty() || e->compiler_stack.back()->get_type() != fif::control_structure::str_do_loop) {
		e->report_error("invalid use of until");
		e->mode = fif::fif_mode::error;
	} else {
		fif::do_loop_scope* c = static_cast<fif::do_loop_scope*>(e->compiler_stack.back().get());
		c->at_until(*e);
	}
	return p + 2;
}
inline int32_t* fif_end_do(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->compiler_stack.empty() || e->compiler_stack.back()->get_type() != fif::control_structure::str_do_loop) {
		e->report_error("invalid use of end-do");
		e->mode = fif::fif_mode::error;
	} else {
		if(e->compiler_stack.back()->finish(*e))
			e->compiler_stack.pop_back();
	}
	return p + 2;
}
inline int32_t* from_r(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		auto type_a = s.return_type_back(0);
		auto expr_a = s.return_ex_back(0);
		s.push_back_main(type_a, 0, expr_a);
		s.pop_return();
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto type_a = s.return_type_back(0);
		auto dat_a = s.return_data_back(0);
		s.push_back_main(type_a, dat_a, nullptr);
		s.pop_return();
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		auto type_a = s.return_type_back(0);
		s.push_back_main(type_a, 0, nullptr);
		s.pop_return();
	}
	return p + 2;
}
inline int32_t* to_r(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		auto type_a = s.main_type_back(0);
		auto expr_a = s.main_ex_back(0);
		s.push_back_return(type_a, 0, expr_a);
		s.pop_main();
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto type_a = s.main_type_back(0);
		auto dat_a = s.main_data_back(0);
		s.push_back_return(type_a, dat_a, nullptr);
		s.pop_main();
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		auto type_a = s.main_type_back(0);
		s.push_back_return(type_a, 0, nullptr);
		s.pop_main();
	}
	return p + 2;
}


inline void add_precompiled(fif::environment& env, std::string name, fif::fif_call fn, std::vector<int32_t> types, bool immediate = false) {
	if(auto it = env.dict.words.find(name); it != env.dict.words.end()) {
		env.dict.word_array[it->second].instances.push_back(int32_t(env.dict.all_instances.size()));
		env.dict.word_array[it->second].immediate |= immediate;
	} else {
		env.dict.words.insert_or_assign(name, int32_t(env.dict.word_array.size()));
		env.dict.word_array.emplace_back();
		env.dict.word_array.back().instances.push_back(int32_t(env.dict.all_instances.size()));
		env.dict.word_array.back().immediate = immediate;
	}
	env.dict.all_instances.push_back(fif::compiled_word_instance{ });
	auto& w = std::get< fif::compiled_word_instance>(env.dict.all_instances.back());
	w.implementation = fn;
	w.stack_types_count = int32_t(types.size());
	w.stack_types_start = int32_t(env.dict.all_stack_types.size());
	env.dict.all_stack_types.insert(env.dict.all_stack_types.end(), types.begin(), types.end());
}

inline int32_t* ilt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntSLT, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b < a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32lt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOLT, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &ia, 4);
		memcpy(&fb, &ib, 4);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb < fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64lt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOLT, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &ia, 8);
		memcpy(&fb, &ib, 8);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb < fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* uilt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntULT, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = uint64_t(s.main_data_back(0));
		auto b = uint64_t(s.main_data_back(1));
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b < a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* igt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntSGT, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b > a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* uigt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntUGT, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = uint64_t(s.main_data_back(0));
		auto b = uint64_t(s.main_data_back(1));
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b > a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32gt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOGT, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &ia, 4);
		memcpy(&fb, &ib, 4);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb > fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64gt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOGT, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &ia, 8);
		memcpy(&fb, &ib, 8);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb > fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* ile(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntSLE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b <= a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* uile(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntULE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = uint64_t(s.main_data_back(0));
		auto b = uint64_t(s.main_data_back(1));
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b <= a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32le(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOLE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &ia, 4);
		memcpy(&fb, &ib, 4);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb <= fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64le(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOLE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &ia, 8);
		memcpy(&fb, &ib, 8);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb <= fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* ige(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntSGE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b >= a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* uige(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntUGE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = uint64_t(s.main_data_back(0));
		auto b = uint64_t(s.main_data_back(1));
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b >= a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32ge(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOGE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &ia, 4);
		memcpy(&fb, &ib, 4);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb >= fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64ge(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOGE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &ia, 8);
		memcpy(&fb, &ib, 8);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb >= fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* ieq(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntEQ, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b == a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32eq(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOEQ, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &ia, 4);
		memcpy(&fb, &ib, 4);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb == fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64eq(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealOEQ, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &ia, 8);
		memcpy(&fb, &ib, 8);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb == fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* ine(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildICmp(e->llvm_builder, LLVMIntPredicate::LLVMIntNE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, b != a, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32ne(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealONE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		float fa = 0;
		float fb = 0;
		memcpy(&fa, &ia, 4);
		memcpy(&fb, &ib, 4);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb != fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64ne(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFCmp(e->llvm_builder, LLVMRealPredicate::LLVMRealONE, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ia = s.main_data_back(0);
		auto ib = s.main_data_back(1);
		double fa = 0;
		double fb = 0;
		memcpy(&fa, &ia, 8);
		memcpy(&fb, &ib, 8);
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, fb != fa, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}

inline int32_t* f_select(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(3);
		auto result = LLVMBuildSelect(e->llvm_builder, s.main_ex_back(0), s.main_ex_back(1), s.main_ex_back(2), "");

		if(e->dict.type_array[s.main_type_back(1)].flags != 0) {
			auto drop_result = LLVMBuildSelect(e->llvm_builder, s.main_ex_back(0), s.main_ex_back(2), s.main_ex_back(1), "");
			s.push_back_main(s.main_type_back(1), 0, drop_result);
			execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
		}

		s.pop_main();
		s.pop_main();
		s.set_main_ex_back(0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(3);
		auto ib = s.main_data_back(2);
		auto ia = s.main_data_back(1);
		auto ex = s.main_data_back(0);

		if(e->dict.type_array[s.main_type_back(1)].flags != 0) {
			s.push_back_main(s.main_type_back(1), ex != 0 ? ib : ia, nullptr);
			execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
		}

		s.pop_main();
		s.pop_main();
		s.set_main_data_back(0, ex != 0 ? ia : ib);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(3);
		s.pop_main();
		s.pop_main();
	}
	return p + 2;
}

inline int32_t* make_immediate(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		e->report_error("cannot turn a word immediate in compiled code");
		e->mode = fif_mode::error;
		return nullptr;
	} else if(e->mode == fif::fif_mode::interpreting) {
		if(e->dict.word_array.back().specialization_of == -1) {
			e->dict.word_array.back().immediate = true;
		} else {
			e->report_error("cannot mark a specialized word as immediate");
			e->mode = fif_mode::error;
			return nullptr;
		}
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		
	}
	return p + 2;
}

inline int32_t* open_bracket(fif::state_stack& s, int32_t* p, fif::environment* e) {
	switch_compiler_stack_mode(*e, fif_mode::interpreting);
	return p + 2;
}
inline int32_t* close_bracket(fif::state_stack& s, int32_t* p, fif::environment* e) {
	restore_compiler_stack_mode(*e);
	return p + 2;
}

inline int32_t* impl_heap_allot(fif::state_stack& s, int32_t* p, fif::environment* e) { // must drop contents ?
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto val = s.main_ex_back(0);
		auto new_mem = LLVMBuildMalloc(e->llvm_builder, e->dict.type_array[s.main_type_back(0)].llvm_type, "");
		LLVMBuildStore(e->llvm_builder, val, new_mem);
		int32_t ptr_type[] = { fif_ptr, std::numeric_limits<int32_t>::max(), s.main_type_back(0), -1 };
		std::vector<int32_t> subs;
		auto mem_type = resolve_span_type(std::span<int32_t const>(ptr_type, ptr_type + 4), subs, *e);
		s.pop_main();
		s.push_back_main(mem_type.type, 0, new_mem);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto ptr = malloc(8);
		int64_t dat = s.main_data_back(0);
		memcpy(ptr, &dat, 8);

		int32_t ptr_type[] = { fif_ptr, std::numeric_limits<int32_t>::max(), s.main_type_back(0), -1 };
		std::vector<int32_t> subs;
		auto mem_type = resolve_span_type(std::span<int32_t const>(ptr_type, ptr_type + 4), subs, *e);
		s.pop_main();
		s.push_back_main(mem_type.type, (int64_t)ptr, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		int32_t ptr_type[] = { fif_ptr, std::numeric_limits<int32_t>::max(), s.main_type_back(0), -1 };
		std::vector<int32_t> subs;
		auto mem_type = resolve_span_type(std::span<int32_t const>(ptr_type, ptr_type + 4), subs, *e);
		s.pop_main();
		s.push_back_main(mem_type.type, 0, nullptr);
	}
	return p + 2;
}

inline int32_t* impl_heap_free(fif::state_stack& s, int32_t* p, fif::environment* e) { // must drop contents ?
	if(fif::typechecking_failed(e->mode))
		return p + 2;

	auto ptr_type = s.main_type_back(0);
	auto decomp = e->dict.type_array[ptr_type].decomposed_types_start;
	auto pointer_contents = e->dict.all_stack_types[decomp + 1];

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto iresult = LLVMBuildLoad2(e->llvm_builder, e->dict.type_array[pointer_contents].llvm_type, s.main_ex_back(0), "");
		LLVMBuildFree(e->llvm_builder, s.main_ex_back(0));
		s.pop_main();
		s.push_back_main(pointer_contents, 0, iresult);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {

		auto a = s.main_data_back(0);
		int64_t* ptr = (int64_t*)a;
		auto val = *ptr;
		free(ptr);
		s.pop_main();
		s.push_back_main(pointer_contents, val, 0);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(pointer_contents, 0, 0);
	}
	return p + 2;
}

inline int32_t* impl_load(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_failed(e->mode))
		return p + 2;

	auto ptr_type = s.main_type_back(0);
	auto decomp = e->dict.type_array[ptr_type].decomposed_types_start;
	auto pointer_contents = e->dict.all_stack_types[decomp + 1];

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildLoad2(e->llvm_builder, e->dict.type_array[pointer_contents].llvm_type, s.main_ex_back(0), "");
		s.pop_main();
		s.push_back_main(pointer_contents, 0, result);

		if(e->dict.type_array[pointer_contents].flags != 0) {
			execute_fif_word(fif::parse_result{ "dup", false }, *e, false);

			// remove the extra value and save the duplicated expression on top of the stack
			auto expr_b = s.main_ex_back(0);
			s.set_main_ex_back(1, expr_b);
			s.pop_main();
		}
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		s.pop_main();
		int64_t* ptr_val = (int64_t*)v;
		auto effective_type = pointer_contents;

		if(pointer_contents != -1 && e->dict.type_array[pointer_contents].refcounted_type()) {
			if(e->dict.type_array[pointer_contents].decomposed_types_count >= 2) {
				auto child_index = e->dict.type_array[pointer_contents].decomposed_types_start + 1;
				auto child_type = e->dict.all_stack_types[child_index];

				if(e->dict.type_array[pointer_contents].single_member_struct()) {
					effective_type = child_type;
				}
			}
		}

		if(effective_type == fif_i32 || effective_type == fif_f32)
			s.push_back_main(pointer_contents, *(int32_t*)(ptr_val), nullptr);
		else if(effective_type == fif_u32)
			s.push_back_main(pointer_contents, *(uint32_t*)(ptr_val), nullptr);
		else if(effective_type == fif_i16)
			s.push_back_main(pointer_contents, *(int16_t*)(ptr_val), nullptr);
		else if(effective_type == fif_u16)
			s.push_back_main(pointer_contents, *(uint16_t*)(ptr_val), nullptr);
		else if(effective_type == fif_i8)
			s.push_back_main(pointer_contents, *(int8_t*)(ptr_val), nullptr);
		else if(effective_type == fif_u8 || effective_type == fif_bool)
			s.push_back_main(pointer_contents, *(uint8_t*)(ptr_val), nullptr);
		else
			s.push_back_main(pointer_contents, *ptr_val, nullptr);

		if(e->dict.type_array[pointer_contents].flags != 0) {
			execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
			// remove the extra value and save the duplicated expression on top of the stack
			auto data_b = s.main_data_back(0);
			s.set_main_data_back(1, data_b);
			s.pop_main();
		}
	} else if(fif::typechecking_mode(e->mode)) {
		s.pop_main();
		s.push_back_main(pointer_contents, 0, nullptr);
	}
	return p + 2;
}

inline int32_t* impl_load_deallocated(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_failed(e->mode))
		return p + 2;

	auto ptr_type = s.main_type_back(0);
	auto decomp = e->dict.type_array[ptr_type].decomposed_types_start;
	auto pointer_contents = e->dict.all_stack_types[decomp + 1];

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildLoad2(e->llvm_builder, e->dict.type_array[pointer_contents].llvm_type, s.main_ex_back(0), "");
		s.pop_main();
		s.push_back_main(pointer_contents, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		s.pop_main();
		int64_t* ptr_val = (int64_t*)v;
		auto effective_type = pointer_contents;

		if(pointer_contents != -1 && e->dict.type_array[pointer_contents].refcounted_type()) {
			if(e->dict.type_array[pointer_contents].decomposed_types_count >= 2) {
				auto child_index = e->dict.type_array[pointer_contents].decomposed_types_start + 1;
				auto child_type = e->dict.all_stack_types[child_index];

				if(e->dict.type_array[pointer_contents].single_member_struct()) {
					effective_type = child_type;
				}
			}
		}

		if(effective_type == fif_i32 || effective_type == fif_f32)
			s.push_back_main(pointer_contents, *(int32_t*)(ptr_val), nullptr);
		else if(effective_type == fif_u32)
			s.push_back_main(pointer_contents, *(uint32_t*)(ptr_val), nullptr);
		else if(effective_type == fif_i16)
			s.push_back_main(pointer_contents, *(int16_t*)(ptr_val), nullptr);
		else if(effective_type == fif_u16)
			s.push_back_main(pointer_contents, *(uint16_t*)(ptr_val), nullptr);
		else if(effective_type == fif_i8)
			s.push_back_main(pointer_contents, *(int8_t*)(ptr_val), nullptr);
		else if(effective_type == fif_u8 || effective_type == fif_bool)
			s.push_back_main(pointer_contents, *(uint8_t*)(ptr_val), nullptr);
		else
			s.push_back_main(pointer_contents, *ptr_val, nullptr);

	} else if(fif::typechecking_mode(e->mode)) {
		s.pop_main();
		s.push_back_main(pointer_contents, 0, nullptr);
	}
	return p + 2;
}

inline int32_t* impl_store(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_failed(e->mode))
		return p + 2;

	auto ptr_type = s.main_type_back(0);
	auto decomp = e->dict.type_array[ptr_type].decomposed_types_start;
	auto pointer_contents = e->dict.all_stack_types[decomp + 1];

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(e->dict.type_array[pointer_contents].flags != 0) {
			auto iresult = LLVMBuildLoad2(e->llvm_builder, e->dict.type_array[pointer_contents].llvm_type, s.main_ex_back(0), "");
			s.push_back_main(pointer_contents, 0, iresult);
			execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
		}

		auto result = LLVMBuildStore(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0));
		s.pop_main();
		s.pop_main();
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {

		auto v = s.main_data_back(0);
		int64_t* ptr_val = (int64_t*)v;

		if(e->dict.type_array[pointer_contents].flags != 0) {
			s.push_back_main(pointer_contents, *ptr_val, nullptr);
			execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
		}

		auto to_write = s.main_data_back(1);

		auto effective_type = pointer_contents;
		if(pointer_contents != -1 && e->dict.type_array[pointer_contents].refcounted_type()) {
			if(e->dict.type_array[pointer_contents].decomposed_types_count >= 2) {
				auto child_index = e->dict.type_array[pointer_contents].decomposed_types_start + 1;
				auto child_type = e->dict.all_stack_types[child_index];

				if(e->dict.type_array[pointer_contents].single_member_struct()) {
					effective_type = child_type;
				}
			}
		}

		if(effective_type == fif_i32 || effective_type == fif_f32 || effective_type == fif_u32)
			memcpy(ptr_val, &to_write, 4);
		else if(effective_type == fif_u16 || effective_type == fif_i16)
			memcpy(ptr_val, &to_write, 2);
		else if(effective_type == fif_u8 || effective_type == fif_bool || effective_type == fif_i8)
			memcpy(ptr_val, &to_write, 1);
		else
			memcpy(ptr_val, &to_write, 8);

		s.pop_main();
		s.pop_main();
	} else if(fif::typechecking_mode(e->mode)) {
		s.pop_main();
		s.pop_main();
	}
	return p + 2;
}

inline int32_t* impl_uninit_store(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_failed(e->mode))
		return p + 2;

	auto ptr_type = s.main_type_back(0);
	auto decomp = e->dict.type_array[ptr_type].decomposed_types_start;
	auto pointer_contents = e->dict.all_stack_types[decomp + 1];

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildStore(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0));
		s.pop_main();
		s.pop_main();
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {

		auto v = s.main_data_back(0);
		int64_t* ptr_val = (int64_t*)v;

		auto to_write = s.main_data_back(1);

		auto effective_type = pointer_contents;
		if(pointer_contents != -1 && e->dict.type_array[pointer_contents].refcounted_type()) {
			if(e->dict.type_array[pointer_contents].decomposed_types_count >= 2) {
				auto child_index = e->dict.type_array[pointer_contents].decomposed_types_start + 1;
				auto child_type = e->dict.all_stack_types[child_index];

				if(e->dict.type_array[pointer_contents].single_member_struct()) {
					effective_type = child_type;
				}
			}
		}

		if(effective_type == fif_i32 || effective_type == fif_f32 || effective_type == fif_u32)
			memcpy(ptr_val, &to_write, 4);
		else if(effective_type == fif_u16 || effective_type == fif_i16)
			memcpy(ptr_val, &to_write, 2);
		else if(effective_type == fif_u8 || effective_type == fif_bool || effective_type == fif_i8)
			memcpy(ptr_val, &to_write, 1);
		else
			memcpy(ptr_val, &to_write, 8);

		s.pop_main();
		s.pop_main();
	} else if(fif::typechecking_mode(e->mode)) {
		s.pop_main();
		s.pop_main();
	}
	return p + 2;
}

inline int32_t* do_pointer_cast(fif::state_stack& s, int32_t* p, fif::environment* e) {
	s.set_main_type_back(0, *(p + 2));
	return p + 3;
}


inline int32_t* pointer_cast(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->source_stack.empty()) {
		e->report_error("pointer cast was unable to read the word describing the pointer type");
		e->mode = fif_mode::error;
		return nullptr;
	}
	auto ptype = read_token(e->source_stack.back(), *e);
	bool bad_type = ptype.is_string;
	auto resolved_type = resolve_type(ptype.content, *e, e->compiler_stack.back()->type_substitutions());
	if(resolved_type == -1) {
		bad_type = true;
	} else if(resolved_type != fif_opaque_ptr) {
		if(e->dict.type_array[resolved_type].decomposed_types_count == 0) {
			bad_type = true;
		} else if(e->dict.all_stack_types[e->dict.type_array[resolved_type].decomposed_types_start] != fif_ptr) {
			bad_type = true;
		}
	}

	if(bad_type) {
		e->report_error("pointer attempted to be cast to a non pointer type");
		e->mode = fif_mode::error;
		return nullptr;
	}

	if(!typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
		s.set_main_type_back(0, resolved_type);
	}

	if(e->mode == fif::fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			fif_call imm = do_pointer_cast;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			compile_bytes->push_back(resolved_type);
		}
	}
	return p + 2;
}

inline int32_t* impl_sizeof(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->source_stack.empty()) {
		e->report_error("sizeof was unable to read the word describing the type");
		e->mode = fif_mode::error;
		return nullptr;
	}
	auto ptype = read_token(e->source_stack.back(), *e);
	bool bad_type = ptype.is_string;
	auto resolved_type = resolve_type(ptype.content, *e, e->compiler_stack.back()->type_substitutions());
	if(resolved_type == -1) {
		e->report_error("sizeof given an invalid type");
		e->mode = fif_mode::error;
		return nullptr;
	}

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.push_back_main(fif_i32, 0, LLVMConstTrunc(LLVMSizeOf(e->dict.type_array[resolved_type].llvm_type), LLVMInt32TypeInContext(e->llvm_context)));
#endif
	} else if(e->mode == fif::fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			fif_call imm = immediate_i32;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			compile_bytes->push_back(interepreter_size(resolved_type, *e));
		}
		s.push_back_main(fif_i32, 8, 0);
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.push_back_main(fif_i32, 8, 0);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.push_back_main(fif_i32, 0, 0);
	}
	return p + 2;
}

inline int32_t* impl_index(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto num_bytes = s.main_ex_back(1);
		auto iresult = LLVMBuildInBoundsGEP2(e->llvm_builder, LLVMInt8TypeInContext(e->llvm_context), s.main_ex_back(0), &num_bytes, 1, "");
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_opaque_ptr, 0, iresult);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto bytes = s.main_data_back(1);
		auto pval = s.main_data_back(0);
		uint8_t* ptr = (uint8_t*)pval;
		pval += bytes;
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_opaque_ptr, (int64_t)pval, 0);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_opaque_ptr, 0, 0);
	}

	return p + 2;
}

inline int32_t* allocate_buffer(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto iresult = LLVMBuildArrayMalloc(e->llvm_builder, LLVMInt8TypeInContext(e->llvm_context), s.main_ex_back(0), "");
		LLVMBuildMemSet(e->llvm_builder, iresult, LLVMConstInt(LLVMInt8TypeInContext(e->llvm_context), 0, false), s.main_ex_back(0), 1);
		s.pop_main();
		s.push_back_main(fif_opaque_ptr, 0, iresult);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {

		auto bytes = s.main_data_back(0);
		auto val = malloc(size_t(bytes));
		memset(val, 0, size_t(bytes));
		s.pop_main();
		s.push_back_main(fif_opaque_ptr, (int64_t)val, 0);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif_opaque_ptr, 0, 0);
	}
	return p + 2;
}

inline int32_t* copy_buffer(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto bytes = s.main_ex_back(0);
		auto dest_ptr = s.main_ex_back(1);
		auto source_ptr = s.main_ex_back(2);
		s.pop_main();
		s.pop_main();
		s.pop_main();
		auto res = LLVMBuildMemCpy(e->llvm_builder, dest_ptr, 1, source_ptr, 1, bytes);
		s.push_back_main(fif_opaque_ptr, 0, dest_ptr);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto bytes = s.main_data_back(0);
		auto dest_ptr = s.main_data_back(1);
		auto source_ptr = s.main_data_back(2);
		s.pop_main();
		s.pop_main();
		s.pop_main();
		memcpy((void*)dest_ptr, (void*)source_ptr, size_t(bytes));
		s.push_back_main(fif_opaque_ptr, dest_ptr, 0);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif_opaque_ptr, 0, 0);
	}
	return p + 2;
}

inline int32_t* free_buffer(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_failed(e->mode))
		return p + 2;

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		LLVMBuildFree(e->llvm_builder, s.main_ex_back(0));
		s.pop_main();
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto a = s.main_data_back(0);
		void* ptr = (void*)a;
		free(ptr);
		s.pop_main();
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
	}
	return p + 2;
}

inline int32_t* do_let_creation(fif::state_stack& s, int32_t* p, fif::environment* e) {
	char* string_ptr = nullptr;
	memcpy(&string_ptr, p + 2, 8);

	auto l = e->compiler_stack.back()->create_let(std::string{ string_ptr }, s.main_type_back(0), s.main_data_back(0), nullptr);
	if(!l) {
		e->report_error("could not create a let with that name");
		e->mode = fif_mode::error;
		return nullptr;
	}
	s.pop_main();
	return p + 4;
}

inline int32_t* create_let(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->source_stack.empty()) {
		e->report_error("let was unable to read the declaration name");
		e->mode = fif_mode::error;
		return nullptr;
	}
	auto name = read_token(e->source_stack.back(), *e);

	if(e->mode == fif_mode::interpreting || (typechecking_mode(e->mode) && !typechecking_failed(e->mode))) {
		auto l = e->compiler_stack.back()->create_let(std::string{ name.content }, s.main_type_back(0), s.main_data_back(0), nullptr);
		if(!l) {
			e->report_error("could not create a let with that name");
			e->mode = fif_mode::error;
			return nullptr;
		}
	} else if(e->mode == fif_mode::compiling_llvm) {
		auto l = e->compiler_stack.back()->create_let(std::string{ name.content }, s.main_type_back(0), 0, s.main_ex_back(0));
		if(!l) {
			e->report_error("could not create a let with that name");
			e->mode = fif_mode::error;
			return nullptr;
		}
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto l = e->compiler_stack.back()->create_let(std::string{ name.content }, s.main_type_back(0), 0, 0);
		if(!l) {
			e->report_error("could not create a let with that name");
			e->mode = fif_mode::error;
			return nullptr;
		}
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			fif_call imm = do_let_creation;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));

			auto string_constant = e->get_string_constant(name.content);
			char const* cptr = string_constant.data();
			uint64_t let_addr = 0;
			memcpy(&let_addr, &cptr, 8);
			compile_bytes->push_back(int32_t(let_addr & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((let_addr >> 32) & 0xFFFFFFFF));
		}
	}

	if(!typechecking_failed(e->mode)) {
		s.pop_main();
	}
	return p + 2;
}

inline int32_t* do_var_creation(fif::state_stack& s, int32_t* p, fif::environment* e) {
	char* string_ptr = nullptr;
	memcpy(&string_ptr, p + 2, 8);

	auto l = e->compiler_stack.back()->create_var(std::string{ string_ptr }, s.main_type_back(0));
	if(!l) {
		e->report_error("could not create a var with that name");
		e->mode = fif_mode::error;
		return nullptr;
	}
	l->data = s.main_data_back(0);
	s.pop_main();
	return p + 4;
}

inline int32_t* create_var(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->source_stack.empty()) {
		e->report_error("war was unable to read the declaration name");
		e->mode = fif_mode::error;
		return nullptr;
	}
	auto name = read_token(e->source_stack.back(), *e);

	if(e->mode == fif_mode::interpreting || (typechecking_mode(e->mode) && !typechecking_failed(e->mode))) {
		auto l = e->compiler_stack.back()->create_var(std::string{ name.content }, s.main_type_back(0));
		if(!l) {
			e->report_error("could not create a var with that name");
			e->mode = fif_mode::error;
			return nullptr;
		}
		l->data = s.main_data_back(0);
	} else if(e->mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto l = e->compiler_stack.back()->create_var(std::string{ name.content }, s.main_type_back(0));
		if(!l) {
			e->report_error("could not create a var with that name");
			e->mode = fif_mode::error;
			return nullptr;
		}
		LLVMBuildStore(e->llvm_builder, s.main_ex_back(0), l->alloc);
#endif
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto l = e->compiler_stack.back()->create_var(std::string{ name.content }, s.main_type_back(0));
		if(!l) {
			e->report_error("could not create a var with that name");
			e->mode = fif_mode::error;
			return nullptr;
		}
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			fif_call imm = do_var_creation;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));

			auto string_constant = e->get_string_constant(name.content);
			char const* cptr = string_constant.data();
			uint64_t let_addr = 0;
			memcpy(&let_addr, &cptr, 8);
			compile_bytes->push_back(int32_t(let_addr & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((let_addr >> 32) & 0xFFFFFFFF));
		}
	}

	if(!typechecking_failed(e->mode)) {
		s.pop_main();
	}
	return p + 2;
}

inline int32_t* create_global_impl(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->source_stack.empty()) {
		e->report_error("global was unable to read the declaration name");
		e->mode = fif_mode::error;
		return nullptr;
	}
	auto name = read_token(e->source_stack.back(), *e);
	auto type = s.main_data_back(0);

	if(typechecking_mode(e->mode)) {
		if(!typechecking_failed(e->mode))
			s.pop_main();
		return p + 2;
	}

	s.pop_main();
	for(auto& ptr : e->compiler_stack) {
		if(ptr->get_type() == control_structure::globals) {
			auto existing = static_cast<compiler_globals_layer*>(ptr.get())->get_global_var(std::string{ name.content });

			if(existing) {
				e->report_error("duplicate global definition");
				e->mode = fif_mode::error;
				return nullptr;
			}

			static_cast<compiler_globals_layer*>(ptr.get())->create_global_var(std::string{ name.content }, int32_t(type));
			return p + 2;
		}
	}

	e->report_error("could not find the globals layer");
	e->mode = fif_mode::error;
	return nullptr;
}

inline int32_t* do_fextract(fif::state_stack& s, int32_t* p, fif::environment* e) {
	int32_t index_value = *(p + 2);
	auto stype = s.main_type_back(0);
	assert(1 + index_value < e->dict.type_array[stype].decomposed_types_count);
	auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + index_value;
	auto child_type = e->dict.all_stack_types[child_index];

	if(e->dict.type_array[stype].single_member_struct()) {
		s.mark_used_from_main(1);
		s.set_main_type_back(0, child_type);
	} else {
		auto ptr = s.main_data_back(0);
		auto children = (int64_t*)(ptr);
		auto child_data = children[index_value];

		if(e->dict.type_array[child_type].flags != 0) {
			s.push_back_main(child_type, child_data, nullptr);
			execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
			child_data = s.main_data_back(0);
			s.pop_main();
			s.pop_main();
		}

		if(e->dict.type_array[stype].flags != 0) {
			execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
		} else {
			s.pop_main();
		}

		s.push_back_main(child_type, child_data, nullptr);
	}

	return p + 3;
}

inline int32_t* forth_extract(fif::state_stack& s, int32_t* p, fif::environment* e) {
	auto index_str = read_token(e->source_stack.back(), *e);
	auto index_value = parse_int(index_str.content);
	auto stype = s.main_type_back(0);

	if(stype == -1 || e->dict.type_array[stype].refcounted_type() == false) {
		e->report_error("attempted to use a structure operation on a non-structure type");
		e->mode = fif_mode::error;
		return nullptr;
	}

	assert(1 + index_value < e->dict.type_array[stype].decomposed_types_count);
	auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + index_value;
	auto child_type = e->dict.all_stack_types[child_index];

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(1);
			s.set_main_type_back(0, child_type);
		} else {
			auto struct_expr = s.main_ex_back(0);
			auto result = LLVMBuildExtractValue(e->llvm_builder, struct_expr, index_value, "");

			if(e->dict.type_array[child_type].flags != 0) {
				s.push_back_main(child_type, 0, result);
				execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
				result = s.main_ex_back(0);
				s.pop_main();
				s.pop_main();
			}
			if(e->dict.type_array[stype].flags != 0) {
				execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
			} else {
				s.pop_main();
			}
			s.push_back_main(child_type, 0, result);
		}
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(1);
			s.set_main_type_back(0, child_type);
		} else {
			auto ptr = s.main_data_back(0);
			auto children = (int64_t*)(ptr);
			auto child_data = children[index_value];


			if(e->dict.type_array[child_type].flags != 0) {
				s.push_back_main(child_type, child_data, nullptr);
				execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
				child_data = s.main_data_back(0);
				s.pop_main();
				s.pop_main();
			}

			if(e->dict.type_array[stype].flags != 0) {
				execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
			} else {
				s.pop_main();
			}

			s.push_back_main(child_type, child_data, nullptr);
		}
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(child_type, 0, nullptr);
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			fif_call imm = do_fextract;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			compile_bytes->push_back(index_value);
		}
		s.pop_main();
		s.push_back_main(child_type, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* do_fextractc(fif::state_stack& s, int32_t* p, fif::environment* e) {
	int32_t index_value = *(p + 2);
	auto stype = s.main_type_back(0);
	assert(1 + index_value < e->dict.type_array[stype].decomposed_types_count);
	auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + index_value;
	auto child_type = e->dict.all_stack_types[child_index];

	if(e->dict.type_array[stype].single_member_struct()) {
		s.mark_used_from_main(1);
		s.set_main_type_back(0, child_type);
		execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
		s.set_main_type_back(1, stype);
	} else {
		auto ptr = s.main_data_back(0);
		auto children = (int64_t*)(ptr);
		auto child_data = children[index_value];

		if(e->dict.type_array[child_type].flags != 0) {
			s.push_back_main(child_type, child_data, nullptr);
			execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
			child_data = s.main_data_back(0);
			s.pop_main();
			s.pop_main();
		}

		s.mark_used_from_main(1);
		s.push_back_main(child_type, child_data, nullptr);
	}
	return p + 3;
}

inline int32_t* forth_extract_copy(fif::state_stack& s, int32_t* p, fif::environment* e) {
	auto index_str = read_token(e->source_stack.back(), *e);
	auto index_value = parse_int(index_str.content);
	auto stype = s.main_type_back(0);

	if(stype == -1 || e->dict.type_array[stype].refcounted_type() == false) {
		e->report_error("attempted to use a structure operation on a non-structure type");
		e->mode = fif_mode::error;
		return nullptr;
	}

	assert(1 + index_value < e->dict.type_array[stype].decomposed_types_count);
	auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + index_value;
	auto child_type = e->dict.all_stack_types[child_index];

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(1);
			s.set_main_type_back(0, child_type);
			execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
			s.set_main_type_back(1, stype);
		} else {
			auto struct_expr = s.main_ex_back(0);
			auto result = LLVMBuildExtractValue(e->llvm_builder, struct_expr, index_value, "");
			s.mark_used_from_main(1);

			if(e->dict.type_array[child_type].flags != 0) {
				s.push_back_main(child_type, 0, result);
				execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
				result = s.main_ex_back(0);
				s.pop_main();
				s.pop_main();
			}
			s.push_back_main(child_type, 0, result);
		}
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(1);
			s.set_main_type_back(0, child_type);
			execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
			s.set_main_type_back(1, stype);
		} else {
			auto ptr = s.main_data_back(0);
			auto children = (int64_t*)(ptr);
			auto child_data = children[index_value];
			s.mark_used_from_main(1);

			if(e->dict.type_array[child_type].flags != 0) {
				s.push_back_main(child_type, child_data, nullptr);
				execute_fif_word(fif::parse_result{ "dup", false }, *e, false);
				child_data = s.main_data_back(0);
				s.pop_main();
				s.pop_main();
			}

			s.push_back_main(child_type, child_data, nullptr);
		}
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
		s.push_back_main(child_type, 0, nullptr);
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			fif_call imm = do_fextractc;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			compile_bytes->push_back(index_value);
		}
		s.mark_used_from_main(1);
		s.push_back_main(child_type, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* do_finsert(fif::state_stack& s, int32_t* p, fif::environment* e) {
	int32_t index_value = *(p + 2);
	auto stype = s.main_type_back(0);
	assert(1 + index_value < e->dict.type_array[stype].decomposed_types_count);
	auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + index_value;
	auto child_type = e->dict.all_stack_types[child_index];

	if(e->dict.type_array[stype].single_member_struct()) {
		s.mark_used_from_main(2);
		s.set_main_type_back(0, child_type);
		execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
		s.set_main_type_back(0, stype);
	} else {
		auto ptr = s.main_data_back(0);
		auto children = (int64_t*)(ptr);

		if(e->dict.type_array[child_type].flags != 0) {
			s.push_back_main(child_type, children[index_value], 0);
			execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
		}
		s.pop_main();
		children[index_value] = s.main_data_back(0);
		s.pop_main();

		s.push_back_main(stype, ptr, 0);
	}
	return p + 3;
}

inline int32_t* forth_insert(fif::state_stack& s, int32_t* p, fif::environment* e) {
	auto index_str = read_token(e->source_stack.back(), *e);
	auto index_value = parse_int(index_str.content);
	auto stype = s.main_type_back(0);

	if(stype == -1 || e->dict.type_array[stype].refcounted_type() == false) {
		e->report_error("attempted to use a structure operation on a non-structure type");
		e->mode = fif_mode::error;
		return nullptr;
	}

	assert(1 + index_value < e->dict.type_array[stype].decomposed_types_count);
	auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + index_value;
	auto child_type = e->dict.all_stack_types[child_index];

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(2);
			s.set_main_type_back(0, child_type);
			execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
			s.set_main_type_back(0, stype);
		} else {
			auto struct_expr = s.main_ex_back(0);

			if(e->dict.type_array[child_type].flags != 0) {
				auto oldv = LLVMBuildExtractValue(e->llvm_builder, struct_expr, index_value, "");
				s.push_back_main(child_type, 0, oldv);
				execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
			}

			s.pop_main();
			struct_expr = LLVMBuildInsertValue(e->llvm_builder, struct_expr, s.main_ex_back(0), index_value, "");
			s.pop_main();
			s.push_back_main(stype, 0, struct_expr);
		}
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(2);
			s.set_main_type_back(0, child_type);
			execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
			s.set_main_type_back(0, stype);
		} else {
			auto ptr = s.main_data_back(0);
			auto children = (int64_t*)(ptr);

			if(e->dict.type_array[child_type].flags != 0) {
				s.push_back_main(child_type, children[index_value], 0);
				execute_fif_word(fif::parse_result{ "drop", false }, *e, false);
			}
			s.pop_main();
			children[index_value] = s.main_data_back(0);
			s.pop_main();

			s.push_back_main(stype, ptr, 0);
		}
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.pop_main();
		s.push_back_main(stype, 0, nullptr);
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			fif_call imm = do_finsert;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			compile_bytes->push_back(index_value);
		}
		s.pop_main();
		s.pop_main();
		s.push_back_main(stype, 0, nullptr);
	}
	return p + 2;
}

inline int32_t* do_fgep(fif::state_stack& s, int32_t* p, fif::environment* e) {
	int32_t index_value = *(p + 2);
	auto ptr_type = s.main_type_back(0);

	if(ptr_type == -1) {
		e->report_error("attempted to use a pointer operation on a non-pointer type");
		e->mode = fif_mode::error;
		return nullptr;
	}
	auto decomp = e->dict.type_array[ptr_type].decomposed_types_start;
	if(e->dict.type_array[ptr_type].decomposed_types_count == 0 || e->dict.all_stack_types[e->dict.type_array[ptr_type].decomposed_types_start] != fif_ptr) {
		e->report_error("attempted to use a struct-pointer operation on a non-struct-pointer type");
		e->mode = fif_mode::error;
		return nullptr;
	}
	auto stype = e->dict.all_stack_types[decomp + 1];

	assert(1 + index_value < e->dict.type_array[stype].decomposed_types_count);
	auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + index_value;
	auto child_type = e->dict.all_stack_types[child_index];

	if(e->dict.type_array[stype].single_member_struct()) {
		int32_t type_storage[] = { fif_ptr, std::numeric_limits<int32_t>::max(), child_type, -1 };
		std::vector<int32_t> subs;
		auto child_ptr_type = resolve_span_type(std::span<int32_t const>(type_storage, type_storage + 4), subs, *e);

		s.mark_used_from_main(1);
		s.set_main_type_back(0, child_ptr_type.type);
	} else {
		auto ptr = s.main_data_back(0);
		auto children = (int64_t*)(ptr);
		auto child_ptr = children + index_value;

		int32_t type_storage[] = { fif_ptr, std::numeric_limits<int32_t>::max(), child_type, -1 };
		std::vector<int32_t> subs;
		auto child_ptr_type = resolve_span_type(std::span<int32_t const>(type_storage, type_storage + 4), subs, *e);

		s.pop_main();
		s.push_back_main(child_ptr_type.type, (int64_t)child_ptr, 0);
	}
	return p + 3;
}

inline int32_t* forth_gep(fif::state_stack& s, int32_t* p, fif::environment* e) {
	auto index_str = read_token(e->source_stack.back(), *e);
	auto index_value = parse_int(index_str.content);
	auto ptr_type = s.main_type_back(0);

	if(ptr_type == -1) {
		e->report_error("attempted to use a pointer operation on a non-pointer type");
		e->mode = fif_mode::error;
		return nullptr;
	}

	auto decomp = e->dict.type_array[ptr_type].decomposed_types_start;

	if(e->dict.type_array[ptr_type].decomposed_types_count == 0 || e->dict.all_stack_types[e->dict.type_array[ptr_type].decomposed_types_start] != fif_ptr) {
		e->report_error("attempted to use a struct-pointer operation on a non-struct-pointer type");
		e->mode = fif_mode::error;
		return nullptr;
	}

	auto stype = e->dict.all_stack_types[decomp + 1];

	if(stype == -1 || e->dict.type_array[stype].refcounted_type() == false) {
		e->report_error("attempted to use a structure operation on a non-structure type");
		e->mode = fif_mode::error;
		return nullptr;
	}

	assert(1 + index_value < e->dict.type_array[stype].decomposed_types_count);
	auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + index_value;
	auto child_type = e->dict.all_stack_types[child_index];

	int32_t type_storage[] = { fif_ptr, std::numeric_limits<int32_t>::max(), child_type, -1 };
	std::vector<int32_t> subs;
	auto child_ptr_type = resolve_span_type(std::span<int32_t const>(type_storage, type_storage + 4), subs, *e);

	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(1);
			s.set_main_type_back(0, child_ptr_type.type);
		} else {
			auto struct_expr = s.main_ex_back(0);
			auto ptr_expr = LLVMBuildStructGEP2(e->llvm_builder, e->dict.type_array[stype].llvm_type, struct_expr, uint32_t(index_value), "");
			s.pop_main();
			s.push_back_main(child_ptr_type.type, 0, ptr_expr);
		}
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(1);
			s.set_main_type_back(0, child_ptr_type.type);
		} else {
			auto ptr = s.main_data_back(0);
			auto children = (int64_t*)(ptr);
			auto child_ptr = children + index_value;

			s.pop_main();
			s.push_back_main(child_ptr_type.type, (int64_t)child_ptr, 0);
		}
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(child_ptr_type.type, 0, nullptr);
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			fif_call imm = do_fgep;
			uint64_t imm_bytes = 0;
			memcpy(&imm_bytes, &imm, 8);
			compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
			compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			compile_bytes->push_back(index_value);
		}
		s.pop_main();
		s.push_back_main(child_ptr_type.type, 0, nullptr);
	}
	return p + 2;
}

inline int32_t* do_fsmz(fif::state_stack& s, int32_t* p, fif::environment* e) {
	char* command = nullptr;
	memcpy(&command, p + 2, 8);

	auto stype = s.main_type_back(0);

	if(e->dict.type_array[stype].single_member_struct()) {
		auto child_index = e->dict.type_array[stype].decomposed_types_start + 1;
		auto child_type = e->dict.all_stack_types[child_index];

		s.mark_used_from_main(1);
		s.set_main_type_back(0, child_type);
		execute_fif_word(parse_result{ std::string_view{ command }, false }, *e, false);
	} else {
		auto children_count = struct_child_count(stype, *e);
		auto ptr = s.main_data_back(0);
		auto children = (int64_t*)(ptr);

		for(int32_t i = 0; i < children_count; ++i) {
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
			auto child_type = e->dict.all_stack_types[child_index];
			s.push_back_main(child_type, children[i], nullptr);
			execute_fif_word(parse_result{ std::string_view{ command }, false }, *e, false);
		}

		free(children);
		s.pop_main();
	}
	return p + 4;
}

inline int32_t* forth_struct_map_zero(fif::state_stack& s, int32_t* p, fif::environment* e) {
	auto mapped_function = read_token(e->source_stack.back(), *e);
	auto stype = s.main_type_back(0);
	auto children_count = struct_child_count(stype, *e);


	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(e->dict.type_array[stype].single_member_struct()) {
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1;
			auto child_type = e->dict.all_stack_types[child_index];

			s.mark_used_from_main(1);
			s.set_main_type_back(0, child_type);
			execute_fif_word(mapped_function, *e, false);
		} else {
			auto struct_expr = s.main_ex_back(0);

			for(int32_t i = 0; i < children_count; ++i) {
				auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
				auto child_type = e->dict.all_stack_types[child_index];
				s.push_back_main(child_type, 0, LLVMBuildExtractValue(e->llvm_builder, struct_expr, uint32_t(i), ""));
				execute_fif_word(mapped_function, *e, false);
			}

			s.pop_main();
		}
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		if(e->dict.type_array[stype].single_member_struct()) {
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1;
			auto child_type = e->dict.all_stack_types[child_index];

			s.mark_used_from_main(1);
			s.set_main_type_back(0, child_type);
			execute_fif_word(mapped_function, *e, false);
		} else {
			auto ptr = s.main_data_back(0);
			auto children = (int64_t*)(ptr);

			for(int32_t i = 0; i < children_count; ++i) {
				auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
				auto child_type = e->dict.all_stack_types[child_index];
				s.push_back_main(child_type, children[i], nullptr);
				execute_fif_word(mapped_function, *e, false);
			}
			free(children);
			s.pop_main();
		}
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			{
				fif_call imm = do_fsmz;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			}
			{
				auto str_const = e->get_string_constant(mapped_function.content).data();
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &str_const, 8);
				compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			}
		}
		s.pop_main();
	}
	return p + 2;
}

inline int32_t* do_fsmo(fif::state_stack& s, int32_t* p, fif::environment* e) {
	char* command = nullptr;
	memcpy(&command, p + 2, 8);

	auto stype = s.main_type_back(0);

	if(e->dict.type_array[stype].single_member_struct()) {
		auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + 0;
		auto child_type = e->dict.all_stack_types[child_index];
		s.set_main_type_back(0, child_type);
		execute_fif_word(parse_result{ std::string_view{ command }, false }, *e, false);
		s.set_main_type_back(0, stype);
	} else {
		auto children_count = struct_child_count(stype, *e);
		auto ptr = s.main_data_back(0);
		auto children = (int64_t*)(ptr);

		for(int32_t i = 0; i < children_count; ++i) {
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
			auto child_type = e->dict.all_stack_types[child_index];
			s.push_back_main(child_type, children[i], nullptr);
			execute_fif_word(parse_result{ std::string_view{ command }, false }, *e, false);
			children[i] = s.main_data_back(0);
			s.pop_main();
		}
	}

	s.mark_used_from_main(1);
	return p + 4;
}
inline int32_t* forth_struct_map_one(fif::state_stack& s, int32_t* p, fif::environment* e) {
	auto mapped_function = read_token(e->source_stack.back(), *e);
	auto stype = s.main_type_back(0);
	auto children_count = struct_child_count(stype, *e);


	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(e->dict.type_array[stype].single_member_struct()) {
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + 0;
			auto child_type = e->dict.all_stack_types[child_index];
			s.set_main_type_back(0, child_type);
			execute_fif_word(mapped_function, *e, false);
			s.set_main_type_back(0, stype);
		} else {
			auto struct_expr = s.main_ex_back(0);

			for(int32_t i = 0; i < children_count; ++i) {
				auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
				auto child_type = e->dict.all_stack_types[child_index];
				s.push_back_main(child_type, 0, LLVMBuildExtractValue(e->llvm_builder, struct_expr, uint32_t(i), ""));
				execute_fif_word(mapped_function, *e, false);
				struct_expr = LLVMBuildInsertValue(e->llvm_builder, struct_expr, s.main_ex_back(0), uint32_t(i), "");
				s.pop_main();
			}
			s.set_main_ex_back(0, struct_expr);
		}
		s.mark_used_from_main(1);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		if(e->dict.type_array[stype].single_member_struct()) {
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + 0;
			auto child_type = e->dict.all_stack_types[child_index];
			s.set_main_type_back(0, child_type);
			execute_fif_word(mapped_function, *e, false);
			s.set_main_type_back(0, stype);
		} else {
			auto ptr = s.main_data_back(0);
			auto children = (int64_t*)(ptr);

			for(int32_t i = 0; i < children_count; ++i) {
				auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
				auto child_type = e->dict.all_stack_types[child_index];
				s.push_back_main(child_type, children[i], nullptr);
				execute_fif_word(mapped_function, *e, false);

				children[i] = s.main_data_back(0);

				s.pop_main();
			}
		}
		s.mark_used_from_main(1);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			{
				fif_call imm = do_fsmo;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			}
			{
				auto str_const = e->get_string_constant(mapped_function.content).data();
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &str_const, 8);
				compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			}
		}
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* do_fsmt(fif::state_stack& s, int32_t* p, fif::environment* e) {
	char* command = nullptr;
	memcpy(&command, p + 2, 8);

	auto stype = s.main_type_back(0);

	if(e->dict.type_array[stype].single_member_struct()) {
		s.mark_used_from_main(1);
		auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + 0;
		auto child_type = e->dict.all_stack_types[child_index];
		s.set_main_type_back(0, child_type);
		execute_fif_word(parse_result{ std::string_view{ command }, false }, *e, false);
		s.set_main_type_back(0, stype);
		s.set_main_type_back(1, stype);
	} else {
		auto children_count = struct_child_count(stype, *e);
		auto ptr = s.main_data_back(0);
		auto new_ptr = e->dict.type_array[stype].interpreter_zero(stype, e);

		auto children = (int64_t*)(ptr);
		auto new_children = (int64_t*)(new_ptr);

		for(int32_t i = 0; i < children_count; ++i) {
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
			auto child_type = e->dict.all_stack_types[child_index];
			s.push_back_main(child_type, children[i], nullptr);
			execute_fif_word(parse_result{ std::string_view{ command }, false }, *e, false);

			new_children[i] = s.main_data_back(0);

			s.pop_main();
			s.pop_main();
		}

		s.mark_used_from_main(1);
		s.push_back_main(stype, new_ptr, nullptr);
	}
	return p + 4;
}
inline int32_t* forth_struct_map_two(fif::state_stack& s, int32_t* p, fif::environment* e) {
	auto mapped_function = read_token(e->source_stack.back(), *e);
	auto stype = s.main_type_back(0);
	auto children_count = struct_child_count(stype, *e);


	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(1);
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + 0;
			auto child_type = e->dict.all_stack_types[child_index];
			s.set_main_type_back(0, child_type);
			execute_fif_word(mapped_function, *e, false);
			s.set_main_type_back(0, stype);
			s.set_main_type_back(1, stype);
		} else {
			auto struct_expr = s.main_ex_back(0);
			auto new_struct_expr = e->dict.type_array[stype].zero_constant(e->llvm_context, stype, e);

			for(int32_t i = 0; i < children_count; ++i) {
				auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
				auto child_type = e->dict.all_stack_types[child_index];
				s.push_back_main(child_type, 0, LLVMBuildExtractValue(e->llvm_builder, struct_expr, uint32_t(i), ""));
				execute_fif_word(mapped_function, *e, false);
				new_struct_expr = LLVMBuildInsertValue(e->llvm_builder, new_struct_expr, s.main_ex_back(0), uint32_t(i), "");
				s.pop_main();
				s.pop_main();
			}
			s.mark_used_from_main(1);
			s.push_back_main(stype, 0, new_struct_expr);
		}
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		if(e->dict.type_array[stype].single_member_struct()) {
			s.mark_used_from_main(1);
			auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + 0;
			auto child_type = e->dict.all_stack_types[child_index];
			s.set_main_type_back(0, child_type);
			execute_fif_word(mapped_function, *e, false);
			s.set_main_type_back(0, stype);
			s.set_main_type_back(1, stype);
		} else {
			auto ptr = s.main_data_back(0);
			auto new_ptr = e->dict.type_array[stype].interpreter_zero(stype, e);

			auto children = (int64_t*)(ptr);
			auto new_children = (int64_t*)(new_ptr);

			for(int32_t i = 0; i < children_count; ++i) {
				auto child_index = e->dict.type_array[stype].decomposed_types_start + 1 + i;
				auto child_type = e->dict.all_stack_types[child_index];
				s.push_back_main(child_type, children[i], nullptr);
				execute_fif_word(mapped_function, *e, false);


				new_children[i] = s.main_data_back(0);


				s.pop_main();
				s.pop_main();
			}

			s.mark_used_from_main(1);
			s.push_back_main(stype, new_ptr, nullptr);
		}
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
		s.push_back_main(stype, 0, nullptr);
	} else if(e->mode == fif_mode::compiling_bytecode) {
		auto compile_bytes = e->compiler_stack.back()->bytecode_compilation_progress();
		if(compile_bytes) {
			{
				fif_call imm = do_fsmt;
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &imm, 8);
				compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			}
			{
				auto str_const = e->get_string_constant(mapped_function.content).data();
				uint64_t imm_bytes = 0;
				memcpy(&imm_bytes, &str_const, 8);
				compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
				compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
			}
		}
		s.mark_used_from_main(1);
		s.push_back_main(stype, 0, nullptr);
	}
	return p + 2;
}

inline int32_t* type_construction(state_stack& s, int32_t* p, environment* e) {
	auto type = *(p + 2);
	auto is_refcounted = e->dict.type_array[type].refcounted_type();
	s.push_back_main( type, (is_refcounted && e->dict.type_array[type].interpreter_zero) ? e->dict.type_array[type].interpreter_zero(type, e) : 0, nullptr);
	return p + 3;
}
inline int32_t* do_make(state_stack& s, int32_t* p, environment* env) {
	if(env->source_stack.empty()) {
		env->report_error("make was unable to read the word describing the type");
		env->mode = fif_mode::error;
		return nullptr;
	}
	auto type = read_token(env->source_stack.back(), *env);
	
	if(typechecking_failed(env->mode))
		return p + 2;

	auto resolved_type = resolve_type(type.content, *env, env->compiler_stack.back()->type_substitutions());
	
	if(resolved_type == -1) {
		env->report_error("make was unable to resolve the type");
		env->mode = fif_mode::error;
		return nullptr;
	}

	if(typechecking_mode(env->mode)) {
		s.push_back_main(resolved_type, 0, nullptr);
		return p + 2;
	}

	auto compile_bytes = env->compiler_stack.back()->bytecode_compilation_progress();
	if(compile_bytes && env->mode == fif_mode::compiling_bytecode) {
		fif_call imm = type_construction;
		uint64_t imm_bytes = 0;
		memcpy(&imm_bytes, &imm, 8);
		compile_bytes->push_back(int32_t(imm_bytes & 0xFFFFFFFF));
		compile_bytes->push_back(int32_t((imm_bytes >> 32) & 0xFFFFFFFF));
		compile_bytes->push_back(resolved_type);
	}
	LLVMValueRef val = nullptr;
	if(env->mode == fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		if(env->dict.type_array[resolved_type].zero_constant) {
			val = env->dict.type_array[resolved_type].zero_constant(env->llvm_context, resolved_type, env);
		} else {
			env->report_error("attempted to compile a type without an llvm representation");
			env->mode = fif_mode::error;
			return nullptr;
		}
#endif
	}
	int64_t data = 0;
	if(env->mode == fif_mode::interpreting) {
		if(env->dict.type_array[resolved_type].refcounted_type() && env->dict.type_array[resolved_type].interpreter_zero)
			data = env->dict.type_array[resolved_type].interpreter_zero(resolved_type, env);
	}
	s.push_back_main(resolved_type, data, val);
	execute_fif_word(fif::parse_result{ "init", false }, *env, false);

	return p + 2;
}

inline int32_t* struct_definition(fif::state_stack&, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode))
		return p + 2;
	if(e->mode != fif::fif_mode::interpreting) {
		e->report_error("attempted to define a struct inside a definition");
		e->mode = fif::fif_mode::error;
		return nullptr;
	}

	if(e->source_stack.empty()) {
		e->report_error("attempted to define a struct without a source");
		e->mode = fif::fif_mode::error;
		return nullptr;
	}

	auto name_token = fif::read_token(e->source_stack.back(), *e);

	if(e->dict.types.find(std::string(name_token.content)) != e->dict.types.end()) {
		e->report_error("attempted to redefine an existing type");
		e->mode = fif::fif_mode::error;
		return nullptr;
	}

	std::vector<int32_t> stack_types;
	std::vector<std::string_view> names;
	int32_t max_variable = -1;
	bool read_extra_count = false;

	while(true) {
		auto next_token = fif::read_token(e->source_stack.back(), *e);
		if(next_token.content.length() == 0 || next_token.content == ";") {
			break;
		}
		if(next_token.content == "$") {
			read_extra_count = true;
			break;
		}

		auto result = internal_generate_type(next_token.content, *e);
		if(result.type_array.empty()) {
			e->mode = fif_mode::error;
			e->report_error("unable to resolve type from text");
			return nullptr;
		}
		stack_types.insert(stack_types.end(), result.type_array.begin(), result.type_array.end());
		max_variable = std::max(max_variable, result.max_variable);

		auto nnext_token = fif::read_token(e->source_stack.back(), *e);
		if(nnext_token.content.length() == 0 || nnext_token.content == ";") {
			e->report_error("struct contained a type without a matching name");
			e->mode = fif::fif_mode::error;
			return nullptr;
		}
		names.push_back(nnext_token.content);
	}

	int32_t extra_count = 0;
	if(read_extra_count) {
		auto next_token = fif::read_token(e->source_stack.back(), *e);
		auto next_next_token = fif::read_token(e->source_stack.back(), *e);
		if(next_next_token.content != ";") {
			e->report_error("struct definition ended incorrectly");
			e->mode = fif::fif_mode::error;
			return nullptr;
		}
		extra_count = parse_int(next_token.content);
	}

	make_struct_type(name_token.content, std::span<int32_t const>{stack_types.begin(), stack_types.end()}, names, *e, max_variable + 1, extra_count);

	return p + 2;
}
inline int32_t* export_definition(fif::state_stack&, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode))
		return p + 2;
	if(e->mode != fif::fif_mode::interpreting) {
		e->report_error("attempted to define an export inside a definition");
		e->mode = fif::fif_mode::error;
		return nullptr;
	}

	if(e->source_stack.empty()) {
		e->report_error("attempted to define an export without a source");
		e->mode = fif::fif_mode::error;
		return nullptr;
	}

	auto name_token = fif::read_token(e->source_stack.back(), *e);

	std::vector<int32_t> stack_types;
	std::vector<std::string_view> names;
	int32_t max_variable = -1;
	bool read_extra_count = false;

	while(true) {
		auto next_token = fif::read_token(e->source_stack.back(), *e);
		if(next_token.content.length() == 0 || next_token.content == ";") {
			break;
		}
		names.push_back(next_token.content);
	}

	auto fn_to_export = names.back();
	names.pop_back();

	for(auto tyn : names) {
		auto result = resolve_type(tyn, *e, nullptr);
		if(result == -1) {
			e->mode = fif_mode::error;
			e->report_error("unable to resolve type from text");
			return nullptr;
		}
		stack_types.push_back(result);
	}
#ifdef USE_LLVM
	make_exportable_function(std::string(name_token.content), std::string(fn_to_export), stack_types, { }, *e);
#endif
	return p + 2;
}
inline int32_t* do_use_base(state_stack& s, int32_t* p, environment* env) {
	if(env->source_stack.empty()) {
		env->report_error("make was unable to read name of word");
		env->mode = fif_mode::error;
		return nullptr;
	}
	auto type = read_token(env->source_stack.back(), *env);

	if(typechecking_failed(env->mode))
		return p + 2;

	execute_fif_word(type, *env, true);

	return p + 2;
}

inline int32_t* long_comment(fif::state_stack&, int32_t* p, fif::environment* e) {
	if(e->source_stack.empty()) {
		return p + 2;
	}

	int32_t depth = 1;
	while(depth > 0) {
		auto token = fif::read_token(e->source_stack.back(), *e);
		if(token.content == "))" && token.is_string == false)
			--depth;
		if(token.content == "((" && token.is_string == false)
			++depth;
		if(token.content == "" && token.is_string == false)
			break;
	}
	
	return p + 2;
}
inline int32_t* line_comment(fif::state_stack&, int32_t* p, fif::environment* e) {
	if(e->source_stack.empty()) {
		return p + 2;
	}

	std::string_view& source = e->source_stack.back();
	while(source.length() > 0 && source[0] != '\n' && source[0] != '\r')
		source = source.substr(1);

	return p + 2;
}

inline int32_t* zext_i64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildZExt(e->llvm_builder, s.main_ex_back(0), LLVMInt64TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_type_back(0, fif_i64);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* zext_i32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildZExt(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FFFFFFFF);
		s.set_main_type_back(0, fif_i32);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* zext_i16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildZExt(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FFFF);
		s.set_main_type_back(0, fif_i16);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* zext_i8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildZExt(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FF);
		s.set_main_type_back(0, fif_i8);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* zext_ui64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildZExt(e->llvm_builder, s.main_ex_back(0), LLVMInt64TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_type_back(0, fif_u64);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* zext_ui32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildZExt(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FFFFFFFF);
		s.set_main_type_back(0, fif_u32);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* zext_ui16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildZExt(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FFFF);
		s.set_main_type_back(0, fif_u16);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* zext_ui8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildZExt(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FF);
		s.set_main_type_back(0, fif_u8);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sext_i64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSExt(e->llvm_builder, s.main_ex_back(0), LLVMInt64TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		if(s.main_type_back(0) == fif_i8 || s.main_type_back(0) == fif_u8) {
			auto v = int8_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i16 || s.main_type_back(0) == fif_u16) {
			auto v = int16_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i32 || s.main_type_back(0) == fif_u32) {
			auto v = int32_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} if(s.main_type_back(0) == fif_bool) {
			auto v = s.main_data_back(0);
			s.set_main_data_back(0, v != 0 ? -1 : 0);
		}
		s.set_main_type_back(0, fif_i64);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sext_i32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSExt(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		if(s.main_type_back(0) == fif_i8 || s.main_type_back(0) == fif_u8) {
			auto v = int8_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i16 || s.main_type_back(0) == fif_u16) {
			auto v = int16_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i32 || s.main_type_back(0) == fif_u32) {
			auto v = int32_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} if(s.main_type_back(0) == fif_bool) {
			auto v = s.main_data_back(0);
			s.set_main_data_back(0, v != 0 ? -1 : 0);
		}
		s.set_main_type_back(0, fif_i32);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sext_i16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSExt(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		if(s.main_type_back(0) == fif_i8 || s.main_type_back(0) == fif_u8) {
			auto v = int8_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i16 || s.main_type_back(0) == fif_u16) {
			auto v = int16_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i32 || s.main_type_back(0) == fif_u32) {
			auto v = int32_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} if(s.main_type_back(0) == fif_bool) {
			auto v = s.main_data_back(0);
			s.set_main_data_back(0, v != 0 ? -1 : 0);
		}
		s.set_main_type_back(0, fif_i16);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sext_i8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSExt(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		if(s.main_type_back(0) == fif_i8 || s.main_type_back(0) == fif_u8) {
			auto v = int8_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i16 || s.main_type_back(0) == fif_u16) {
			auto v = int16_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i32 || s.main_type_back(0) == fif_u32) {
			auto v = int32_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} if(s.main_type_back(0) == fif_bool) {
			auto v = s.main_data_back(0);
			s.set_main_data_back(0, v != 0 ? -1 : 0);
		}
		s.set_main_type_back(0, fif_i8);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sext_ui64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSExt(e->llvm_builder, s.main_ex_back(0), LLVMInt64TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		if(s.main_type_back(0) == fif_i8 || s.main_type_back(0) == fif_u8) {
			auto v = int8_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i16 || s.main_type_back(0) == fif_u16) {
			auto v = int16_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i32 || s.main_type_back(0) == fif_u32) {
			auto v = int32_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} if(s.main_type_back(0) == fif_bool) {
			auto v = s.main_data_back(0);
			s.set_main_data_back(0, v != 0 ? -1 : 0);
		}
		s.set_main_type_back(0, fif_u64);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sext_ui32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSExt(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		if(s.main_type_back(0) == fif_i8 || s.main_type_back(0) == fif_u8) {
			auto v = int8_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i16 || s.main_type_back(0) == fif_u16) {
			auto v = int16_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i32 || s.main_type_back(0) == fif_u32) {
			auto v = int32_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} if(s.main_type_back(0) == fif_bool) {
			auto v = s.main_data_back(0);
			s.set_main_data_back(0, v != 0 ? -1 : 0);
		}
		s.set_main_type_back(0, fif_u32);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sext_ui16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSExt(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		if(s.main_type_back(0) == fif_i8 || s.main_type_back(0) == fif_u8) {
			auto v = int8_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i16 || s.main_type_back(0) == fif_u16) {
			auto v = int16_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i32 || s.main_type_back(0) == fif_u32) {
			auto v = int32_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} if(s.main_type_back(0) == fif_bool) {
			auto v = s.main_data_back(0);
			s.set_main_data_back(0, v != 0 ? -1 : 0);
		}
		s.set_main_type_back(0, fif_u16);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sext_ui8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSExt(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		if(s.main_type_back(0) == fif_i8 || s.main_type_back(0) == fif_u8) {
			auto v = int8_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i16 || s.main_type_back(0) == fif_u16) {
			auto v = int16_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} else if(s.main_type_back(0) == fif_i32 || s.main_type_back(0) == fif_u32) {
			auto v = int32_t(s.main_data_back(0));
			s.set_main_data_back(0, v);
		} if(s.main_type_back(0) == fif_bool) {
			auto v = s.main_data_back(0);
			s.set_main_data_back(0, v != 0 ? -1 : 0);
		}
		s.set_main_type_back(0, fif_u8);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* trunc_ui8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildTrunc(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FF);
		s.set_main_type_back(0, fif_u8);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* trunc_i8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildTrunc(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, int8_t(s.main_data_back(0) & 0x0FF));
		s.set_main_type_back(0, fif_i8);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* trunc_i1(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildTrunc(e->llvm_builder, s.main_ex_back(0), LLVMInt1TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_bool, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x01);
		s.set_main_type_back(0, fif_bool);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* trunc_ui16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildTrunc(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FFFF);
		s.set_main_type_back(0, fif_u16);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* trunc_i16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildTrunc(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, int16_t(s.main_data_back(0) & 0x0FFFF));
		s.set_main_type_back(0, fif_i16);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* trunc_ui32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildTrunc(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FFFFFFFF);
		s.set_main_type_back(0, fif_u32);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* trunc_i32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildTrunc(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, int32_t(s.main_data_back(0) & 0x0FFFFFFFF));
		s.set_main_type_back(0, fif_i32);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* nop1(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
		s.mark_used_from_main(1);
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* ftrunc(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPTrunc(e->llvm_builder, s.main_ex_back(0), LLVMFloatTypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_f32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		float fout = float(fin);
		int64_t iout = 0;
		memcpy(&iout, &fout, 4);
		s.pop_main();
		s.push_back_main(fif_f32, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* fext(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPExt(e->llvm_builder, s.main_ex_back(0), LLVMFloatTypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_f64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		double fout = double(fin);
		int64_t iout = 0;
		memcpy(&iout, &fout, 8);
		s.pop_main();
		s.push_back_main(fif_f64, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_f64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32i8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToSI(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		int64_t iout = int8_t(fin);
		s.pop_main();
		s.push_back_main(fif_i8, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sif32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSIToFP(e->llvm_builder, s.main_ex_back(0), LLVMFloatTypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_f32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fout = float(v);
		int64_t iout = 0;
		memcpy(&iout, &fout, 4);
		s.pop_main();
		s.push_back_main(fif_f32, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* uif32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildUIToFP(e->llvm_builder, s.main_ex_back(0), LLVMFloatTypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_f32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = uint64_t(s.main_data_back(0));
		float fout = float(v);
		int64_t iout = 0;
		memcpy(&iout, &fout, 4);
		s.pop_main();
		s.push_back_main(fif_f32, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* sif64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildSIToFP(e->llvm_builder, s.main_ex_back(0), LLVMDoubleTypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_f64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fout = double(v);
		int64_t iout = 0;
		memcpy(&iout, &fout, 8);
		s.pop_main();
		s.push_back_main(fif_f64, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_f64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* uif64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildUIToFP(e->llvm_builder, s.main_ex_back(0), LLVMDoubleTypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_f64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = uint64_t(s.main_data_back(0));
		double fout = double(v);
		int64_t iout = 0;
		memcpy(&iout, &fout, 8);
		s.pop_main();
		s.push_back_main(fif_f64, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_f64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32i16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToSI(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		int64_t iout = int16_t(fin);
		s.pop_main();
		s.push_back_main(fif_i16, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32i32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToSI(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		int64_t iout = int32_t(fin);
		s.pop_main();
		s.push_back_main(fif_i32, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32i64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToSI(e->llvm_builder, s.main_ex_back(0), LLVMInt64TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		int64_t iout = int64_t(fin);
		s.pop_main();
		s.push_back_main(fif_i64, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32ui8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToUI(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		int64_t iout = int64_t(uint8_t(fin));
		s.pop_main();
		s.push_back_main(fif_u8, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32ui16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToUI(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		int64_t iout = int64_t(uint16_t(fin));
		s.pop_main();
		s.push_back_main(fif_u16, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32ui32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToUI(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		int64_t iout = int64_t(uint32_t(fin));
		s.pop_main();
		s.push_back_main(fif_u32, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f32ui64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToUI(e->llvm_builder, s.main_ex_back(0), LLVMInt64TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		float fin = 0;
		memcpy(&fin, &v, 4);
		int64_t iout = int64_t(uint64_t(fin));
		s.pop_main();
		s.push_back_main(fif_u64, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u64, 0, nullptr);
	}
	return p + 2;
}

inline int32_t* f64i8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToSI(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		int64_t iout = int8_t(fin);
		s.pop_main();
		s.push_back_main(fif_i8, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64i16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToSI(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		int64_t iout = int16_t(fin);
		s.pop_main();
		s.push_back_main(fif_i16, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64i32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToSI(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		int64_t iout = int32_t(fin);
		s.pop_main();
		s.push_back_main(fif_i32, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64i64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToSI(e->llvm_builder, s.main_ex_back(0), LLVMInt64TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_i64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		int64_t iout = int64_t(fin);
		s.pop_main();
		s.push_back_main(fif_i64, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_i64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64ui8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToUI(e->llvm_builder, s.main_ex_back(0), LLVMInt8TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u8, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		int64_t iout = int64_t(uint8_t(fin));
		s.pop_main();
		s.push_back_main(fif_u8, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u8, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64ui16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToUI(e->llvm_builder, s.main_ex_back(0), LLVMInt16TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u16, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		int64_t iout = int64_t(uint16_t(fin));
		s.pop_main();
		s.push_back_main(fif_u16, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u16, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64ui32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToUI(e->llvm_builder, s.main_ex_back(0), LLVMInt32TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u32, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		int64_t iout = int64_t(uint32_t(fin));
		s.pop_main();
		s.push_back_main(fif_u32, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u32, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* f64ui64(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		auto result = LLVMBuildFPToUI(e->llvm_builder, s.main_ex_back(0), LLVMInt64TypeInContext(e->llvm_context), "");
		s.pop_main();
		s.push_back_main(fif_u64, 0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		auto v = s.main_data_back(0);
		double fin = 0;
		memcpy(&fin, &v, 8);
		int64_t iout = int64_t(uint64_t(fin));
		s.pop_main();
		s.push_back_main(fif_u64, iout, nullptr);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.pop_main();
		s.push_back_main(fif::fif_u64, 0, nullptr);
	}
	return p + 2;
}
inline int32_t* bit_and(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto result = LLVMBuildAnd(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, a & b);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* bit_or(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto result = LLVMBuildOr(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, a | b);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* bit_xor(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto result = LLVMBuildXor(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, a ^ b);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* bit_not(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(1);
		auto result = LLVMBuildNot(e->llvm_builder, s.main_ex_back(0), "");
		s.set_main_ex_back(0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		auto a = s.main_data_back(0);
		if(s.main_type_back(0) == fif_bool)
			s.set_main_data_back(0, a == 0);
		else
			s.set_main_data_back(0, ~a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* bit_shl(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto result = LLVMBuildShl(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, b << a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* bit_ashr(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto result = LLVMBuildAShr(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, b >> a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}
inline int32_t* bit_lshr(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
#ifdef USE_LLVM
		s.mark_used_from_main(2);
		auto result = LLVMBuildLShr(e->llvm_builder, s.main_ex_back(1), s.main_ex_back(0), "");
		s.pop_main();
		s.set_main_ex_back(0, result);
#endif
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(2);
		auto a = s.main_data_back(0);
		auto b = s.main_data_back(1);
		s.pop_main();
		s.set_main_data_back(0, uint64_t(b) >> a);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(2);
		s.pop_main();
	}
	return p + 2;
}

inline int32_t* ident32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FFFFFFFF);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* ident16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FFFF);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* ident8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x0FF);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* idents32(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, int32_t(s.main_data_back(0) & 0x0FFFFFFFF));
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* idents16(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, int16_t(s.main_data_back(0) & 0x0FFFF));
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* idents8(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, int8_t(s.main_data_back(0) & 0x0FF));
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}
inline int32_t* ident1(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(e->mode == fif::fif_mode::compiling_llvm) {
	} else if(e->mode == fif::fif_mode::interpreting) {
		s.mark_used_from_main(1);
		s.set_main_data_back(0, s.main_data_back(0) & 0x01);
	} else if(fif::typechecking_mode(e->mode) && !fif::typechecking_failed(e->mode)) {
		s.mark_used_from_main(1);
	}
	return p + 2;
}

inline void initialize_standard_vocab(environment& fif_env) {
	add_precompiled(fif_env, ":", colon_definition, { });
	add_precompiled(fif_env, ":s", colon_specialization, { });
	add_precompiled(fif_env, "((", long_comment, { }, true);
	add_precompiled(fif_env, "--", line_comment, { }, true);

	add_precompiled(fif_env, "+", iadd, { fif::fif_i32, fif::fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "+", iadd, { fif::fif_u32, fif::fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "+", iadd, { fif::fif_u64, fif::fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "+", iadd, { fif::fif_i64, fif::fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "+", iadd, { fif::fif_i16, fif::fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "+", iadd, { fif::fif_u16, fif::fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "+", iadd, { fif::fif_i8, fif::fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "+", iadd, { fif::fif_u8, fif::fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "+", f32_add, { fif::fif_f32, fif::fif_f32, -1, fif::fif_f32 });
	add_precompiled(fif_env, "+", f64_add, { fif::fif_f64, fif::fif_f64, -1, fif::fif_f64 });

	add_precompiled(fif_env, "-", isub, { fif::fif_i32, fif::fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "-", isub, { fif::fif_u32, fif::fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "-", isub, { fif::fif_u64, fif::fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "-", isub, { fif::fif_i64, fif::fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "-", isub, { fif::fif_i16, fif::fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "-", isub, { fif::fif_u16, fif::fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "-", isub, { fif::fif_i8, fif::fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "-", isub, { fif::fif_u8, fif::fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "-", f32_sub, { fif::fif_f32, fif::fif_f32, -1, fif::fif_f32 });
	add_precompiled(fif_env, "-", f64_sub, { fif::fif_f64, fif::fif_f64, -1, fif::fif_f64 });

	add_precompiled(fif_env, "/", sidiv, { fif::fif_i32, fif::fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "/", uidiv, { fif::fif_u32, fif::fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "/", uidiv, { fif::fif_u64, fif::fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "/", sidiv, { fif::fif_i64, fif::fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "/", sidiv, { fif::fif_i16, fif::fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "/", uidiv, { fif::fif_u16, fif::fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "/", sidiv, { fif::fif_i8, fif::fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "/", uidiv, { fif::fif_u8, fif::fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "/", f32_div, { fif::fif_f32, fif::fif_f32, -1, fif::fif_f32 });
	add_precompiled(fif_env, "/", f64_div, { fif::fif_f64, fif::fif_f64, -1, fif::fif_f64 });

	add_precompiled(fif_env, "mod", simod, { fif::fif_i32, fif::fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "mod", uimod, { fif::fif_u32, fif::fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "mod", uimod, { fif::fif_u64, fif::fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "mod", simod, { fif::fif_i64, fif::fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "mod", simod, { fif::fif_i16, fif::fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "mod", uimod, { fif::fif_u16, fif::fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "mod", simod, { fif::fif_i8, fif::fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "mod", uimod, { fif::fif_u8, fif::fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "mod", f32_mod, { fif::fif_f32, fif::fif_f32, -1, fif::fif_f32 });
	add_precompiled(fif_env, "mod", f64_mod, { fif::fif_f64, fif::fif_f64, -1, fif::fif_f64 });

	add_precompiled(fif_env, "*", imul, { fif::fif_i32, fif::fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "*", imul, { fif::fif_u32, fif::fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "*", imul, { fif::fif_u64, fif::fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "*", imul, { fif::fif_i64, fif::fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "*", imul, { fif::fif_i16, fif::fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "*", imul, { fif::fif_u16, fif::fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "*", imul, { fif::fif_i8, fif::fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "*", imul, { fif::fif_u8, fif::fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "*", f32_mul, { fif::fif_f32, fif::fif_f32, -1, fif::fif_f32 });
	add_precompiled(fif_env, "*", f64_mul, { fif::fif_f64, fif::fif_f64, -1, fif::fif_f64 });

	add_precompiled(fif_env, "<", ilt, { fif::fif_i32, fif::fif_i32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", ile, { fif::fif_i32, fif::fif_i32, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", igt, { fif::fif_i32, fif::fif_i32, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", ige, { fif::fif_i32, fif::fif_i32, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", ieq, { fif::fif_i32, fif::fif_i32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_i32, fif::fif_i32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<", ilt, { fif::fif_i64, fif::fif_i64, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", ile, { fif::fif_i64, fif::fif_i64, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", igt, { fif::fif_i64, fif::fif_i64, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", ige, { fif::fif_i64, fif::fif_i64, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", ieq, { fif::fif_i64, fif::fif_i64, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_i64, fif::fif_i64, -1, fif::fif_bool });
	add_precompiled(fif_env, "<", ilt, { fif::fif_i16, fif::fif_i16, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", ile, { fif::fif_i16, fif::fif_i16, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", igt, { fif::fif_i16, fif::fif_i16, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", ige, { fif::fif_i16, fif::fif_i16, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", ieq, { fif::fif_i16, fif::fif_i16, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_i16, fif::fif_i16, -1, fif::fif_bool });
	add_precompiled(fif_env, "<", ilt, { fif::fif_i8, fif::fif_i8, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", ile, { fif::fif_i8, fif::fif_i8, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", igt, { fif::fif_i8, fif::fif_i8, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", ige, { fif::fif_i8, fif::fif_i8, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", ieq, { fif::fif_i8, fif::fif_i8, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_i8, fif::fif_i8, -1, fif::fif_bool });

	add_precompiled(fif_env, "<", uilt, { fif::fif_u32, fif::fif_u32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", uile, { fif::fif_u32, fif::fif_u32, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", uigt, { fif::fif_u32, fif::fif_u32, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", uige, { fif::fif_u32, fif::fif_u32, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", ieq, { fif::fif_u32, fif::fif_u32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_u32, fif::fif_u32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<", uilt, { fif::fif_u64, fif::fif_u64, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", uile, { fif::fif_u64, fif::fif_u64, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", uigt, { fif::fif_u64, fif::fif_u64, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", uige, { fif::fif_u64, fif::fif_u64, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", ieq, { fif::fif_u64, fif::fif_u64, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_u64, fif::fif_u64, -1, fif::fif_bool });
	add_precompiled(fif_env, "<", uilt, { fif::fif_u16, fif::fif_u16, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", uile, { fif::fif_u16, fif::fif_u16, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", uigt, { fif::fif_u16, fif::fif_u16, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", uige, { fif::fif_u16, fif::fif_u16, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", ieq, { fif::fif_u16, fif::fif_u16, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_u16, fif::fif_u16, -1, fif::fif_bool });
	add_precompiled(fif_env, "<", uilt, { fif::fif_u8, fif::fif_u8, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", uile, { fif::fif_u8, fif::fif_u8, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", uigt, { fif::fif_u8, fif::fif_u8, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", uige, { fif::fif_u8, fif::fif_u8, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", ieq, { fif::fif_u8, fif::fif_u8, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_u8, fif::fif_u8, -1, fif::fif_bool });

	add_precompiled(fif_env, "<", f32lt, { fif::fif_f32, fif::fif_f32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", f32le, { fif::fif_f32, fif::fif_f32, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", f32gt, { fif::fif_f32, fif::fif_f32, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", f32ge, { fif::fif_f32, fif::fif_f32, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", f32eq, { fif::fif_f32, fif::fif_f32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", f32ne, { fif::fif_f32, fif::fif_f32, -1, fif::fif_bool });
	add_precompiled(fif_env, "<", f64lt, { fif::fif_f64, fif::fif_f64, -1, fif::fif_bool });
	add_precompiled(fif_env, "<=", f64le, { fif::fif_f64, fif::fif_f64, -1, fif::fif_bool });
	add_precompiled(fif_env, ">", f64gt, { fif::fif_f64, fif::fif_f64, -1, fif::fif_bool });
	add_precompiled(fif_env, ">=", f64ge, { fif::fif_f64, fif::fif_f64, -1, fif::fif_bool });
	add_precompiled(fif_env, "=", f64eq, { fif::fif_f64, fif::fif_f64, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", f64ne, { fif::fif_f64, fif::fif_f64, -1, fif::fif_bool });

	add_precompiled(fif_env, "=", ieq, { fif::fif_bool, fif::fif_bool, -1, fif::fif_bool });
	add_precompiled(fif_env, "<>", ine, { fif::fif_bool, fif::fif_bool, -1, fif::fif_bool });

	add_precompiled(fif_env, ">i64", zext_i64, { fif::fif_u64, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", zext_i64, { fif::fif_u32, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", zext_i64, { fif::fif_u16, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", zext_i64, { fif::fif_u8, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", nop1, { fif::fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", sext_i64, { fif::fif_i32, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", sext_i64, { fif::fif_i16, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", sext_i64, { fif::fif_i8, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", zext_i64, { fif::fif_bool, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", f32i64, { fif::fif_f32, -1, fif::fif_i64 });
	add_precompiled(fif_env, ">i64", f64i64, { fif::fif_f64, -1, fif::fif_i64 });

	add_precompiled(fif_env, ">u64", nop1, { fif::fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", zext_ui64, { fif::fif_u32, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", zext_ui64, { fif::fif_u16, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", zext_ui64, { fif::fif_u8, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", sext_ui64, { fif::fif_i64, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", sext_ui64, { fif::fif_i32, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", sext_ui64, { fif::fif_i16, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", sext_ui64, { fif::fif_i8, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", zext_ui64, { fif::fif_bool, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", f32ui64, { fif::fif_f32, -1, fif::fif_u64 });
	add_precompiled(fif_env, ">u64", f64ui64, { fif::fif_f64, -1, fif::fif_u64 });

	add_precompiled(fif_env, ">i32", trunc_i32, { fif::fif_u64, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", zext_i32, { fif::fif_u32, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", zext_i32, { fif::fif_u16, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", zext_i32, { fif::fif_u8, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", trunc_i32, { fif::fif_i64, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", idents32, { fif::fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", sext_i32, { fif::fif_i16, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", sext_i32, { fif::fif_i8, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", zext_i32, { fif::fif_bool, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", f32i32, { fif::fif_f32, -1, fif::fif_i32 });
	add_precompiled(fif_env, ">i32", f64i32, { fif::fif_f64, -1, fif::fif_i32 });

	add_precompiled(fif_env, ">u32", trunc_ui32, { fif::fif_u64, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", ident32, { fif::fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", zext_ui32, { fif::fif_u16, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", zext_ui32, { fif::fif_u8, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", trunc_ui32, { fif::fif_i64, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", sext_ui32, { fif::fif_i32, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", sext_ui32, { fif::fif_i16, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", sext_ui32, { fif::fif_i8, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", zext_ui32, { fif::fif_bool, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", f32ui32, { fif::fif_f32, -1, fif::fif_u32 });
	add_precompiled(fif_env, ">u32", f64ui32, { fif::fif_f64, -1, fif::fif_u32 });

	add_precompiled(fif_env, ">i16", trunc_i16, { fif::fif_u64, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", trunc_i16, { fif::fif_u32, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", zext_i16, { fif::fif_u16, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", zext_i16, { fif::fif_u8, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", trunc_i16, { fif::fif_i64, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", trunc_i16, { fif::fif_i32, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", idents16, { fif::fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", sext_i16, { fif::fif_i8, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", zext_i16, { fif::fif_bool, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", f32i16, { fif::fif_f32, -1, fif::fif_i16 });
	add_precompiled(fif_env, ">i16", f64i16, { fif::fif_f64, -1, fif::fif_i16 });

	add_precompiled(fif_env, ">u16", trunc_ui16, { fif::fif_u64, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", trunc_ui16, { fif::fif_u32, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", ident16, { fif::fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", zext_ui16, { fif::fif_u8, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", trunc_ui16, { fif::fif_i64, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", trunc_ui16, { fif::fif_i32, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", sext_ui16, { fif::fif_i16, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", sext_ui16, { fif::fif_i8, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", zext_ui16, { fif::fif_bool, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", f32ui16, { fif::fif_f32, -1, fif::fif_u16 });
	add_precompiled(fif_env, ">u16", f64ui16, { fif::fif_f64, -1, fif::fif_u16 });

	add_precompiled(fif_env, ">i8", trunc_i8, { fif::fif_u64, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", trunc_i8, { fif::fif_u32, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", trunc_i8, { fif::fif_u16, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", zext_i8, { fif::fif_u8, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", trunc_i8, { fif::fif_i64, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", trunc_i8, { fif::fif_i32, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", trunc_i8, { fif::fif_i16, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", idents8, { fif::fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", zext_i8, { fif::fif_bool, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", f32i8, { fif::fif_f32, -1, fif::fif_i8 });
	add_precompiled(fif_env, ">i8", f64i8, { fif::fif_f64, -1, fif::fif_i8 });

	add_precompiled(fif_env, ">u8", trunc_ui8, { fif::fif_u64, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", trunc_ui8, { fif::fif_u32, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", trunc_ui8, { fif::fif_u16, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", ident8, { fif::fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", trunc_ui8, { fif::fif_i64, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", trunc_ui8, { fif::fif_i32, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", trunc_ui8, { fif::fif_i16, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", sext_ui8, { fif::fif_i8, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", zext_ui8, { fif::fif_bool, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", f32ui8, { fif::fif_f32, -1, fif::fif_u8 });
	add_precompiled(fif_env, ">u8", f64ui8, { fif::fif_f64, -1, fif::fif_u8 });

	add_precompiled(fif_env, ">f32", uif32, { fif::fif_u64, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", uif32, { fif::fif_u32, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", uif32, { fif::fif_u16, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", uif32, { fif::fif_u8, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", sif32, { fif::fif_i64, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", sif32, { fif::fif_i32, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", sif32, { fif::fif_i16, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", sif32, { fif::fif_i8, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", nop1, { fif::fif_f32, -1, fif::fif_f32 });
	add_precompiled(fif_env, ">f32", ftrunc, { fif::fif_f64, -1, fif::fif_f32 });

	add_precompiled(fif_env, ">f64", uif64, { fif::fif_u64, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", uif64, { fif::fif_u32, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", uif64, { fif::fif_u16, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", uif64, { fif::fif_u8, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", sif64, { fif::fif_i64, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", sif64, { fif::fif_i32, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", sif64, { fif::fif_i16, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", sif64, { fif::fif_i8, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", fext, { fif::fif_f32, -1, fif::fif_f64 });
	add_precompiled(fif_env, ">f64", nop1, { fif::fif_f64, -1, fif::fif_f64 });

	add_precompiled(fif_env, ">bool", trunc_i1, { fif::fif_u64, -1, fif::fif_bool });
	add_precompiled(fif_env, ">bool", trunc_i1, { fif::fif_u32, -1, fif::fif_bool });
	add_precompiled(fif_env, ">bool", trunc_i1, { fif::fif_u16, -1, fif::fif_bool });
	add_precompiled(fif_env, ">bool", trunc_i1, { fif::fif_u8, -1, fif::fif_bool });
	add_precompiled(fif_env, ">bool", trunc_i1, { fif::fif_i64, -1, fif::fif_bool });
	add_precompiled(fif_env, ">bool", trunc_i1, { fif::fif_i32, -1, fif::fif_bool });
	add_precompiled(fif_env, ">bool", trunc_i1, { fif::fif_i16, -1, fif::fif_bool });
	add_precompiled(fif_env, ">bool", trunc_i1, { fif::fif_i8, -1, fif::fif_bool });
	add_precompiled(fif_env, ">bool", ident1, { fif::fif_bool, -1, fif::fif_bool });

	add_precompiled(fif_env, "and", bit_and, { fif::fif_u64, fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "and", bit_and, { fif::fif_u32, fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "and", bit_and, { fif::fif_u16, fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "and", bit_and, { fif::fif_u8, fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "and", bit_and, { fif::fif_i64, fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "and", bit_and, { fif::fif_i32, fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "and", bit_and, { fif::fif_i16, fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "and", bit_and, { fif::fif_i8, fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "and", bit_and, { fif::fif_bool, fif_bool, -1, fif::fif_bool });

	add_precompiled(fif_env, "or", bit_or, { fif::fif_u64, fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "or", bit_or, { fif::fif_u32, fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "or", bit_or, { fif::fif_u16, fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "or", bit_or, { fif::fif_u8, fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "or", bit_or, { fif::fif_i64, fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "or", bit_or, { fif::fif_i32, fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "or", bit_or, { fif::fif_i16, fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "or", bit_or, { fif::fif_i8, fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "or", bit_or, { fif::fif_bool, fif_bool, -1, fif::fif_bool });

	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_u64, fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_u32, fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_u16, fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_u8, fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_i64, fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_i32, fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_i16, fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_i8, fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "xor", bit_xor, { fif::fif_bool, fif_bool, -1, fif::fif_bool });

	add_precompiled(fif_env, "not", bit_not, { fif::fif_u64, -1, fif::fif_u64 });
	add_precompiled(fif_env, "not", bit_not, { fif::fif_u32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "not", bit_not, { fif::fif_u16, -1, fif::fif_u16 });
	add_precompiled(fif_env, "not", bit_not, { fif::fif_u8, -1, fif::fif_u8 });
	add_precompiled(fif_env, "not", bit_not, { fif::fif_i64, -1, fif::fif_i64 });
	add_precompiled(fif_env, "not", bit_not, { fif::fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "not", bit_not, { fif::fif_i16, -1, fif::fif_i16 });
	add_precompiled(fif_env, "not", bit_not, { fif::fif_i8, -1, fif::fif_i8 });
	add_precompiled(fif_env, "not", bit_not, { fif::fif_bool, -1, fif::fif_bool });

	add_precompiled(fif_env, "shl", bit_shl, { fif::fif_u64, fif_i32, -1, fif::fif_u64 });
	add_precompiled(fif_env, "shl", bit_shl, { fif::fif_i64, fif_i32, -1, fif::fif_i64 });
	add_precompiled(fif_env, "shl", bit_shl, { fif::fif_u32, fif_i32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "shl", bit_shl, { fif::fif_i32, fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "shl", bit_shl, { fif::fif_u16, fif_i32, -1, fif::fif_u16 });
	add_precompiled(fif_env, "shl", bit_shl, { fif::fif_i16, fif_i32, -1, fif::fif_i16 });
	add_precompiled(fif_env, "shl", bit_shl, { fif::fif_u8, fif_i32, -1, fif::fif_u8 });
	add_precompiled(fif_env, "shl", bit_shl, { fif::fif_i8, fif_i32, -1, fif::fif_i8 });

	add_precompiled(fif_env, "shr", bit_lshr, { fif::fif_u64, fif_i32, -1, fif::fif_u64 });
	add_precompiled(fif_env, "shr", bit_ashr, { fif::fif_i64, fif_i32, -1, fif::fif_i64 });
	add_precompiled(fif_env, "shr", bit_lshr, { fif::fif_u32, fif_i32, -1, fif::fif_u32 });
	add_precompiled(fif_env, "shr", bit_ashr, { fif::fif_i32, fif_i32, -1, fif::fif_i32 });
	add_precompiled(fif_env, "shr", bit_lshr, { fif::fif_u16, fif_i32, -1, fif::fif_u16 });
	add_precompiled(fif_env, "shr", bit_ashr, { fif::fif_i16, fif_i32, -1, fif::fif_i16 });
	add_precompiled(fif_env, "shr", bit_lshr, { fif::fif_u8, fif_i32, -1, fif::fif_u8 });
	add_precompiled(fif_env, "shr", bit_ashr, { fif::fif_i8, fif_i32, -1, fif::fif_i8 });

	add_precompiled(fif_env, "init", init, { -2, -1, -2 });
	add_precompiled(fif_env, "dup", dup, { -2, -1, -2, -2 });
	add_precompiled(fif_env, "copy", copy, { -2, -1, -2, -2 });
	add_precompiled(fif_env, "drop", drop, { -2 });
	add_precompiled(fif_env, "swap", fif_swap, { -2, -3, -1, -2, -3 });
	add_precompiled(fif_env, "if", fif_if, { }, true);
	add_precompiled(fif_env, "else", fif_else, { }, true);
	add_precompiled(fif_env, "then", fif_then, { }, true);
	add_precompiled(fif_env, "end-if", fif_then, { }, true);
	add_precompiled(fif_env, "while", fif_while, { }, true);
	add_precompiled(fif_env, "loop", fif_loop, { }, true);
	add_precompiled(fif_env, "end-while", fif_end_while, { }, true);
	add_precompiled(fif_env, "do", fif_do, { }, true);
	add_precompiled(fif_env, "until", fif_until, { }, true);
	add_precompiled(fif_env, "end-do", fif_end_do, { fif::fif_bool }, true);
	add_precompiled(fif_env, ">r", to_r, { -2, -1, -1, -1, -2 });
	add_precompiled(fif_env, "r>", from_r, { -1, -2, -1, -2 });
	add_precompiled(fif_env, "immediate", make_immediate, {  });
	add_precompiled(fif_env, "[", open_bracket, { }, true);
	add_precompiled(fif_env, "]", close_bracket, { }, true);
	add_precompiled(fif_env, "ptr-cast", pointer_cast, { fif_opaque_ptr }, true);
	add_precompiled(fif_env, "ptr-cast", pointer_cast, { fif_ptr, std::numeric_limits<int32_t>::max(), -2, -1 }, true);
	add_precompiled(fif_env, "heap-alloc", impl_heap_allot, { -2, -1, fif_ptr, std::numeric_limits<int32_t>::max(), -2, -1 });
	add_precompiled(fif_env, "heap-free", impl_heap_free, { fif_ptr, std::numeric_limits<int32_t>::max(), -2, -1, -1, -2 });
	add_precompiled(fif_env, "@", impl_load, { fif_ptr, std::numeric_limits<int32_t>::max(), -2, -1, -1, -2 });
	add_precompiled(fif_env, "@@", impl_load_deallocated, { fif_ptr, std::numeric_limits<int32_t>::max(), -2, -1, -1, -2 });
	add_precompiled(fif_env, "!", impl_store, { fif_ptr, std::numeric_limits<int32_t>::max(), -2, -1, -2 });
	add_precompiled(fif_env, "!!", impl_uninit_store, { fif_ptr, std::numeric_limits<int32_t>::max(), -2, -1, -2 });
	add_precompiled(fif_env, "sizeof", impl_sizeof, { -1, fif_i32 }, true);
	add_precompiled(fif_env, "buf-free", free_buffer, { fif_opaque_ptr });
	add_precompiled(fif_env, "buf-alloc", allocate_buffer, { fif_i32, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-alloc", allocate_buffer, { fif_i64, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-alloc", allocate_buffer, { fif_i16, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-alloc", allocate_buffer, { fif_i8, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-copy", copy_buffer, { fif_i32, fif_opaque_ptr, fif_opaque_ptr, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-copy", copy_buffer, { fif_i64, fif_opaque_ptr, fif_opaque_ptr, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-copy", copy_buffer, { fif_i16, fif_opaque_ptr, fif_opaque_ptr, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-copy", copy_buffer, { fif_i8, fif_opaque_ptr, fif_opaque_ptr, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-add", impl_index, { fif_opaque_ptr, fif_i32, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-add", impl_index, { fif_opaque_ptr, fif_i64, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-add", impl_index, { fif_opaque_ptr, fif_i16, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "buf-add", impl_index, { fif_opaque_ptr, fif_i8, -1, fif_opaque_ptr });
	add_precompiled(fif_env, "let", create_let, { -2 }, true);
	add_precompiled(fif_env, "var", create_var, { -2 }, true);
	add_precompiled(fif_env, "global", create_global_impl, { fif_type });
	add_precompiled(fif_env, "forth.insert", forth_insert, { }, true);
	add_precompiled(fif_env, "forth.extract", forth_extract, { }, true);
	add_precompiled(fif_env, "forth.extract-copy", forth_extract_copy, { }, true);
	add_precompiled(fif_env, "forth.gep", forth_gep, { }, true);
	add_precompiled(fif_env, "struct-map2", forth_struct_map_two, { }, true);
	add_precompiled(fif_env, "struct-map1", forth_struct_map_one, { }, true);
	add_precompiled(fif_env, "struct-map0", forth_struct_map_zero, { }, true);
	add_precompiled(fif_env, "make", do_make, { }, true);
	add_precompiled(fif_env, ":struct", struct_definition, { });
	add_precompiled(fif_env, ":export", export_definition, { });
	add_precompiled(fif_env, "use-base", do_use_base, { }, true);
	add_precompiled(fif_env, "select", f_select, { fif_bool, -2, -2, -1, -2 });

	auto preinterpreted =
		": r@ r> dup >r ; "
		": over >r dup r> swap ; "
		": nip >r drop r> ; "
		": tuck swap over ; "
		": 2dup >r dup r> dup >r swap r> ; "
		": buf-resize " // ptr old new -> ptr
		"	buf-alloc swap >r >r dup r> r> buf-copy swap buf-free "
		" ; "
		":struct dy-array-block ptr(nil) memory i32 size i32 capacity i32 refcount ; "
		":struct dy-array ptr(dy-array-block) ptr $ 1 ; "
		":s init dy-array($0) s: " // array -> array
		"	sizeof dy-array-block buf-alloc ptr-cast ptr(dy-array-block) swap .ptr! "
		" ; "
		":s dup dy-array($0) s: " // array -> array, array
		"	use-base dup .ptr@ " // original copy ptr-to-block
		"	.refcount dup @ 1 +  swap ! " // increase ref count
		" ; "
		":s drop dy-array($0) s: .ptr@ " // array ->
		"	.refcount dup @ -1 + " // decrement refcount
		"	dup 0 >= if "
		"		swap ! " // store refcount back into pointer
		"	else "
		"		drop drop " // drop pointer to refcount and -1
		"		.ptr@ dup .size let sz .memory let mem " // grab values
		"		while "
		"			sz @ 0 > "
		"		loop "
		"			sz @ -1 + sz ! " // reduce sz
		"			sz @ sizeof $0 * mem @ buf-add ptr-cast ptr($0) @@ " // copy last value, dupless
		"			drop " // run its destructor
		"		end-while "
		"		mem @ buf-free " // free managed buffer
		"		.ptr@ ptr-cast ptr(nil) buf-free " // destroy control block
		"	end-if "
		"	use-base drop "
		" ; "
		":s push dy-array($0) $0 s: " // array , value -> array
		"	let val " // store value to be saved in val
		"	.ptr@ "
		"	dup dup .size let sz .capacity let cap .memory let mem" // destructure
		"	sz @ sizeof $0 * cap @ >= if " // size >= capacity ?
		"		mem @ " // put old buffer on stack
		"		cap @ sizeof $0 * " // old size
		"		cap @ 1 + 2 * sizeof $0 * " // new size
		"		dup cap ! " // copy new size to capacity
		"		buf-resize "
		"		mem ! "
		"	end-if "
		"	sz @ sizeof $0 * mem @ buf-add ptr-cast ptr($0) val swap !! " // move value into last position
		"	sz @ 1 + sz ! " // +1 to stored size
		" ; "
		":s pop dy-array($0) s: " // array -> array , value
		"	.ptr@ " // ptr to control block on stack
		"	dup .size let sz .memory let mem" // destructure
		"	sz @ 0 > if "
		"		sz @ -1 + sz ! " // reduce sz
		"		sz @ sizeof $0 * mem @ buf-add ptr-cast ptr($0) @@ " // copy last value, dupless
		"	else "
		"		make $0 " // nothing left, make new value
		"	end-if "
		" ; "
		":s empty? dy-array($0) s: " // array -> array, value
		"	.ptr@ .size @ <= 0 "
		" ; "
		":s index-into dy-array($0) i32 s:" // array, int -> array, ptr(0)
		"	let index .ptr@ .memory @ sizeof $0 index * swap buf-add ptr-cast ptr($0) "
		" ; "
		":s size dy-array($0) s:" // array -> array, int
		"	.ptr@ .size @ "
		" ; "
		;

	fif::interpreter_stack values{ };
	fif::run_fif_interpreter(fif_env, preinterpreted, values);

}

}
