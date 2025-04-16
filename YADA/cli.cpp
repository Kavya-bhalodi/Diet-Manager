#ifndef CLI_CPP
#define CLI_CPP

#include "food.cpp"
#include "log.cpp"
#include "profile.cpp"
#include <iostream>
#include <limits>
using namespace std;

// Enhanced Command Line Interface
class CLI
{
private:
    FoodManager &foodManager;
    LogManager &logManager;
    ProfileManager &profileManager;
    bool running = true;

    const string RESET = "\033[0m";
    const string BOLD = "\033[1m";
    const string GREEN = "\033[32m";
    const string BLUE = "\033[34m";
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string RED = "\033[31m";

    void printHeader(const string &title)
    {
        cout << BOLD << BLUE;
        cout << "\n╔════" << string(title.length(), '=') << "════╗\n";
        cout << "║    " << title << "    ║\n";
        cout << "╚════" << string(title.length(), '=') << "════╝\n"
             << RESET;
    }

    void printMenuOption(const string &number, const string &description)
    {
        cout << CYAN << " [" << number << "] " << RESET << description << "\n";
    }

    void printDivider()
    {
        cout << BLUE << "──────────────────────────────────────────────────────\n"
             << RESET;
    }

    void printSuccess(const string &message)
    {
        cout << GREEN << "✔ " << message << RESET << "\n";
    }

    void printError(const string &message)
    {
        cout << RED << "✘ " << message << RESET << "\n";
    }

    void printInfo(const string &message)
    {
        cout << YELLOW << "ℹ " << message << RESET << "\n";
    }

    void displayMenu()
    {
        printHeader("YADA (Yet Another Diet Assistant)");
        printMenuOption("1", "Food Database");
        printMenuOption("2", "Daily Log");
        printMenuOption("3", "User Profile");
        printMenuOption("4", "Save All");
        printMenuOption("5", "Exit");
        printDivider();
        cout << "Enter your choice: ";
    }

    void displayFoodDatabaseMenu()
    {
        printHeader("Food Database Menu");
        printMenuOption("1", "View all foods");
        printMenuOption("2", "Search foods by keywords");
        printMenuOption("3", "View food details");
        printMenuOption("4", "Add new basic food");
        printMenuOption("5", "Create composite food");
        printMenuOption("6", "Save database");
        printMenuOption("7", "Search Online using API");
        printMenuOption("8", "Back to main menu");
        printDivider();
        cout << "Enter your choice: ";
    }

    void displayDailyLogMenu()
    {
        printHeader("Daily Log Menu");
        cout << "Current date: " << logManager.getCurrentDate() << "\n";
        printMenuOption("1", "View daily log");
        printMenuOption("2", "Add food to log");
        printMenuOption("3", "Remove food from log");
        printMenuOption("4", "Undo last action");
        printMenuOption("5", "Change current date");
        printMenuOption("6", "View calorie summary");
        printMenuOption("7", "Save log");
        printMenuOption("8", "Back to main menu");
        printDivider();
        cout << "Enter your choice: ";
    }

    void displayProfileMenu()
    {
        printHeader("User Profile Menu");
        printMenuOption("1", "View profile");
        printMenuOption("2", "Update profile");
        printMenuOption("3", "Change calorie calculation method");
        printMenuOption("4", "Save profile");
        printMenuOption("5", "Back to main menu");
        printDivider();
        cout << "Enter your choice: ";
    }

    void viewAllFoods()
    {
        auto foods = foodManager.getAllFoods();
        if (foods.empty())
        {
            // cout << "No foods in database.\n";
            printError("No foods in database.");
            return;
        }

        // cout << "\n===== All Foods =====\n";
        printHeader("All Foods");
        for (const auto &food : foods)
        {
            cout << CYAN << "ID: " << RESET << food->getId() << " (" << food->getType() << ")\n";
            cout << YELLOW << "Calories: " << RESET << food->getCaloriesPerServing() << " per serving\n";
            cout << GREEN << "Keywords: " << RESET;
            for (const auto &keyword : food->getKeywords())
            {
                cout << keyword << " ";
            }
            cout << "\n"
                 << BLUE << "──────────────────────────" << RESET << "\n";
        }
    }

