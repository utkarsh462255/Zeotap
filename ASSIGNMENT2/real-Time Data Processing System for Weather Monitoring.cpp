#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <unordered_map>
#include <algorithm>

// WeatherDataFetcher class
class WeatherDataFetcher {
public:
    static nlohmann::json fetchWeatherData(const std::string& city, const std::string& apiKey) {
        CURL* curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if(curl) {
            std::string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }

        return nlohmann::json::parse(readBuffer);
    }

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};

// WeatherAggregator class
class WeatherAggregator {
public:
    struct WeatherSummary {
        double averageTemp;
        double maxTemp;
        double minTemp;
        std::string dominantCondition;
    };

    WeatherSummary calculateDailySummary(const std::vector<nlohmann::json>& dailyData) {
        double sumTemp = 0, maxTemp = -1e9, minTemp = 1e9;
        std::unordered_map<std::string, int> conditionCount;
        for (const auto& entry : dailyData) {
            double temp = entry["main"]["temp"].get<double>() - 273.15; // Convert from Kelvin to Celsius
            sumTemp += temp;
            maxTemp = std::max(maxTemp, temp);
            minTemp = std::min(minTemp, temp);
            std::string condition = entry["weather"][0]["main"].get<std::string>();
            conditionCount[condition]++;
        }

        std::string dominantCondition = std::max_element(conditionCount.begin(), conditionCount.end(),
                                                          [](const auto& a, const auto& b) { return a.second < b.second; })->first;

        return {sumTemp / dailyData.size(), maxTemp, minTemp, dominantCondition};
    }
};

// AlertManager class
class AlertManager {
public:
    void checkForAlert(double currentTemp, const double threshold) {
        if (currentTemp > threshold) {
            std::cout << "Alert: Temperature exceeds threshold!" << std::endl;
            // Additional code to send email notifications or logs
        }
    }
};

// MongoDBHandler class
class MongoDBHandler {
public:
    MongoDBHandler() {
        mongocxx::instance instance{};
        client = mongocxx::client{mongocxx::uri{"mongodb://localhost:27017"}};
        db = client["weatherDB"];
    }

    void storeWeatherData(const nlohmann::json& data) {
        auto collection = db["rawData"];
        bsoncxx::document::value doc_value = bsoncxx::from_json(data.dump());
        collection.insert_one(doc_value.view());
    }

    void storeDailySummary(const WeatherAggregator::WeatherSummary& summary) {
        auto collection = db["dailySummaries"];
        bsoncxx::builder::stream::document document{};
        document << "averageTemp" << summary.averageTemp
                 << "maxTemp" << summary.maxTemp
                 << "minTemp" << summary.minTemp
                 << "dominantCondition" << summary.dominantCondition;
        collection.insert_one(document.view());
    }

private:
    mongocxx::client client;
    mongocxx::database db;
};

// Main function
int main() {
    std::string apiKey = "your_openweathermap_api_key"; // Replace with your actual API key
    std::vector<std::string> cities = {"Delhi", "Mumbai", "Chennai", "Bangalore", "Kolkata", "Hyderabad"};
    double alertThreshold = 35.0; // Threshold temperature for alerts in Celsius

    MongoDBHandler dbHandler;
    WeatherAggregator aggregator;
    AlertManager alertManager;

    // Fetch weather data for each city
    std::vector<nlohmann::json> dailyData;
    for (const auto& city : cities) {
        auto data = WeatherDataFetcher::fetchWeatherData(city, apiKey);
        dbHandler.storeWeatherData(data);
        dailyData.push_back(data);
        double currentTemp = data["main"]["temp"].get<double>() - 273.15; // Convert to Celsius
        alertManager.checkForAlert(currentTemp, alertThreshold);
    }

    // Calculate and store daily summary
    auto summary = aggregator.calculateDailySummary(dailyData);
    dbHandler.storeDailySummary(summary);

    // Output daily summary
    std::cout << "Daily Summary:\n"
              << "Average Temperature: " << summary.averageTemp << " °C\n"
              << "Max Temperature: " << summary.maxTemp << " °C\n"
              << "Min Temperature: " << summary.minTemp << " °C\n"
              << "Dominant Condition: " << summary.dominantCondition << "\n";

    return 0;
}
