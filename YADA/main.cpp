#include "cli.cpp"
using namespace std;

int main()
{
    auto basicFoodFactory = make_shared<JsonBasicFoodFactory>();
    FoodManager foodManager(basicFoodFactory);
    LogManager logManager(foodManager);
    ProfileManager profileManager;

    CLI cli(foodManager, logManager, profileManager);
    cli.run();
}