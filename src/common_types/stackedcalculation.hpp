#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <utility>

namespace sys {

	// Operation types
	enum class Operation {
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE
	};

	// Structure to hold each calculation step
	struct CalculationStep {
		Operation operation;
		float value;
		std::string_view explanation;

		CalculationStep(Operation op, float val, std::string_view exp)
			: operation(op), value(val), explanation(exp) { }
	};

	// -------------------------------------------------------------------
	// Step nodes – each node represents one operation and stores the rest
	// of the steps as a nested structure.
	// -------------------------------------------------------------------

	// Empty step list
	struct NoStep { };

	// A single step node holds the operation type as a non-type template parameter,
	// the operand value, the explanation, and the remaining steps.
	template<Operation Op, typename Next>
	struct StepNode {
		float value;
		std::string_view explanation;
		Next next;

		StepNode(float v, std::string_view exp, Next n)
			: value(v), explanation(exp), next(std::move(n)) { }
	};

	// US49 stacked_calculation class allows uniting number calculations for backend with explanation tooltips for the UI
	template<typename Steps = NoStep>
	class stacked_calculation {
	private:
		float initialValue;
		Steps steps;

		// Apply a single operation to a value
		static float apply(Operation op, float current, float val) {
			switch(op) {
			case Operation::ADD:      return current + val;
			case Operation::SUBTRACT: return current - val;
			case Operation::MULTIPLY: return current * val;
			case Operation::DIVIDE:
				if(val == 0.0f) throw std::runtime_error("Division by zero");
				return current / val;
			}
			return current; // unreachable
		}

		// Evaluation helpers
		static float evaluate_impl(const NoStep&, float current) {
			return current;
		}

		template<Operation Op, typename Next>
		static float evaluate_impl(const StepNode<Op, Next>& step, float current) {
			float newCurrent = apply(Op, current, step.value);
			return evaluate_impl(step.next, newCurrent);
		}

		// First collect all previous steps, then add the current one (so the order is preserved).
		static void collect_steps(const NoStep&, std::vector<CalculationStep>&) { }

		template<Operation Op, typename Next>
		static void collect_steps(const StepNode<Op, Next>& step,
								  std::vector<CalculationStep>& out) {
			collect_steps(step.next, out);
			out.emplace_back(Op, step.value, step.explanation);
		}

	public:
		// US49AC1 stacked_calculation is constructed with initial float value (default 0.0f) and no steps
		explicit stacked_calculation(float init = 0.0f)
			: initialValue(init), steps() { }

		// Internal constructor used when appending a step.
		stacked_calculation(float init, Steps s)
			: initialValue(init), steps(std::move(s)) { }

		// US49AC2 User can add a value with a string_view explanation of the reason
		auto add(float value, std::string_view explanation) const {
			auto newSteps = StepNode<Operation::ADD, Steps>(value, explanation, steps);
			return stacked_calculation<decltype(newSteps)>(
				initialValue, std::move(newSteps));
		}
		// US49AC3 User can subtract a value with a string_view explanation of the reason
		auto subtract(float value, std::string_view explanation) const {
			auto newSteps = StepNode<Operation::SUBTRACT, Steps>(value, explanation, steps);
			return stacked_calculation<decltype(newSteps)>(
				initialValue, std::move(newSteps));
		}
		// US49AC4 User can multiply current value by provided value with a string_view explanation of the reason
		auto multiply(float value, std::string_view explanation) const {
			auto newSteps = StepNode<Operation::MULTIPLY, Steps>(value, explanation, steps);
			return stacked_calculation<decltype(newSteps)>(
				initialValue, std::move(newSteps));
		}
		// US49AC5 User can divide current value by provided value with a string_view explanation of the reason
		auto divide(float value, std::string_view explanation) const {
			// Division‑by‑zero check is deferred until evaluation (lazy).
			auto newSteps = StepNode<Operation::DIVIDE, Steps>(value, explanation, steps);
			return stacked_calculation<decltype(newSteps)>(
				initialValue, std::move(newSteps));
		}

		// US49AC5 User can get resulting value from the stack
		float getResult() const {
			return evaluate_impl(steps, initialValue);
		}
		// US49AC6 User can get all steps used for calculation for tooltipsand UI
		std::vector<CalculationStep> getSteps() const {
			std::vector<CalculationStep> result;
			collect_steps(steps, result);
			return result;
		}

		// US49AC7 User can reuse existing stack by clearing it
		auto clear() const {
			return stacked_calculation<NoStep>(initialValue);
		}
		// US49AC8 User can reuse existing stack by reseting it to a different initial value
		auto reset(float newInitialValue) const {
			return stacked_calculation<NoStep>(newInitialValue);
		}
	};


}
