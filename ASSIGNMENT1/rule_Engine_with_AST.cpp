#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

// NodeType: Define the type of node (Operator or Operand)
enum class NodeType {
    OPERATOR,
    OPERAND
};

// Node: Represents a node in the AST
struct Node {
    NodeType type;
    std::string value; // Used for operand nodes to hold the condition (e.g., "age > 30")
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Node(NodeType t, const std::string& val = "") : type(t), value(val), left(nullptr), right(nullptr) {}
};

// Function to create a JSON representation of a Node for MongoDB storage
bsoncxx::document::value toBSON(const std::shared_ptr<Node>& node) {
    using namespace bsoncxx::builder::stream;
    document doc{};

    doc << "type" << (node->type == NodeType::OPERATOR ? "operator" : "operand")
        << "value" << node->value;

    if (node->left) {
        doc << "left" << toBSON(node->left);
    }
    if (node->right) {
        doc << "right" << toBSON(node->right);
    }
    
    return doc << finalize;
}

// Class for MongoDB database interaction
class RuleEngineDB {
public:
    RuleEngineDB() {
        mongocxx::instance instance{};
        mongocxx::uri uri("mongodb://localhost:27017");
        client = mongocxx::client(uri);
        db = client["rule_engine"];
    }

    // Store the rule AST in MongoDB
    void saveRule(const std::string& rule_name, const std::shared_ptr<Node>& root) {
        auto collection = db["rules"];
        bsoncxx::document::value doc = toBSON(root);
        collection.insert_one(
            bsoncxx::builder::stream::document{} << "rule_name" << rule_name << "ast" << doc << bsoncxx::builder::stream::finalize
        );
        std::cout << "Rule saved successfully!" << std::endl;
    }

    // Retrieve a rule AST from MongoDB
    std::shared_ptr<Node> loadRule(const std::string& rule_name) {
        auto collection = db["rules"];
        auto doc = collection.find_one(
            bsoncxx::builder::stream::document{} << "rule_name" << rule_name << bsoncxx::builder::stream::finalize
        );

        if (doc) {
            return parseBSON(doc->view()["ast"].get_document().view());
        } else {
            std::cerr << "Rule not found!" << std::endl;
            return nullptr;
        }
    }

private:
    mongocxx::client client;
    mongocxx::database db;

    // Helper function to parse BSON to AST
    std::shared_ptr<Node> parseBSON(const bsoncxx::document::view& doc) {
        NodeType type = doc["type"].get_utf8().value.to_string() == "operator" ? NodeType::OPERATOR : NodeType::OPERAND;
        auto node = std::make_shared<Node>(type, doc["value"].get_utf8().value.to_string());

        if (doc.find("left") != doc.end()) {
            node->left = parseBSON(doc["left"].get_document().view());
        }
        if (doc.find("right") != doc.end()) {
            node->right = parseBSON(doc["right"].get_document().view());
        }
        return node;
    }
};

// Function to create a rule AST from a rule string (simplified for illustration)
std::shared_ptr<Node> createRuleAST(const std::string& rule) {
    // This function should parse the rule string and create an AST.
    // For now, we assume rules are in simple form.
    // In a real implementation, this would involve parsing the string properly.

    // Placeholder: Creating a dummy AST manually for illustration
    auto root = std::make_shared<Node>(NodeType::OPERATOR, "AND");
    root->left = std::make_shared<Node>(NodeType::OPERAND, "age > 30");
    root->right = std::make_shared<Node>(NodeType::OPERAND, "department = 'Sales'");
    return root;
}

// Function to combine multiple rule ASTs into a single AST
std::shared_ptr<Node> combineRules(const std::vector<std::shared_ptr<Node>>& rules) {
    auto root = std::make_shared<Node>(NodeType::OPERATOR, "AND");

    for (const auto& rule : rules) {
        if (!root->left) {
            root->left = rule;
        } else if (!root->right) {
            root->right = rule;
        } else {
            auto newRoot = std::make_shared<Node>(NodeType::OPERATOR, "AND");
            newRoot->left = root;
            newRoot->right = rule;
            root = newRoot;
        }
    }

    return root;
}

// Function to evaluate the AST against given data
bool evaluateAST(const std::shared_ptr<Node>& node, const std::unordered_map<std::string, int>& data) {
    if (!node) return false;

    if (node->type == NodeType::OPERAND) {
        // Example condition evaluation (needs proper parsing)
        if (node->value == "age > 30") {
            return data.at("age") > 30;
        }
        // Add more condition evaluations here
        return false;
    } else if (node->type == NodeType::OPERATOR) {
        if (node->value == "AND") {
            return evaluateAST(node->left, data) && evaluateAST(node->right, data);
        } else if (node->value == "OR") {
            return evaluateAST(node->left, data) || evaluateAST(node->right, data);
        }
    }
    return false;
}

// Main function to run test cases
int main() {
    // Initialize MongoDB connection
    RuleEngineDB db;

    // Create a sample rule and save it
    auto rule1 = createRuleAST("age > 30 AND department = 'Sales'");
    db.saveRule("rule1", rule1);

    // Load the rule from the database and evaluate it against some data
    auto loadedRule = db.loadRule("rule1");
    if (loadedRule) {
        std::unordered_map<std::string, int> data = {{"age", 35}, {"salary", 60000}};
        bool result = evaluateAST(loadedRule, data);
        std::cout << "Evaluation result: " << (result ? "True" : "False") << std::endl;
    }

    // Create another rule and combine it with the first
    auto rule2 = createRuleAST("salary > 50000 OR experience > 5");
    std::vector<std::shared_ptr<Node>> combinedRules = {rule1, rule2};
    auto combinedAST = combineRules(combinedRules);
    db.saveRule("combined_rule", combinedAST);

    // Evaluate the combined rule
    auto loadedCombinedRule = db.loadRule("combined_rule");
    if (loadedCombinedRule) {
        std::unordered_map<std::string, int> data = {{"age", 40}, {"salary", 55000}, {"experience", 6}};
        bool combinedResult = evaluateAST(loadedCombinedRule, data);
        std::cout << "Combined rule evaluation result: " << (combinedResult ? "True" : "False") << std::endl;
    }

    return 0;
}
