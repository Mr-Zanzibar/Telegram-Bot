#include <iostream>
#include <tgbot/tgbot.h>
#include <cpr/cpr.h>
#include <json.hpp>

// forgot this lol

const std::string WEATHER_COMMAND = "/weather";
const std::string NEWS_COMMAND = "/news";
const std::string API_KEY = "6532d6a2ab5148eb8b202230192606";
const std::string API_URL = "https://api.weather.com/v2/pws/observations/current?stationId=KCASANFR58&format=json&units=m&apiKey=" + API_KEY;
const std::map<std::string, std::string> city_news = {
    {"rome", "https://www.republica.it"},
    {"berlin", "https://www.berliner-zeitung.de"},
    {"paris", "https://www.lefigaro.fr"},
    {"madrid", "https://elpais.com"}
};

std::string parseWeatherInformation(const std::string& response, bool useFahrenheit) {
  auto jsonResponse = nlohmann::json::parse(response);
  auto temperature = jsonResponse["observations"][0]["metric"]["temp"];
  auto humidity = jsonResponse["observations"][0]["metric"]["humidity"];

  if (useFahrenheit) {
    temperature = (temperature * 9.0 / 5.0) + 32.0;
  }

  return "Temperature: " + std::to_string(temperature) + (useFahrenheit ? "°F" : "°C") + ", Humidity: " + std::to_string(humidity) + "%";
}

void sendWeatherInformation(TgBot::Bot& bot, TgBot::Message::Ptr message, const std::string& location) {
auto response = cpr::Get(cpr::Url{API_URL});

if (response.status_code != 200) {
    bot.getApi().sendMessage(message->chat->id, "An error occurred while fetching weather information. Please try again later.");
    return;
}

try {
    auto weatherInfo = parseWeatherInformation(response.text);

    bot.getApi().sendMessage(message->chat->id, "The weather in " + location + " is: " + weatherInfo);
} catch (const std::exception& e) {
    bot.getApi().sendMessage(message->chat->id, "An error occurred while parsing weather information. Please try again later.");
    return;
}

}

void sendNewsInformation(TgBot::Bot& bot, TgBot::Message::Ptr message, const std::string& city) {
    auto it = city_news.find(city);
    if (it != city_news.end()) {
        bot.getApi().sendMessage(message->chat->id, "Here is the link to the latest news in " + city + ": " + it->second);
    } else {
        bot.getApi().sendMessage(message->chat->id, "Sorry, news for the specified city is not available.");
    }
}

int main() {
TgBot::Bot bot("YOUR_BOT_TOKEN");

bot.getEvents().onCommand(WEATHER_COMMAND, [&bot](TgBot::Message::Ptr message) {
  if (message->text.length() < WEATHER_COMMAND.length() + 2) {
    bot.getApi().sendMessage(message->chat->id, "Invalid location");
    return;
  }

  std::string location = message->text.substr(WEATHER_COMMAND.length() + 1);
  std::string args = location;
  bool useFahrenheit = false;

  // Check if the user wants to see the temperature in Fahrenheit
  size_t fahrenheitPos = location.find(" fahrenheit");
  if (fahrenheitPos != std::string::npos) {
    args = location.substr(0, fahrenheitPos);
    useFahrenheit = true;
  }

  sendWeatherInformation(bot, message, args, useFahrenheit);
});


try {
    std::cout << "Bot username: " << bot.getApi().getMe()->username << std::endl;
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
        longPoll.start();
    }
} catch (TgBot::TgException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}

return 0;

}