    void searchFoods()
    {
        string keywordsInput;
        cout << "Enter keywords (comma separated): ";
        getline(cin, keywordsInput);

        vector<string> keywords;
        size_t pos = 0;
        while ((pos = keywordsInput.find(',')) != string::npos)
        {
            string keyword = keywordsInput.substr(0, pos);
            keyword.erase(0, keyword.find_first_not_of(" \t"));
            keyword.erase(keyword.find_last_not_of(" \t") + 1);
            keywords.push_back(keyword);
            keywordsInput.erase(0, pos + 1);
        }
        keywordsInput.erase(0, keywordsInput.find_first_not_of(" \t"));
        keywordsInput.erase(keywordsInput.find_last_not_of(" \t") + 1);
        if (!keywordsInput.empty())
        {
            keywords.push_back(keywordsInput);
        }

        string matchType;
        cout << "Match all keywords or any keyword? (all/any): ";
        getline(cin, matchType);
        bool matchAll = (matchType == "all");

        auto results = foodManager.searchFoods(keywords, matchAll);
        if (results.empty())
        {
            cout << "No foods found matching those keywords.\n";
            return;
        }

        cout << "\n===== Search Results =====\n";
        for (const auto &food : results)
        {
            cout << "ID: " << food->getId() << " (" << food->getType() << ")\n";
            cout << "Calories: " << food->getCaloriesPerServing() << " per serving\n";
            cout << "Keywords: ";
            for (const auto &keyword : food->getKeywords())
            {
                cout << keyword << " ";
            }
            cout << "\n\n";
        }
    }

    void viewFoodDetails()
    {
        string id;
        cout << "Enter food ID: ";
        getline(cin, id);

        auto food = foodManager.getFoodById(id);
        if (!food)
        {
            printError("Food not found.");
            return;
        }

        printHeader("Food Details");
        cout << CYAN << "ID: " << RESET << food->getId() << "\n";
        cout << CYAN << "Type: " << RESET << food->getType() << "\n";
        cout << CYAN << "Calories: " << RESET << food->getCaloriesPerServing() << " per serving\n";

        cout << CYAN << "Keywords: " << RESET;
        for (const auto &keyword : food->getKeywords())
        {
            cout << YELLOW << keyword << " " << RESET;
        }
        cout << "\n";
        printDivider();

        cout << food->getDescription() << "\n";

        if (food->getType() == "composite")
        {
            auto compositeFood = dynamic_pointer_cast<CompositeFood>(food);
            printHeader("Components");
            for (const auto &[compId, servings] : compositeFood->getComponents())
            {
                auto component = foodManager.getFoodById(compId);
                if (component)
                {
                    cout << "• " << CYAN << component->getId() << RESET
                         << " (" << GREEN << servings << " servings" << RESET << ")\n";
                }
            }
        }
    }

