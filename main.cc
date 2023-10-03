#include "hurdle.h"
#include "server_utils/crow_all.h"
#include "server_utils/sessions.h"

typedef SessionMiddleware<HurdleGame> GameMiddleware;

int main() {
  // Load the Hurdle words from the data/ folder.
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");

  // Initialize the Crow HTTP server.
  crow::App<crow::CORSHandler, GameMiddleware> app;

  // Initialize Cross-Origin Resource Sharing (CORS) to allow the frontend to
  // access the server.
  auto& cors_middleware = app.get_middleware<crow::CORSHandler>();
  cors_middleware.global().origin("*");
  cors_middleware.global().max_age(7200);  // Chrome's maximum

  // Initialize the session middleware to allow the server to keep track of
  // multiple games.
  auto& session_middleware = app.get_middleware<GameMiddleware>();
  session_middleware.header_name = "X-Hurdle-Game-ID";
  session_middleware.constructor = [&]() { return HurdleGame(hurdlewords); };

  // Every time a letter is pressed on the Hurdle frontend, that letter
  // is passed to this function, which is passed to LetterEntered.
  CROW_ROUTE(app, "/wordle_key_pressed/<string>")
  ([&](const crow::request& req, std::string s) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    game.LetterEntered(s.at(0));
    return game.JsonFromHurdleState();
  });

  // Every time the enter key is pressed on the Hurdle frontend,
  // the WordSubmitted function is called.
  CROW_ROUTE(app, "/enter_pressed")
  ([&](const crow::request& req) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    game.WordSubmitted();
    return game.JsonFromHurdleState();
  });

  // Every time the backspace key is pressed on the Hurdle frontend,
  // the LetterDeleted function is called.
  CROW_ROUTE(app, "/delete_pressed")
  ([&](const crow::request& req) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    game.LetterDeleted();
    return game.JsonFromHurdleState();
  });

  // Every time the "Next Hurdle" button is pressed on the Hurdle frontend,
  // the NewHurdle function is called.
  CROW_ROUTE(app, "/new_game")
  ([&](const crow::request& req) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    game.NewHurdle();
    return game.JsonFromHurdleState();
  });

  // When the Hurdle frontend is loaded, the GetGameState function is called to
  // load the initial state of the game that's currently stored.
  CROW_ROUTE(app, "/game")
  ([&](const crow::request& req) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    return game.JsonFromHurdleState();
  });

  app.port(18080).concurrency(1).run();
}
