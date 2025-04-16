#ifndef LOG_CPP
#define LOG_CPP

#include "food.cpp"
#include <ctime>
#include <stack>
using namespace std;

// Log Entry class
class LogEntry
{
private:
    string foodId;
    int servings;

public:
    LogEntry() : foodId(""), servings(0) {}
    LogEntry(const string &foodId, int servings)
        : foodId(foodId), servings(servings) {}

    string getFoodId() const { return foodId; }
    int getServings() const { return servings; }
    void addServings(int additionalServings) { servings += additionalServings; }

    json toJson() const
    {
        json j;
        j["foodId"] = foodId;
        j["servings"] = servings;
        return j;
    }

    static LogEntry fromJson(const json &j)
    {
        return LogEntry(j["foodId"].get<string>(), j["servings"].get<int>());
    }

    double getTotalCalories(const FoodManager &foodManager) const
    {
        auto food = foodManager.getFoodById(foodId);
        if (food)
        {
            return food->getCaloriesPerServing() * servings;
        }
        return 0.0;
    }
};

// Command interface for undo functionality
class Command
{
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// Daily Log class
class DailyLog
{
private:
    vector<LogEntry> entries;

public:
    const vector<LogEntry> &getEntries() const { return entries; }

    void addEntry(const LogEntry &entry)
    {
        for (auto &existingEntry : entries)
        {
            if (existingEntry.getFoodId() == entry.getFoodId())
            {
                existingEntry.addServings(entry.getServings());
                return;
            }
        }
        entries.push_back(entry);
    }

    void removeEntry(size_t index)
    {
        if (index < entries.size())
        {
            entries.erase(entries.begin() + index);
        }
    }

    double getTotalCalories(const FoodManager &foodManager) const
    {
        double total = 0.0;
        for (const auto &entry : entries)
        {
            total += entry.getTotalCalories(foodManager);
        }
        return total;
    }

    json toJson() const
    {
        json j = json::array();
        for (const auto &entry : entries)
        {
            j.push_back(entry.toJson());
        }
        return j;
    }

    static DailyLog fromJson(const json &j)
    {
        DailyLog log;
        for (const auto &entryJson : j)
        {
            log.addEntry(LogEntry::fromJson(entryJson));
        }
        return log;
    }
};

// Add food command
class AddFoodLogCommand : public Command
{
private:
    map<string, DailyLog> &logs;
    string date;
    string foodId;
    int servings;

public:
    AddFoodLogCommand(map<string, DailyLog> &logs, const string &date, const string &foodId, int servings)
        : logs(logs), date(date), foodId(foodId), servings(servings) {}

    void execute() override
    {
        logs[date].addEntry(LogEntry(foodId, servings));
    }

    void undo() override
    {
        auto &log = logs[date];
        auto &entries = const_cast<vector<LogEntry> &>(log.getEntries());

        for (auto it = entries.begin(); it != entries.end(); ++it)
        {
            if (it->getFoodId() == foodId)
            {
                it->addServings(-servings); // Reduce servings
                if (it->getServings() <= 0)
                {
                    entries.erase(it); // Remove entry if servings become zero
                }
                break;
            }
        }
    }
};

class RemoveFoodLogCommand : public Command
{
private:
    map<string, DailyLog> &logs;
    string date;
    size_t index;
    LogEntry removedEntry;

public:
    RemoveFoodLogCommand(map<string, DailyLog> &logs, const string &date, size_t index)
        : logs(logs), date(date), index(index)
    {
        if (logs.find(date) != logs.end() && index < logs[date].getEntries().size())
        {
            removedEntry = logs[date].getEntries()[index];
        }
    }

    void execute() override
    {
        if (logs.find(date) != logs.end())
        {
            logs[date].removeEntry(index);
        }
    }

    void undo() override
    {
        if (!removedEntry.getFoodId().empty())
        {
            logs[date].addEntry(removedEntry); // Restore the removed entry
        }
    }
};

// Log Manager class
class LogManager
{
private:
    map<string, DailyLog> logs;
    stack<shared_ptr<Command>> undoStack;
    string currentDate;
    FoodManager &foodManager;
    bool modified = false;

public:
    LogManager(FoodManager &foodManager) : foodManager(foodManager)
    {
        // Initialize current date
        time_t t = time(nullptr);
        tm timeinfo = *localtime(&t);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
        currentDate = string(buffer);
    }

    bool loadLog()
    {
        try
        {
            ifstream file("daily_logs.json");
            if (!file.is_open())
            {
                return false;
            }

            json j;
            file >> j;

            for (const auto &[date, logJson] : j.items())
            {
                logs[date] = DailyLog::fromJson(logJson);
            }

            modified = false;
            return true;
        }
        catch (exception &e)
        {
            cerr << "Error loading log: " << e.what() << endl;
            return false;
        }
    }

    bool saveLog()
    {
        try
        {
            json j;
            for (const auto &[date, log] : logs)
            {
                j[date] = log.toJson();
            }

            ofstream file("daily_logs.json");
            file << j.dump(2);

            modified = false;
            return true;
        }
        catch (exception &e)
        {
            cerr << "Error saving log: " << e.what() << endl;
            return false;
        }
    }

    void setCurrentDate(const string &date)
    {
        currentDate = date;
    }

    string getCurrentDate() const
    {
        return currentDate;
    }

    void addFoodToLog(const string &foodId, int servings)
    {
        auto command = make_shared<AddFoodLogCommand>(logs, currentDate, foodId, servings);
        command->execute();
        undoStack.push(command);
        modified = true;
    }

    void removeFoodFromLog(size_t index)
    {
        auto command = make_shared<RemoveFoodLogCommand>(logs, currentDate, index);
        command->execute();
        undoStack.push(command);
        modified = true;
    }

    void undo()
    {
        if (!undoStack.empty())
        {
            auto command = undoStack.top();
            undoStack.pop();
            command->undo();
            modified = true;
            cout << "Last action undone.\n";
        }
        else
        {
            cout << "Nothing to undo.\n";
        }
    }

    const DailyLog &getCurrentDayLog() const
    {
        auto it = logs.find(currentDate);
        if (it != logs.end())
        {
            return it->second;
        }

        // Return an empty log if not found
        static DailyLog emptyLog;
        return emptyLog;
    }

    double getTotalCaloriesForDay() const
    {
        auto it = logs.find(currentDate);
        if (it != logs.end())
        {
            return it->second.getTotalCalories(foodManager);
        }
        return 0.0;
    }

    bool isModified() const
    {
        return modified;
    }

    vector<string> getAllLogDates() const
    {
        vector<string> dates;
        for (const auto &[date, _] : logs)
        {
            dates.push_back(date);
        }
        // Sort dates in descending order (newest first)
        sort(dates.begin(), dates.end(), greater<string>());
        return dates;
    }
};

#endif