    void addBasicFood()
    {
        printHeader("Add Basic Food");

        string id, description, keywordsInput;
        double calories, proteins, carbs, fats;

        // Get food ID
        cout << CYAN << "Enter food ID: " << RESET;
        getline(cin, id);

        if (id.empty())
        {
            printError("Food ID cannot be empty.");
            return;
        }

        if (foodManager.getFoodById(id))
        {
            printError("Food with this ID already exists.");
            return;
        }

        // Get description
        cout << CYAN << "Enter description: " << RESET;
        getline(cin, description);

        if (description.empty())
        {
            printError("Description cannot be empty.");
            return;
        }

        // Get keywords
        cout << CYAN << "Enter keywords" << RESET << " (comma separated, e.g. breakfast,healthy,protein): ";
        getline(cin, keywordsInput);

        if (keywordsInput.empty())
        {
            printError("At least one keyword is required.");
            return;
        }

        // Process keywords
        vector<string> keywords;
        size_t pos = 0;
        while ((pos = keywordsInput.find(',')) != string::npos)
        {
            string keyword = keywordsInput.substr(0, pos);
            keyword.erase(0, keyword.find_first_not_of(" \t"));
            keyword.erase(keyword.find_last_not_of(" \t") + 1);
            if (!keyword.empty())
            {
                keywords.push_back(keyword);
            }
            keywordsInput.erase(0, pos + 1);
        }
        keywordsInput.erase(0, keywordsInput.find_first_not_of(" \t"));
        keywordsInput.erase(keywordsInput.find_last_not_of(" \t") + 1);
        if (!keywordsInput.empty())
        {
            keywords.push_back(keywordsInput);
        }

        if (keywords.empty())
        {
            printError("At least one valid keyword is required.");
            return;
        }

        // Show entered keywords
        cout << "\n"
             << YELLOW << "Keywords added: " << RESET;
        for (const auto &keyword : keywords)
        {
            cout << GREEN << keyword << RESET << " ";
        }
        cout << "\n";
        printDivider();

        // Get nutritional information
        printHeader("Nutritional Information");

        cout << CYAN << "Enter calories per serving: " << RESET;
        if (!(cin >> calories) || calories < 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid calorie value. Must be a positive number.");
            return;
        }
        cin.ignore();

        cout << CYAN << "Enter proteins" << RESET << " (grams): ";
        if (!(cin >> proteins) || proteins < 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid protein value. Must be a positive number.");
            return;
        }
        cin.ignore();

        cout << CYAN << "Enter carbs" << RESET << " (grams): ";
        if (!(cin >> carbs) || carbs < 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid carbs value. Must be a positive number.");
            return;
        }
        cin.ignore();

        cout << CYAN << "Enter fats" << RESET << " (grams): ";
        if (!(cin >> fats) || fats < 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid fats value. Must be a positive number.");
            return;
        }
        cin.ignore();

        // Show summary before adding
        printDivider();
        printInfo("Food Summary:");
        cout << CYAN << "ID: " << RESET << id << "\n";
        cout << CYAN << "Description: " << RESET << description << "\n";
        cout << CYAN << "Nutritional Values (per serving):" << RESET << "\n";
        cout << "• Calories: " << YELLOW << calories << RESET << "\n";
        cout << "• Proteins: " << GREEN << proteins << "g" << RESET << "\n";
        cout << "• Carbs: " << GREEN << carbs << "g" << RESET << "\n";
        cout << "• Fats: " << GREEN << fats << "g" << RESET << "\n";

        foodManager.addBasicFood(id, keywords, calories, description, proteins, carbs, fats);
        printSuccess("Basic food added successfully!");
    }

