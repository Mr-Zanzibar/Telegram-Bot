#include <iostream>
#include <tgbot/tgbot.h>
#include <cpr/cpr.h>

const std::string WEATHER_COMMAND = "/weather";
const std::string API_KEY = "6532d6a2ab5148eb8b202230192606";
const std::string API_URL = "https://api.weather.com/v2/pws/observations/current?stationId=KCASANFR58&format=json&units=m&apiKey=" + API_KEY;

void sendWeatherInformation(TgBot::Bot& bot, TgBot::Message::Ptr message, const std::string& location) {
    auto response = cpr::Get(cpr::Url{API_URL});

    if (response.status_code != 200) {
        bot.getApi().sendMessage(message->chat->id, "API CALL ERROR");
        return;
    }

    bot.getApi().sendMessage(message->chat->id, "the weather in " + location + " is: " + response.text);
}

int main() {
    TgBot::Bot bot("YOUR_BOT_TOKEN");

    bot.getEvents().onCommand(WEATHER_COMMAND, [&bot](TgBot::Message::Ptr message) {
        if (message->text.length() < WEATHER_COMMAND.length() + 2) {
            bot.getApi().sendMessage(message->chat->id, "Invalid Location");
            return;
        }

        std::string location = message->text.substr(WEATHER_COMMAND.length() + 1);
        sendWeatherInformation(bot, message, location);
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
