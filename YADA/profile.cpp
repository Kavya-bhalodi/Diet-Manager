#ifndef PROFILE_CPP
#define PROFILE_CPP

#include <string>
#include "json.hpp"
#include <fstream>
#include <iostream>
using namespace std;

using json = nlohmann::json;


// User Profile class
class CalorieCalculationStrategy
{
public:
    virtual ~CalorieCalculationStrategy() = default;
    virtual double calculateCalories(const string &gender, double height, int age, double weight, const string &activityLevel) const = 0;
};

class HarrisBenedictStrategy : public CalorieCalculationStrategy
{
public:
    double calculateCalories(const string &gender, double height, int age, double weight, const string &activityLevel) const override
    {
        double bmr = 0.0;
        if (gender == "male")
        {
            bmr = 88.362 + (13.397 * weight) + (4.799 * height) - (5.677 * age);
        }
        else
        {
            bmr = 447.593 + (9.247 * weight) + (3.098 * height) - (4.330 * age);
        }

        double activityFactor = getActivityFactor(activityLevel);
        return bmr * activityFactor;
    }

private:
    double getActivityFactor(const string &activityLevel) const
    {
        if (activityLevel == "light")
            return 1.375;
        else if (activityLevel == "moderate")
            return 1.55;
        else if (activityLevel == "active")
            return 1.725;
        else if (activityLevel == "very active")
            return 1.9;
        return 1.2; // Default: sedentary
    }
};

class MifflinStJeorStrategy : public CalorieCalculationStrategy
{
public:
    double calculateCalories(const string &gender, double height, int age, double weight, const string &activityLevel) const override
    {
        double bmr = 0.0;
        if (gender == "male")
        {
            bmr = (10 * weight) + (6.25 * height) - (5 * age) + 5;
        }
        else
        {
            bmr = (10 * weight) + (6.25 * height) - (5 * age) - 161;
        }

        double activityFactor = getActivityFactor(activityLevel);
        return bmr * activityFactor;
    }

private:
    double getActivityFactor(const string &activityLevel) const
    {
        if (activityLevel == "light")
            return 1.375;
        else if (activityLevel == "moderate")
            return 1.55;
        else if (activityLevel == "active")
            return 1.725;
        else if (activityLevel == "very active")
            return 1.9;
        return 1.2; // Default: sedentary
    }
};

// User Profile class
class UserProfile
{
private:
    string gender;
    double height; // in cm
    int age;
    double weight; // in kg
    string activityLevel;
    shared_ptr<CalorieCalculationStrategy> calorieStrategy; // Strategy pattern

public:
    UserProfile()
        : gender(""), height(0), age(0), weight(0), activityLevel(""),
          calorieStrategy(make_shared<HarrisBenedictStrategy>()) {} // Default strategy

    void setCalorieCalculationStrategy(shared_ptr<CalorieCalculationStrategy> strategy)
    {
        calorieStrategy = strategy;
    }

    double calculateTargetCalories() const
    {
        if (!calorieStrategy || gender.empty() || height <= 0 || age <= 0 || weight <= 0 || activityLevel.empty())
        {
            return 0.0;
        }
        return calorieStrategy->calculateCalories(gender, height, age, weight, activityLevel);
    }

    string getGender() const { return gender; }
    double getHeight() const { return height; }
    int getAge() const { return age; }
    double getWeight() const { return weight; }
    string getActivityLevel() const { return activityLevel; }
    string getCalculationMethod() const
    {
        if (dynamic_cast<HarrisBenedictStrategy *>(calorieStrategy.get()))
            return "harris-benedict";
        else if (dynamic_cast<MifflinStJeorStrategy *>(calorieStrategy.get()))
            return "mifflin-st-jeor";

        return "unknown";
    }

    void setGender(const string &g) { gender = g; }
    void setHeight(double h) { height = h; }
    void setAge(int a) { age = a; }
    void setWeight(double w) { weight = w; }
    void setActivityLevel(const string &al) { activityLevel = al; }

    json toJson() const
    {
        json j;
        j["gender"] = gender;
        j["height"] = height;
        j["age"] = age;
        j["weight"] = weight;
        j["activityLevel"] = activityLevel;
        return j;
    }

    static UserProfile fromJson(const json &j)
    {
        UserProfile profile;
        profile.setGender(j["gender"].get<string>());
        profile.setHeight(j["height"].get<double>());
        profile.setAge(j["age"].get<int>());
        profile.setWeight(j["weight"].get<double>());
        profile.setActivityLevel(j["activityLevel"].get<string>());
        return profile;
    }
};

// Profile Manager class
class ProfileManager
{
private:
    UserProfile profile;
    bool modified = false;

public:
    bool loadProfile()
    {
        try
        {
            ifstream file("user_profile.json");
            if (!file.is_open())
            {
                return false;
            }

            json j;
            file >> j;
            profile = UserProfile::fromJson(j);
            modified = false;
            return true;
        }
        catch (exception &e)
        {
            cerr << "Error loading profile: " << e.what() << endl;
            return false;
        }
    }

    bool saveProfile()
    {
        try
        {
            json j = profile.toJson();
            ofstream file("user_profile.json");
            file << j.dump(2);
            modified = false;
            return true;
        }
        catch (exception &e)
        {
            cerr << "Error saving profile: " << e.what() << endl;
            return false;
        }
    }

    void updateProfile( double height, int age, double weight, const string &activityLevel)
    {
        // profile.setGender(gender);
        profile.setHeight(height);
        profile.setAge(age);
        profile.setWeight(weight);
        profile.setActivityLevel(activityLevel);
        modified = true;
    }

    void setCalculationMethod(const string &method)
    {
        if (method == "harris-benedict")
        {
            profile.setCalorieCalculationStrategy(make_shared<HarrisBenedictStrategy>());
        }
        else if (method == "mifflin-st-jeor")
        {
            profile.setCalorieCalculationStrategy(make_shared<MifflinStJeorStrategy>());
        }
        modified = true;
    }

    double getTargetCalories() const
    {
        return profile.calculateTargetCalories();
    }

    const UserProfile &getProfile() const
    {
        return profile;
    }

    bool isModified() const
    {
        return modified;
    }
};

#endif