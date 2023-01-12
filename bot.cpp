#include <iostream>
#include <tgbot/tgbot.h>
#include <cpr/cpr.h>

int main() {
    TgBot::Bot bot("YOUR_BOT_TOKEN");

    bot.getEvents().onCommand("weather", [&bot](TgBot::Message::Ptr message) {
        std::string location = message->text.substr(6);
        auto response = cpr::Get(cpr::Url{"https://api.weather.com/v2/pws/observations/current?stationId=KCASANFR58&format=json&units=m&apiKey=6532d6a2ab5148eb8b202230192606"});
        // Invia un messaggio all'utente con le informazioni sul tempo
        bot.getApi().sendMessage(message->chat->id, "Il tempo a " + location + " è: " + response.text);
    });

    try {
        std::cout << "Bot username: " << bot.getApi().getMe()->username << std::endl;
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
