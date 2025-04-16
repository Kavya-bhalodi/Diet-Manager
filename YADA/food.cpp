#ifndef FOOD_CPP
#define FOOD_CPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "json.hpp"
#include <fstream>
#include <iostream>
using namespace std;

using json = nlohmann::json;

class Food
{
public:
    virtual ~Food() = default;
    virtual string getId() const = 0;
    virtual vector<string> getKeywords() const = 0;
    virtual double getCaloriesPerServing() const = 0;
    virtual string getDescription() const = 0;
    virtual json toJson() const = 0;
    virtual string getType() const = 0;
};

// Abstract food class
class AbstractFood : public Food
{
protected:
    string id;
    vector<string> keywords;
    double caloriesPerServing;

public:
    AbstractFood(const string &id, const vector<string> &keywords, double calories)
        : id(id), keywords(keywords), caloriesPerServing(calories) {}

    string getId() const override { return id; }
    vector<string> getKeywords() const override { return keywords; }
    double getCaloriesPerServing() const override { return caloriesPerServing; }
    void addKeyword(const string &keyword) { keywords.push_back(keyword); }
};

// Basic food class
class BasicFood : public AbstractFood
{
private:
    string description;
    double proteins;
    double carbs;
    double fats;

public:
    BasicFood(const string &id, const vector<string> &keywords,
              double calories, const string &description,
              double proteins, double carbs, double fats)
        : AbstractFood(id, keywords, calories), description(description),
          proteins(proteins), carbs(carbs), fats(fats) {}

    string getDescription() const override
    {
        return description + "\nNutritional info per serving:\n" +
               "- Calories: " + to_string(caloriesPerServing) + "\n" +
               "- Proteins: " + to_string(proteins) + "g\n" +
               "- Carbs: " + to_string(carbs) + "g\n" +
               "- Fats: " + to_string(fats) + "g";
    }

    string getType() const override { return "basic"; }

    json toJson() const override
    {
        json j;
        j["id"] = id;
        j["keywords"] = keywords;
        j["calories"] = caloriesPerServing;
        j["description"] = description;
        j["proteins"] = proteins;
        j["carbs"] = carbs;
        j["fats"] = fats;
        j["type"] = "basic";
        return j;
    }

    static shared_ptr<BasicFood> fromJson(const json &j)
    {
        return make_shared<BasicFood>(
            j["id"].get<string>(),
            j["keywords"].get<vector<string>>(),
            j["calories"].get<double>(),
            j["description"].get<string>(),
            j["proteins"].get<double>(),
            j["carbs"].get<double>(),
            j["fats"].get<double>());
    }
};

// Composite food class
class CompositeFood : public AbstractFood
{
private:
    map<string, int> components; // Food ID to servings

public:
    CompositeFood(const string &id, const vector<string> &keywords)
        : AbstractFood(id, keywords, 0) {}

    void addComponent(const string &foodId, int servings)
    {
        components[foodId] = servings;
    }

    map<string, int> getComponents() const
    {
        return components;
    }

    string getDescription() const override
    {
        return "Composite food made of multiple ingredients.";
    }

    string getType() const override { return "composite"; }

    json toJson() const override
    {
        json j;
        j["id"] = id;
        j["keywords"] = keywords;
        j["calories"] = caloriesPerServing;
        j["components"] = components;
        j["type"] = "composite";
        return j;
    }

    static shared_ptr<CompositeFood> fromJson(const json &j)
    {
        auto food = make_shared<CompositeFood>(
            j["id"].get<string>(),
            j["keywords"].get<vector<string>>());
        food->caloriesPerServing = j["calories"].get<double>();
        food->components = j["components"].get<map<string, int>>();
        return food;
    }

    void updateCalories(const map<string, shared_ptr<Food>> &foodDatabase)
    {
        caloriesPerServing = 0;
        for (const auto &[foodId, servings] : components)
        {
            if (foodDatabase.find(foodId) != foodDatabase.end())
            {
                caloriesPerServing += foodDatabase.at(foodId)->getCaloriesPerServing() * servings;
            }
        }
    }
};

class BasicFoodFactory
{
public:
    virtual ~BasicFoodFactory() = default;
    virtual shared_ptr<BasicFood> createBasicFood(const json &data) const = 0;
};

class JsonBasicFoodFactory : public BasicFoodFactory
{
public:
    shared_ptr<BasicFood> createBasicFood(const json &data) const override
    {
        return make_shared<BasicFood>(
            data["id"].get<string>(),
            data["keywords"].get<vector<string>>(),
            data["calories"].get<double>(),
            data["description"].get<string>(),
            data["proteins"].get<double>(),
            data["carbs"].get<double>(),
            data["fats"].get<double>());
    }
};

