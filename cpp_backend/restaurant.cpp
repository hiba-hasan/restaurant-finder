#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstring>   
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct Restaurant {
    string name;
    string type;
    string location;
    double cost;
    unordered_map<string, double> distanceFromLocation;
};

class RestaurantData {
public:
    static vector<Restaurant> getAllRestaurants() {
        return {
            {"Veg Delight", "veg", "Guindy", 500, {{"Guindy", 2.5}, {"T.Nagar", 5.0}, {"Besant Nagar", 6.5}}},
            {"Spicy Feast", "non-veg", "T.Nagar", 600, {{"Guindy", 4.0}, {"T.Nagar", 1.0}, {"Besant Nagar", 7.0}}},
            {"Breakfast Corner", "breakfast", "Besant Nagar", 150, {{"Guindy", 6.0}, {"T. Nagar", 6.5}, {"Besant Nagar", 1.0}}},
            {"The Lunch Spot", "lunch", "Guindy", 300, {{"Guindy", 1.5}, {"T.Nagar", 3.0}, {"Besant Nagar", 5.5}}},
            {"Dinner House", "dinner", "T.Nagar", 450, {{"Guindy", 3.5}, {"T.Nagar", 0.5}, {"Besant Nagar", 6.0}}},
            {"Coastal Curry", "non-veg", "Besant Nagar", 700, {{"Guindy", 7.2}, {"T.Nagar", 6.8}, {"Besant Nagar", 1.2}}},
            {"Sunrise Tiffins", "breakfast", "Guindy", 120, {{"Guindy", 1.0}, {"T.Nagar", 2.5}, {"Besant Nagar", 4.0}}},
            {"Midday Meals", "lunch", "T.Nagar", 250, {{"Guindy", 2.0}, {"T.Nagar", 0.8}, {"Besant Nagar", 5.5}}},
            {"Twilight Treats", "dinner", "Besant Nagar", 550, {{"Guindy", 6.2}, {"T.Nagar", 6.0}, {"Besant Nagar", 0.7}}},
            {"Chennai Biryani", "non-veg", "Guindy", 400, {{"Guindy", 1.2}, {"T.Nagar", 3.0}, {"Besant Nagar", 5.2}}}
        };
    }
};

class Filters {
public:
    static vector<Restaurant> filterByType(const vector<Restaurant>& restaurants, const string& type) {
        vector<Restaurant> result;
        for (const auto& r : restaurants) {
            if (r.type == type) {
                result.push_back(r);
            }
        }
        return result;
    }
    static vector<Restaurant> filterByCost(const vector<Restaurant>& restaurants, double maxCost) {
        vector<Restaurant> result;
        for (const auto& r : restaurants) {
            if (r.cost <= maxCost) {
                result.push_back(r);
            }
        }
        return result;
    }
};

class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    TrieNode() : isEndOfWord(false) {}
};

class Trie {
private:
    TrieNode* root;
public:
    Trie() : root(new TrieNode()) {}
    ~Trie() { clear(root); }

    void clear(TrieNode* node) {
        if (!node) return;
        for (auto& [ch, child] : node->children) {
            clear(child);
            delete child;
        }
    }

    void insert(const string& word) {
        TrieNode* curr = root;
        for (char ch : word) {
            if (curr->children.find(ch) == curr->children.end()) {
                curr->children[ch] = new TrieNode();
            }
            curr = curr->children[ch];
        }
        curr->isEndOfWord = true;
    }

    bool search(const string& word) {
        TrieNode* curr = root;
        for (char ch : word) {
            if (curr->children.find(ch) == curr->children.end()) {
                return false;
            }
            curr = curr->children[ch];
        }
        return curr->isEndOfWord;
    }
};

class WeightedGraph {
private:
    unordered_map<string, unordered_map<string, double>> adjacencyList;

public:
    void addEdge(const string& from, const string& to, double weight) {
        adjacencyList[from][to] = weight;
        adjacencyList[to][from] = weight;
    }

    vector<pair<string, double>> getNearestRestaurants(const string& location, double maxKm) {
        vector<pair<string, double>> nearest;
        if (adjacencyList.find(location) == adjacencyList.end()) {
            return nearest;
        }
        const auto& neighbors = adjacencyList.at(location);
        for (const auto& [node, dist] : neighbors) {
            if (dist <= maxKm) {
                nearest.push_back({node, dist});
            }
        }
        return nearest;
    }

    void buildGraph(const vector<Restaurant>& restaurants) {
        for (const auto& r : restaurants) {
            for (const auto& [loc, dist] : r.distanceFromLocation) {
                addEdge(r.name, loc, dist);
            }
        }
    }
};

string restaurantsToJson(const vector<Restaurant>& restaurants) {
    json j = json::array();
    for (const auto& r : restaurants) {
        j.push_back({
            {"name", r.name},
            {"type", r.type},
            {"location", r.location},
            {"cost", r.cost}
        });
    }
    return j.dump(4);
}

extern "C" {

void get_all_restaurants(char* output, size_t size) {
    auto restaurants = RestaurantData::getAllRestaurants();
    string jsonStr = restaurantsToJson(restaurants);
    if (jsonStr.size() >= size) {
        strncpy(output, jsonStr.c_str(), size-1);
        output[size-1] = '\0';
    } else {
        strcpy(output, jsonStr.c_str());
    }
}

void filter_by_type(const char* type, char* output, size_t size) {
    auto restaurants = RestaurantData::getAllRestaurants();
    auto filtered = Filters::filterByType(restaurants, string(type));
    string jsonStr = restaurantsToJson(filtered);
    if (jsonStr.size() >= size) {
        strncpy(output, jsonStr.c_str(), size-1);
        output[size-1] = '\0';
    } else {
        strcpy(output, jsonStr.c_str());
    }
}

void filter_by_cost(double maxCost, char* output, size_t size) {
    auto restaurants = RestaurantData::getAllRestaurants();
    auto filtered = Filters::filterByCost(restaurants, maxCost);
    string jsonStr = restaurantsToJson(filtered);
    if (jsonStr.size() >= size) {
        strncpy(output, jsonStr.c_str(), size-1);
        output[size-1] = '\0';
    } else {
        strcpy(output, jsonStr.c_str());
    }
}

void search_by_name(const char* name, char* output, size_t size) {
    auto restaurants = RestaurantData::getAllRestaurants();
    Trie trie;
    for (auto& r : restaurants) {
        trie.insert(r.name);
    }
    bool found = trie.search(string(name));
    json j = {{"name", name}, {"found", found}};
    string jsonStr = j.dump();
    if (jsonStr.size() >= size) {
        strncpy(output, jsonStr.c_str(), size-1);
        output[size-1] = '\0';
    } else {
        strcpy(output, jsonStr.c_str());
    }
}

void get_nearby(const char* location, double maxKm, char* output, size_t size) {
    auto restaurants = RestaurantData::getAllRestaurants();
    WeightedGraph graph;
    graph.buildGraph(restaurants);
    auto nearby = graph.getNearestRestaurants(string(location), maxKm);

    json j = json::array();
    for (auto& [name, dist] : nearby) {
        json entry = {{"name", name}, {"distance", dist}};
        j.push_back(entry);
    }
    string jsonStr = j.dump(4);
    if (jsonStr.size() >= size) {
        strncpy(output, jsonStr.c_str(), size-1);
        output[size-1] = '\0';
    } else {
        strcpy(output, jsonStr.c_str());
    }
}

} 