    void createCompositeFood()
    {
        string id, keywordsInput;
        map<string, int> components;

        printHeader("Create Composite Food");

        // Get food ID
        cout << CYAN << "Enter composite food ID: " << RESET;
        getline(cin, id);

        if (id.empty())
        {
            printError("Food ID cannot be empty.");
            return;
        }

        if (foodManager.getFoodById(id))
        {
            printError("Food with this ID already exists.");
            return;
        }

        // Get keywords
        cout << CYAN << "Enter keywords" << RESET << " (comma separated, e.g. lunch,dinner,healthy): ";
        getline(cin, keywordsInput);

        if (keywordsInput.empty())
        {
            printError("Keywords cannot be empty. At least one keyword is required.");
            return;
        }

        // Process keywords
        vector<string> keywords;
        size_t pos = 0;
        while ((pos = keywordsInput.find(',')) != string::npos)
        {
            string keyword = keywordsInput.substr(0, pos);
            keyword.erase(0, keyword.find_first_not_of(" \t"));
            keyword.erase(keyword.find_last_not_of(" \t") + 1);
            if (!keyword.empty())
            {
                keywords.push_back(keyword);
            }
            keywordsInput.erase(0, pos + 1);
        }
        keywordsInput.erase(0, keywordsInput.find_first_not_of(" \t"));
        keywordsInput.erase(keywordsInput.find_last_not_of(" \t") + 1);
        if (!keywordsInput.empty())
        {
            keywords.push_back(keywordsInput);
        }

        if (keywords.empty())
        {
            printError("No valid keywords provided. At least one keyword is required.");
            return;
        }

        // Show entered keywords
        cout << "\n"
             << YELLOW << "Keywords added: " << RESET;
        for (const auto &keyword : keywords)
        {
            cout << GREEN << keyword << RESET << " ";
        }
        cout << "\n";

        printDivider();
        printHeader("Add Components");
        printInfo("Now let's add the components for your composite food:");
        printInfo("- Enter 'list' to view all available foods");
        printInfo("- Leave food ID empty when finished");
        printDivider();

        while (true)
        {
            string compId;
            int servings;

            cout << "\n"
                 << CYAN << "Enter component food ID" << RESET
                 << " (or 'list' to view all foods, empty to finish): ";
            getline(cin, compId);

            if (compId.empty())
            {
                break;
            }

            if (compId == "list")
            {
                viewAllFoods();
                continue;
            }

            auto component = foodManager.getFoodById(compId);
            if (!component)
            {
                printError("Food not found. Please try again.");
                continue;
            }

            // Show selected food details
            printInfo("Selected component:");
            cout << CYAN << "Name: " << RESET << component->getId() << "\n"
                 << CYAN << "Calories: " << RESET << component->getCaloriesPerServing()
                 << " per serving\n";

            cout << YELLOW << "Enter number of servings: " << RESET;
            if (!(cin >> servings))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                printError("Invalid input. Please enter a number.");
                continue;
            }
            cin.ignore();

            if (servings <= 0)
            {
                printError("Servings must be positive. Please try again.");
                continue;
            }

            components[compId] = servings;
            printSuccess("Component added successfully!");

            // Show current components
            cout << "\n"
                 << BOLD << "Current components:" << RESET << "\n";
            for (const auto &[id, srv] : components)
            {
                auto food = foodManager.getFoodById(id);
                if (food)
                {
                    cout << "• " << CYAN << food->getId() << RESET
                         << " - " << GREEN << srv << " serving(s)" << RESET
                         << " (" << YELLOW << food->getCaloriesPerServing() * srv
                         << " cal" << RESET << ")\n";
                }
            }
            printDivider();
        }

        if (components.empty())
        {
            printError("No components added. Composite food creation cancelled.");
            return;
        }