// Food Manager class
class FoodManager
{
private:
    map<string, shared_ptr<Food>> foodDatabase;
    bool modified = false;
    shared_ptr<BasicFoodFactory> basicFoodFactory;

public:
    FoodManager(shared_ptr<BasicFoodFactory> factory) : basicFoodFactory(factory) {}

    bool loadFromFile(const string &filename)
    {
        try
        {
            ifstream file(filename);
            if (!file.is_open())
            {
                return false;
            }

            json j;
            file >> j;

            for (const auto &foodJson : j)
            {
                string type = foodJson["type"];
                if (type == "basic")
                {
                    auto food = basicFoodFactory->createBasicFood(foodJson);
                    foodDatabase[food->getId()] = food;
                }
                else if (type == "composite")
                {
                    auto food = CompositeFood::fromJson(foodJson);
                    foodDatabase[food->getId()] = food;
                }
            }

            return true;
        }
        catch (exception &e)
        {
            cerr << "Error loading database: " << e.what() << endl;
            return false;
        }
    }

    bool loadDatabase()
    {
        bool basicLoaded = loadFromFile("basic_foods.json");
        bool compositeLoaded = loadFromFile("composite_foods.json");

        // Update calories of composite foods
        for (auto &[id, food] : foodDatabase)
        {
            if (food->getType() == "composite")
            {
                auto compositeFood = dynamic_pointer_cast<CompositeFood>(food);
                compositeFood->updateCalories(foodDatabase);
            }
        }

        return basicLoaded || compositeLoaded;
    }

    bool saveDatabase()
    {
        try
        {
            json basicFoods = json::array();
            json compositeFoods = json::array();

            for (const auto &[id, food] : foodDatabase)
            {
                if (food->getType() == "basic")
                {
                    basicFoods.push_back(food->toJson());
                }
                else if (food->getType() == "composite")
                {
                    compositeFoods.push_back(food->toJson());
                }
            }

            ofstream basicFile("basic_foods.json");
            basicFile << basicFoods.dump(2);

            ofstream compositeFile("composite_foods.json");
            compositeFile << compositeFoods.dump(2);

            modified = false;
            return true;
        }
        catch (exception &e)
        {
            cerr << "Error saving database: " << e.what() << endl;
            return false;
        }
    }

    void addBasicFood(const string &id, const vector<string> &keywords,
                      double calories, const string &description,
                      double proteins, double carbs, double fats)
    {
        auto food = make_shared<BasicFood>(id, keywords, calories, description, proteins, carbs, fats);
        foodDatabase[id] = food;
        modified = true;
    }

    void createCompositeFood(const string &id, const vector<string> &keywords,
                             const map<string, int> &components)
    {
        auto food = make_shared<CompositeFood>(id, keywords);

        for (const auto &[foodId, servings] : components)
        {
            food->addComponent(foodId, servings);
        }

        food->updateCalories(foodDatabase);
        foodDatabase[id] = food;
        modified = true;
    }

    vector<shared_ptr<Food>> searchFoods(const vector<string> &keywords, bool matchAll)
    {
        vector<shared_ptr<Food>> results;

        for (const auto &[id, food] : foodDatabase)
        {
            auto foodKeywords = food->getKeywords();

            if (matchAll)
            {
                bool allMatch = true;
                for (const auto &keyword : keywords)
                {
                    if (find(foodKeywords.begin(), foodKeywords.end(), keyword) == foodKeywords.end())
                    {
                        allMatch = false;
                        break;
                    }
                }
                if (allMatch)
                {
                    results.push_back(food);
                }
            }
            else
            {
                bool anyMatch = false;
                for (const auto &keyword : keywords)
                {
                    if (find(foodKeywords.begin(), foodKeywords.end(), keyword) != foodKeywords.end())
                    {
                        anyMatch = true;
                        break;
                    }
                }
                if (anyMatch)
                {
                    results.push_back(food);
                }
            }
        }

        return results;
    }

    shared_ptr<Food> getFoodById(const string &id) const
    {
        auto it = foodDatabase.find(id);
        if (it != foodDatabase.end())
        {
            return it->second;
        }
        return nullptr;
    }

    vector<shared_ptr<Food>> getAllFoods()
    {
        vector<shared_ptr<Food>> foods;
        for (const auto &[id, food] : foodDatabase)
        {
            foods.push_back(food);
        }
        return foods;
    }

    bool isModified() const
    {
        return modified;
    }
};

#endif // FOOD_CPP