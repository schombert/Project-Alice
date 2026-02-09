#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <memory>

namespace sys {

	// US49 StackedCalculationWithExplanations allows uniting number calculations for backend with explanation tooltips for the UI
	class StackedCalculationWithExplanations {
	public:
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

	private:
		float currentValue;
		std::vector<CalculationStep> calculationSteps;

	public:
		// US49AC1 StackedCalculationWithExplanations is constructed with initial float value
		StackedCalculationWithExplanations(float initialValue = 0.0f)
			: currentValue(initialValue) {

		}

		// US49AC2 User can add a value with a string_view explanation of the reason
		void add(float value, const std::string_view& explanation) {
			calculationSteps.emplace_back(
				Operation::ADD, value, explanation
			);
		}

		// US49AC3 User can subtract a value with a string_view explanation of the reason
		void subtract(float value, const std::string_view& explanation) {
			calculationSteps.emplace_back(
				Operation::SUBTRACT, value, explanation
			);
		}

		// US49AC4 User can multiply current value by provided value with a string_view explanation of the reason
		void multiply(float value, const std::string_view& explanation) {
			calculationSteps.emplace_back(
				Operation::MULTIPLY, value, explanation
			);
		}

		// US49AC4 User can divide current value by provided value with a string_view explanation of the reason
		void divide(float value, const std::string_view& explanation) {
			if(value == 0) {
				throw std::runtime_error("Division by zero");
			}
			calculationSteps.emplace_back(
				Operation::DIVIDE, value, explanation
			);
		}

		// US49AC5 User can get resulting value from the stack
		float getResult() const {
			auto val = currentValue;
			for(auto step : calculationSteps) {
				if(step.operation == Operation::ADD) {
					val += step.value;
				} else if(step.operation == Operation::SUBTRACT) {
					val -= step.value;
				} else if(step.operation == Operation::MULTIPLY) {
					val *= step.value;
				} else if(step.operation == Operation::DIVIDE) {
					val /= step.value;
				}
			}
			return val;
		}

		// US49AC6 User can get all steps used for calculation for tooltipsand UI
		const std::vector<CalculationStep>& getSteps() const {
			return calculationSteps;
		}

		// US49AC7 User can reuse existing stack by clearing it
		void clear() {
			calculationSteps.clear();
		}

		// US49AC8 User can reuse existing stack by reseting it to a different initial value
		void reset(float newInitialValue) {
			currentValue = newInitialValue;
			calculationSteps.clear();
		}

	private:
		// Helper to get symbol for operation
		std::string_view getOperationSymbol(Operation op) const {
			switch(op) {
			case Operation::ADD: return "+";
			case Operation::SUBTRACT: return "-";
			case Operation::MULTIPLY: return "×";
			case Operation::DIVIDE: return "÷";
			default: return "?";
			}
		}
	};

}
