#include "../include/file_operations.cuh"
#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

using json = nlohmann::json;

void writeFile(std::string filename, Bodies* bodies, float seconds, int n, bool append) {
  json jsonString;

  jsonString["time"] = seconds;
  jsonString["bodies"] = json::array();

  for (int i = 0; i < n; i++) {
    json body = {
        {"id", i},
        {"p", {{"x", bodies->position[i].x}, {"y", bodies->position[i].y}}},
        {"v", {{"x", bodies->velocity[i].x}, {"y", bodies->velocity[i].y}}},
        {"a", {{"x", bodies->acceleration[i].x}, {"y", bodies->acceleration[i].y}}},
        {"m", *bodies->mass}
    };
    jsonString["bodies"].push_back(body);
  }

  if (append) {
    json existingData;

    std::ifstream file(filename, std::ios::in);
    if (file.is_open()) {
      file >> existingData;
      file.close();
    }

    existingData.push_back(jsonString);
    jsonString = existingData;
  } else {  // If we are not appending, we need to wrap the json object in an array
    json array = json::array();
    array.push_back(jsonString);
    jsonString = array;
  }

  std::ofstream file(filename, std::ios::out);
  if (!file.is_open()) {
    std::cout << "Error opening file." << std::endl;
    return;
  }

  file << jsonString.dump(2);

  file.close();
}