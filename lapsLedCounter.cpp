#include <iostream>
#include <fstream>
#include <json.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;
using namespace std;

int main() {
    // Path to your JSON file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    string filePath = "iracing-lap-chart-EXAMPLE.json";

    ifstream jsonFile(filePath);

    // Check if the file was opened successfully
    if (!jsonFile.is_open()) {
        cerr << "Error: Could not open file " << filePath << endl;
        return 1; // EXIT ERROR
    }

    // Parse JSON data
    json j;
    try {
        jsonFile >> j;
    }catch (const json::parse_error& e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
        return 1; // EXIT ERROR
    }

    unordered_map<int, string> teamIdClassMap;
    unordered_map<int, int> leaderLapNumIdMap;
    unordered_map<int, int> leaderLapNumPositionMap;

    string targetClass;
    int targetTeamId = 0;
    int leadingLapsCount = 0;
    int lapsCompleted = -1;

    // First pass: Determine each participating team's car's class.
    int teamCount = 0;
    for (const auto& lapEntry : j["lapData"]) {
        if (lapEntry.contains("finishing_position")) {
            int teamId = lapEntry["finishing_position"]["team_id"];
            string carClass = lapEntry["finishing_position"]["car_class_short_name"];
            string teamName = lapEntry["finishing_position"]["display_name"];
            teamIdClassMap[teamId] = carClass;
            teamCount++;
            cout << "P" << teamCount << " - " << carClass << " - " << teamName << " " << teamId << endl;
            if (lapEntry["finishing_position"]["laps_complete"] > lapsCompleted) {
                lapsCompleted = lapEntry["finishing_position"]["laps_complete"];
            }
        }
    }

    cout << "\nEnter the 6-digit team ID of which you want to count the led laps: ";
    cin >> targetTeamId;
    if (!teamIdClassMap.contains(-targetTeamId)) {
        cout << "Invalid team selected!" << endl;
        return 1;
    }

    targetClass = teamIdClassMap[-targetTeamId];

    for (int i = 1; i <= lapsCompleted; i++) {
        leaderLapNumIdMap[i] = 0;
        leaderLapNumPositionMap[i] = 100;
    }

    // Second pass: Determine leading team for the selected team's class, on each lap.
    int position = 0;
    for (const auto& lapEntry : j["lapData"]) {
        position++;
        for (int lapNum = 1; lapNum <= lapsCompleted; lapNum++) {
            string lap_to_check = "lap_" + to_string(lapNum);
            if (lapEntry.contains(lap_to_check)) {
                if (teamIdClassMap[lapEntry[lap_to_check]["group_id"]] == targetClass) {
                    if (leaderLapNumPositionMap[lapNum] > position) {
                        leaderLapNumPositionMap[lapNum] = position;
                        leaderLapNumIdMap[lapNum] = lapEntry[lap_to_check]["group_id"];
                    }
                }
            }
        }
    }

    // Count how many laps the selected team led it's class.
    for (auto& [lapNum, leaderId] : leaderLapNumIdMap) {
        if (leaderId == -targetTeamId) {
            leadingLapsCount++;
        }
    }

    cout << "\nSelected team led it's class for " << leadingLapsCount << " laps." << endl;

    return 0;
}
