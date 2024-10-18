# Weather Data Aggregator and Alert System

## Overview

This project is a weather data aggregator and alert system that fetches current weather data for multiple cities, calculates daily summaries, and sends alerts when the temperature exceeds a certain threshold. The system uses the OpenWeatherMap API to fetch weather data and stores the data in a MongoDB database.

## Features

- Fetches current weather data for multiple cities using the OpenWeatherMap API
- Calculates daily summaries, including average temperature, max temperature, min temperature, and dominant weather condition
- Stores weather data and daily summaries in a MongoDB database
- Sends alerts when the temperature exceeds a certain threshold

## Requirements

- C++ compiler (e.g., GCC)
- OpenWeatherMap API key
- MongoDB installation (local or remote)
- `curl` library for making HTTP requests
- `nlohmann/json` library for JSON parsing
- `mongocxx` library for MongoDB interactions

## Installation

1. Clone the repository: `git clone https://github.com/utkarsh462255/Zeotap.git`
2. Install dependencies:
   - `sudo apt-get install libcurl4-openssl-dev` (for `curl` library)
   - `sudo apt-get install libmongoc-dev` (for `mongocxx` library)
   - `sudo apt-get install nlohmann-json-dev` (for `nlohmann/json` library)
3. Compile the code: `g++ -std=c++11 -o weather_data_aggregator main.cpp -lcurl -lmongocxx -lnlohmann_json`
4. Create a MongoDB database and collection for storing weather data and daily summaries

## Usage

1. Replace `your_openweathermap_api_key` with your actual OpenWeatherMap API key in `main.cpp`
2. Update the `cities` vector in `main.cpp` with the cities for which you want to fetch weather data
3. Run the program: `./weather_data_aggregator`
4. The program will fetch weather data, calculate daily summaries, and store the data in the MongoDB database
5. The program will also send alerts when the temperature exceeds the threshold specified in `main.cpp`

## API Documentation

### WeatherDataFetcher

- `fetchWeatherData(const std::string& city, const std::string& apiKey)`: Fetches current weather data for a given city using the OpenWeatherMap API

### WeatherAggregator

- `calculateDailySummary(const std::vector<nlohmann::json>& dailyData)`: Calculates daily summaries, including average temperature, max temperature, min temperature, and dominant weather condition

### AlertManager

- `checkForAlert(double currentTemp, const double threshold)`: Sends alerts when the temperature exceeds a certain threshold

### MongoDBHandler

- `storeWeatherData(const nlohmann::json& data)`: Stores weather data in the MongoDB database
- `storeDailySummary(const WeatherAggregator::WeatherSummary& summary)`: Stores daily summaries in the MongoDB database

## Commit Messages

- Follow the standard commit message format: `<type>(<scope>): <subject>`
- Use the following types:
  - `feat`: New feature
  - `fix`: Bug fix
  - `docs`: Documentation update
  - `style`: Code style update
  - `refactor`: Code refactoring
  - `perf`: Performance improvement
  - `test`: Test update
  - `chore`: Miscellaneous update

## License

This project is licensed under the MIT License. See `LICENSE` for details.

## Acknowledgments

- OpenWeatherMap API for providing weather data
- MongoDB for providing a NoSQL database solution
- `curl` library for making HTTP requests
- `nlohmann/json` library for JSON parsing
- `mongocxx` library for MongoDB interactions