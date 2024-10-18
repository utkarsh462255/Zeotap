# Rule Engine with C++ and MongoDB

This project implements a simple 3-tier rule engine application using C++ and MongoDB. The application evaluates user eligibility based on various attributes like age, department, income, and more. It uses an Abstract Syntax Tree (AST) to represent conditional rules and allows for the dynamic creation, combination, and modification of these rules.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Setup Instructions](#setup-instructions)
- [Project Structure](#project-structure)
- [Usage](#usage)
- [Testing](#testing)
- [Future Enhancements](#future-enhancements)

## Features

- **AST Representation:** The application uses an AST data structure to represent and manipulate rules.
- **Dynamic Rule Creation and Combination:** Supports creating individual rules and combining multiple rules.
- **MongoDB Integration:** Rules are stored in MongoDB, allowing for persistent storage and retrieval.
- **Rule Evaluation:** Evaluates the rules against user data to determine eligibility.
- **Error Handling:** Handles cases where rules are not found or data does not match the expected format.

## Prerequisites

- **C++ Compiler:** A modern C++ compiler that supports C++17 or later.
- **MongoDB:** A running instance of MongoDB (local or remote).
- **MongoDB C++ Driver:** The MongoDB C++ driver (`mongocxx` and `bsoncxx`) must be installed.

## Setup Instructions

1. **Install MongoDB:** Follow the [MongoDB installation guide](https://www.mongodb.com/docs/manual/installation/) for your operating system.

2. **Install MongoDB C++ Driver:** Follow the [installation instructions](https://mongocxx.org/mongocxx-v3/installation/) to install the MongoDB C++ driver (`mongocxx`).

3. **Clone the Repository:**

   ```bash
   git clone <repository-url>
   cd rule-engine-cpp

   ```

4. **Compile the Code:** Make sure you link the MongoDB C++ driver libraries when compiling. An example compilation command might look like:

   ```bash
   g++ -o rule_engine main.cpp -std=c++17 -lmongocxx -lbsoncxx

   ```

5. **Run the Application:**
   ```bash
   ./rule_engine
   ```

## Project Structure

main.cpp: Contains the main application logic, including rule creation, combination, evaluation, and MongoDB interaction.
Node: Represents the AST node structure used for rules.
RuleEngineDB: Handles MongoDB interactions for saving and loading rules.
AST Evaluation: Provides logic to evaluate the rules against given user data.

## Usage

- **Creating a Rule:** Use the createRuleAST function to create a rule's AST representation. This function currently creates a sample AST manually.

- **Combining Rules:** Use the combineRules function to combine multiple rule ASTs into a single AST.

- **Saving Rules:** The saveRule method of the RuleEngineDB class stores the AST in MongoDB.

- **Loading Rules:** The loadRule method retrieves the AST from MongoDB by the rule name.

- **Evaluating Rules:** Use the evaluateAST function to check if the user data matches the specified rule conditions.


## Testing

- **Creating and Saving Rules:** The code demonstrates creating an AST for a rule, storing it in MongoDB, and retrieving it.
- **Combining Rules:** Combines two or more rule ASTs using the "AND" operator.
- **Evaluating Rules:** Tests different user data scenarios to check if they match the rule conditions.


## Future Enhancements

- **Improved Rule Parsing:** Implement a real parser to convert rule strings into ASTs.
- **Advanced Condition Handling:** Support more complex condition evaluations and attribute types.
- **Enhanced Error Handling:** Add detailed error handling for invalid rule formats, database failures, and data inconsistencies.
- **User-Defined Functions:** Extend the system to support user-defined functions for more advanced conditions.