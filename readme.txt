YADA (Yet Another Diet Assistant)

How to Run the Program

Prerequisites:
- The program uses the `nlohmann/json` library for JSON parsing. Ensure the `json.hpp` file is included in the same directory as the source code.

Steps to Compile:
- Compile using: g++ main.cpp
- Run using: ./a.out

Features and How to Use Them

1. Food Database Management

- View All Foods:
  - Select option `1` from the "Food Database Menu".
- Search Foods by Keywords:
  - Select option `2` from the "Food Database Menu".
  - Enter keywords (comma-separated) and specify match type (all/any).
- View Food Details:
  - Select option `3` from the "Food Database Menu".
  - Enter the food ID.
- Add New Basic Food:
  - Select option `4` from the "Food Database Menu".
  - Follow the prompts to enter the food's details.
- Create Composite Food:
  - Select option `5` from the "Food Database Menu".
  - Follow the prompts to add components and servings.
- Save Database:
  - Select option `6` from the "Food Database Menu".

2. Daily Consumption Logging

- View Daily Log:
  - Select option `1` from the "Daily Log Menu".
- Add Food to Log:
  - Select option `2` from the "Daily Log Menu".
- Remove Food from Log:
  - Select option `3` from the "Daily Log Menu".
- Undo Last Action:
  - Select option `4` from the "Daily Log Menu".
- Change Current Date:
  - Select option `5` from the "Daily Log Menu".
- View Calorie Summary:
  - Select option `6` from the "Daily Log Menu".
- Save Log:
  - Select option `7` from the "Daily Log Menu".

3. User Profile Management

- View Profile:
  - Select option `1` from the "User Profile Menu".
- Update Profile:
  - Select option `2` from the "User Profile Menu".
- Change Calorie Calculation Method:
  - Select option `3` from the "User Profile Menu".
- Save Profile:
  - Select option `4` from the "User Profile Menu".

4. Save All Data

- Select option `4` from the main menu to save all changes to food database, daily log, and user profile.

5. Exit the Program

- Select option `5` from the main menu.
- You will be prompted to save if there are unsaved changes.

Notes

- The program stores data in the following files:
  - basic_foods.json
  - composite_foods.json
  - daily_logs.json
  - user_profile.json
- Ensure these files are in the same directory as the program to load existing data.

Example Usage

1. Start the program.

2. Add a new basic food:
   - Navigate to the "Food Database Menu" and select option `4`.
   - Enter the food ID, description, keywords, and nutritional values.

3. Log the food in your daily log:
   - Navigate to the "Daily Log Menu" and select option `2`.
   - Enter the food ID and servings.

4. View your calorie summary:
   - Navigate to the "Daily Log Menu" and select option `6`.

5. Update your profile:
   - Navigate to the "User Profile Menu" and select option `2`.
   - Enter your gender, height, weight, age, and activity level.