        foodManager.createCompositeFood(id, keywords, components);
        printSuccess("Composite food created successfully!");
    }

    void viewDailyLog()
    {
        const auto &log = logManager.getCurrentDayLog();
        const auto &entries = log.getEntries();

        printHeader("Daily Log for " + logManager.getCurrentDate());

        if (entries.empty())
        {
            printInfo("No entries for this date.");
            return;
        }

        int index = 0;
        double totalCalories = 0.0;

        for (const auto &entry : entries)
        {
            auto food = foodManager.getFoodById(entry.getFoodId());
            if (food)
            {
                double calories = food->getCaloriesPerServing() * entry.getServings();
                totalCalories += calories;

                cout << CYAN << "[" << index << "] " << RESET
                     << BOLD << food->getId() << RESET
                     << " - " << GREEN << entry.getServings() << " serving(s)" << RESET
                     << " (" << YELLOW << calories << " cal" << RESET << ")\n";
            }
            index++;
        }

        printDivider();
        cout << BOLD << "Total calories: " << YELLOW << totalCalories << RESET << "\n";

        double targetCalories = profileManager.getTargetCalories();
        if (targetCalories > 0)
        {
            double diff = totalCalories - targetCalories;
            cout << BOLD << "Target calories: " << CYAN << targetCalories << RESET << "\n";
            cout << BOLD << "Difference: " << (diff < 0 ? RED : GREEN) << diff
                 << " (" << (diff < 0 ? "under" : "over") << " target)" << RESET << "\n";
        }
    }

    void addFoodToLog()
    {
        string foodId;
        int servings;

        printHeader("Add Food to Log");
        cout << "Enter food ID " << CYAN << "(or 'list' to view all foods): " << RESET;
        getline(cin, foodId);

        if (foodId == "list")
        {
            viewAllFoods();
            cout << "\nEnter food ID: ";
            getline(cin, foodId);
        }

        auto food = foodManager.getFoodById(foodId);
        if (!food)
        {
            printError("Food not found.");
            return;
        }

        // Show food details before confirming
        printInfo("Selected Food:");
        cout << CYAN << "Name: " << RESET << food->getId() << "\n";
        cout << CYAN << "Calories: " << RESET << food->getCaloriesPerServing() << " per serving\n";

        cout << "Enter number of servings: ";
        cin >> servings;
        cin.ignore();

        if (servings <= 0)
        {
            printError("Servings must be positive.");
            return;
        }

        logManager.addFoodToLog(foodId, servings);
        printSuccess("Food added to log successfully!");

        // Show updated calorie total
        double totalCalories = logManager.getTotalCaloriesForDay();
        cout << YELLOW << "Daily total is now: " << totalCalories << " calories" << RESET << "\n";
    }

    void removeFoodFromLog()
    {
        const auto &log = logManager.getCurrentDayLog();
        const auto &entries = log.getEntries();

        if (entries.empty())
        {
            printError("No entries to remove.");
            return;
        }

        printHeader("Remove Food from Log");
        int index = 0;
        for (const auto &entry : entries)
        {
            auto food = foodManager.getFoodById(entry.getFoodId());
            if (food)
            {
                cout << CYAN << "[" << index << "] " << RESET
                     << BOLD << food->getId() << RESET
                     << " - " << GREEN << entry.getServings() << " serving(s)" << RESET
                     << " (" << YELLOW << food->getCaloriesPerServing() * entry.getServings()
                     << " cal" << RESET << ")\n";
            }
            index++;
        }
        printDivider();

        int entryIndex;
        string input;
        cout << "Enter entry number to remove: ";
        getline(cin, input);

        try
        {
            entryIndex = stoi(input);
        }
        catch (const exception &e)
        {
            printError("Invalid input. Please enter a valid number.");
            return;
        }

        if (entryIndex < 0 || entryIndex >= entries.size())
        {
            printError("Invalid entry number.");
            return;
        }

        logManager.removeFoodFromLog(entryIndex);
        printSuccess("Entry removed successfully!");
    }

    void changeDate()
    {
        string date;
        cout << "Enter date (YYYY-MM-DD), or 'today' for current date: ";
        getline(cin, date);

        if (date == "today")
        {
            time_t t = time(nullptr);
            tm timeinfo = *localtime(&t);
            char buffer[11];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
            date = string(buffer);
        }

        // Very basic date validation
        if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        {
            cout << "Invalid date format. Use YYYY-MM-DD.\n";
            return;
        }

        logManager.setCurrentDate(date);
        cout << "Date changed to " << date << ".\n";
    }

    void viewCalorieSummary()
    {
        double totalCalories = logManager.getTotalCaloriesForDay();
        double targetCalories = profileManager.getTargetCalories();

        printHeader("Calorie Summary for " + logManager.getCurrentDate());

        // Create a visual progress bar
        const int barWidth = 40;
        double percentage = (targetCalories > 0) ? (totalCalories / targetCalories) * 100 : 0;
        int progressWidth = static_cast<int>((percentage / 100) * barWidth);

        cout << BOLD << "Total calories consumed: " << YELLOW << totalCalories << RESET << "\n";

        if (targetCalories > 0)
        {
            cout << BOLD << "Target calories: " << CYAN << targetCalories << RESET << "\n";
            double diff = totalCalories - targetCalories;

            // Progress bar
            cout << "[";
            for (int i = 0; i < barWidth; ++i)
            {
                if (i < progressWidth)
                {
                    cout << (diff < 0 ? GREEN : RED) << "█" << RESET;
                }
                else
                {
                    cout << "░";
                }
            }
            cout << "] " << static_cast<int>(percentage) << "%\n";

            // Difference from target
            cout << BOLD << "Difference: " << (diff < 0 ? GREEN : RED)
                 << diff << " calories "
                 << "(" << (diff < 0 ? "under" : "over") << " target)"
                 << RESET << "\n";
        }
        else
        {
            printInfo("No target calories set. Please update your profile.");
        }
    }

    void viewProfile()
    {
        const auto &profile = profileManager.getProfile();

        printHeader("User Profile");

        cout << CYAN << "Gender: " << RESET << profile.getGender() << "\n";
        cout << CYAN << "Height: " << RESET << profile.getHeight() << " cm\n";
        cout << CYAN << "Age: " << RESET << profile.getAge() << " years\n";
        cout << CYAN << "Weight: " << RESET << profile.getWeight() << " kg\n";
        cout << CYAN << "Activity level: " << RESET << profile.getActivityLevel() << "\n";
        cout << CYAN << "Calculation method: " << RESET << profile.getCalculationMethod() << "\n";

        printDivider();

        double targetCalories = profileManager.getTargetCalories();
        if (targetCalories > 0)
        {
            cout << BOLD << "Daily Target: " << GREEN << targetCalories
                 << " calories" << RESET << "\n";
        }
        else
        {
            printError("Target calories not available. Please complete your profile.");
        }
    }

    void updateProfile()
    {
        printHeader("Update Profile");

        // Show current profile if it exists
        const auto &currentProfile = profileManager.getProfile();
        if (currentProfile.getGender() != "")
        {
            printInfo("Current Profile:");
            cout << CYAN << "Gender: " << RESET << currentProfile.getGender() << "\n";
            cout << CYAN << "Height: " << RESET << currentProfile.getHeight() << " cm\n";
            cout << CYAN << "Age: " << RESET << currentProfile.getAge() << " years\n";
            cout << CYAN << "Weight: " << RESET << currentProfile.getWeight() << " kg\n";
            cout << CYAN << "Activity level: " << RESET << currentProfile.getActivityLevel() << "\n";
            printDivider();
        }

        string gender, activityLevel;
        double height, weight;
        int age;

        // Gender input
        // cout << CYAN << "Enter gender" << RESET << " (male/female): ";
        // getline(cin, gender);

        // if (gender != "male" && gender != "female")
        // {
        //     printError("Invalid gender. Please enter 'male' or 'female'.");
        //     return;
        // }

        // Height input
        cout << CYAN << "Enter height" << RESET << " (cm): ";
        if (!(cin >> height))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input. Height must be a number.");
            return;
        }
        cin.ignore();

        if (height <= 0 || height > 300)
        {
            printError("Height must be between 0 and 300 cm.");
            return;
        }

        // Age input
        cout << CYAN << "Enter age" << RESET << " (years): ";
        if (!(cin >> age))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input. Age must be a number.");
            return;
        }
        cin.ignore();

        if (age <= 0 || age > 150)
        {
            printError("Age must be between 0 and 150 years.");
            return;
        }

        // Weight input
        cout << CYAN << "Enter weight" << RESET << " (kg): ";
        if (!(cin >> weight))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input. Weight must be a number.");
            return;
        }
        cin.ignore();

        if (weight <= 0 || weight > 500)
        {
            printError("Weight must be between 0 and 500 kg.");
            return;
        }

        // Activity level input
        printInfo("\nActivity Levels:");
        cout << "• sedentary    (little or no exercise)\n";
        cout << "• light        (exercise 1-3 times/week)\n";
        cout << "• moderate     (exercise 3-5 times/week)\n";
        cout << "• active       (exercise 6-7 times/week)\n";
        cout << "• very active  (hard exercise 6-7 times/week)\n\n";

        cout << CYAN << "Enter activity level" << RESET << ": ";
        getline(cin, activityLevel);

        if (activityLevel != "sedentary" && activityLevel != "light" &&
            activityLevel != "moderate" && activityLevel != "active" &&
            activityLevel != "very active")
        {
            printError("Invalid activity level.");
            return;
        }

        // Show summary before confirming
        printDivider();
        printInfo("Profile Summary:");
        // cout << CYAN << "Gender: " << RESET << gender << "\n";
        cout << CYAN << "Height: " << RESET << height << " cm\n";
        cout << CYAN << "Age: " << RESET << age << " years\n";
        cout << CYAN << "Weight: " << RESET << weight << " kg\n";
        cout << CYAN << "Activity Level: " << RESET << activityLevel << "\n";

        profileManager.updateProfile(height, age, weight, activityLevel);
        printSuccess("Profile updated successfully!");

        // Show calculated target calories
        double targetCalories = profileManager.getTargetCalories();
        cout << BOLD << "Daily calorie target: " << GREEN << targetCalories
             << " calories" << RESET << "\n";
    }

    void changeCalculationMethod()
    {
        printHeader("Change Calculation Method");
        printInfo("Available Methods:");
        cout << CYAN << "1. Harris-Benedict Equation" << RESET << " (traditional method)\n";
        cout << CYAN << "2. Mifflin-St Jeor Equation" << RESET << " (modern method)\n";
        printDivider();

        cout << "Enter your choice: ";
        string choice;
        getline(cin, choice);

        if (choice == "1")
        {
            profileManager.setCalculationMethod("harris-benedict");
            printSuccess("Calculation method changed to Harris-Benedict.");
        }
        else if (choice == "2")
        {
            profileManager.setCalculationMethod("mifflin-st-jeor");
            printSuccess("Calculation method changed to Mifflin-St Jeor.");
        }
        else
        {
            printError("Invalid choice.");
        }
    }

    void searchOnlineAPI()
    {
        printHeader("Search Food Online (USDA Database)");
        
        // Ask for food query
        string foodQuery;
        cout << CYAN << "Enter food to search: " << RESET;
        getline(cin, foodQuery);
        
        if (foodQuery.empty())
        {
            printError("Search query cannot be empty.");
            return;
        }
        
        // Replace spaces with %20 for URL encoding
        size_t pos = 0;
        while ((pos = foodQuery.find(" ", pos)) != string::npos) {
            foodQuery.replace(pos, 1, "%20");
            pos += 3; // length of "%20"
        }
        
        printInfo("Searching for \"" + foodQuery + "\" in USDA database...");
        
        // API key - should ideally be stored more securely
        string apiKey = "R9OFzkfROTGUedVA6omCI3g9dzoBQkgkkekOwiTj"; 
        string command = "curl -s \"https://api.nal.usda.gov/fdc/v1/foods/search?query=" + foodQuery + "&api_key=" + apiKey + "\"";

        array<char, 8192> buffer;
        string result;

        // Execute the curl command
        unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe)
        {
            printError("Failed to run curl command. Check your internet connection.");
            return;
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }

        try
        {
            json response = json::parse(result);
    
            if (!response.contains("foods") || response["foods"].empty())
            {
                printError("No food items found for query: " + foodQuery);
                return;
            }
    
            for (const auto &food : response["foods"])
            {
                cout << "\n" << CYAN << "🔸 Description: " << RESET << food["description"] << "\n";
                cout << CYAN << "🔹 FDC ID: " << RESET << food["fdcId"] << "\n";
                cout << CYAN << "🔸 Nutrients:" << RESET << "\n";
    
                if (food.contains("foodNutrients"))
                {
                    for (const auto &nutrient : food["foodNutrients"])
                    {
                        if (nutrient.contains("nutrientName") && nutrient.contains("value") && nutrient.contains("unitName"))
                        {
                            cout << "   - " << nutrient["nutrientName"] << ": "
                                  << nutrient["value"] << " " << nutrient["unitName"] << "\n";
                        }
                    }
                }
                else
                {
                    printInfo("   (No nutrient data available)");
                }

                break;
            }
        }
        catch (const exception &e)
        {
            printError("Error parsing JSON response: " + string(e.what()));
        }
    }

    void handleFoodDatabaseMenu()
    {
        string choice;
        bool backToMainMenu = false;

        while (!backToMainMenu)
        {
            displayFoodDatabaseMenu();
            getline(cin, choice);

            if (choice == "1")
            {
                viewAllFoods();
            }
            else if (choice == "2")
            {
                searchFoods();
            }
            else if (choice == "3")
            {
                viewFoodDetails();
            }
            else if (choice == "4")
            {
                addBasicFood();
            }
            else if (choice == "5")
            {
                createCompositeFood();
            }
            else if (choice == "6")
            {
                if (foodManager.saveDatabase())
                {
                    cout << "Food database saved successfully.\n";
                }
                else
                {
                    cout << "Error saving food database.\n";
                }
            }
            else if (choice == "7")
            {
                searchOnlineAPI();
            }
            else if (choice == "8")
            {
                backToMainMenu = true;
            }
            else
            {
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }

    void handleDailyLogMenu()
    {
        string choice;
        bool backToMainMenu = false;

        while (!backToMainMenu)
        {
            displayDailyLogMenu();
            getline(cin, choice);

            if (choice == "1")
            {
                viewDailyLog();
            }
            else if (choice == "2")
            {
                addFoodToLog();
            }
            else if (choice == "3")
            {
                removeFoodFromLog();
            }
            else if (choice == "4")
            {
                logManager.undo();
            }
            else if (choice == "5")
            {
                changeDate();
            }
            else if (choice == "6")
            {
                viewCalorieSummary();
            }
            else if (choice == "7")
            {
                if (logManager.saveLog())
                {
                    cout << "Daily log saved successfully.\n";
                }
                else
                {
                    cout << "Error saving daily log.\n";
                }
            }
            else if (choice == "8")
            {
                backToMainMenu = true;
            }
            else
            {
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }

    void handleProfileMenu()
    {
        string choice;
        bool backToMainMenu = false;

        while (!backToMainMenu)
        {
            displayProfileMenu();
            getline(cin, choice);

            if (choice == "1")
            {
                viewProfile();
            }
            else if (choice == "2")
            {
                updateProfile();
            }
            else if (choice == "3")
            {
                changeCalculationMethod();
            }
            else if (choice == "4")
            {
                if (profileManager.saveProfile())
                {
                    cout << "Profile saved successfully.\n";
                }
                else
                {
                    cout << "Error saving profile.\n";
                }
            }
            else if (choice == "5")
            {
                backToMainMenu = true;
            }
            else
            {
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }

    void saveAll()
    {
        bool success = true;

        foodManager.saveDatabase();
        logManager.saveLog();
        profileManager.saveProfile();
        if (foodManager.isModified())
        {
            if (!foodManager.saveDatabase())
            {
                cout << "Error saving food database.\n";
                success = false;
            }
        }

        if (logManager.isModified())
        {
            if (!logManager.saveLog())
            {
                cout << "Error saving daily log.\n";
                success = false;
            }
        }

        if (profileManager.isModified())
        {
            if (!profileManager.saveProfile())
            {
                cout << "Error saving user profile.\n";
                success = false;
            }
        }

        if (success)
        {
            cout << "All data saved successfully.\n";
        }
    }

public:
    CLI(FoodManager &foodManager, LogManager &logManager, ProfileManager &profileManager)
        : foodManager(foodManager), logManager(logManager), profileManager(profileManager) {}

    void initialize()
    {
        if (!foodManager.loadDatabase())
        {
            cout << "Food database not found or empty. Creating new database.\n";
        }

        if (!logManager.loadLog())
        {
            cout << "Daily log not found or empty. Creating new log.\n";
        }

        if (!profileManager.loadProfile())
        {
            cout << "User profile not found. Please create a profile.\n";
        }
    }

    void run()
    {
        string choice;

        initialize();

        while (running)
        {
            displayMenu();
            getline(cin, choice);

            if (choice == "1")
            {
                handleFoodDatabaseMenu();
            }
            else if (choice == "2")
            {
                handleDailyLogMenu();
            }
            else if (choice == "3")
            {
                handleProfileMenu();
            }
            else if (choice == "4")
            {
                saveAll();
            }
            else if (choice == "5")
            {
                // Ask for confirmation if there's unsaved data
                if (foodManager.isModified() || logManager.isModified() || profileManager.isModified())
                {
                    cout << "You have unsaved changes. Save before exiting? (y/n): ";
                    string confirm;
                    getline(cin, confirm);

                    if (confirm == "y" || confirm == "Y")
                    {
                        saveAll();
                    }
                }

                cout << "Thank you for using YADA. Goodbye!\n";
                running = false;
            }
            else
            {
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }
};

#endif // YADA_CLI_H