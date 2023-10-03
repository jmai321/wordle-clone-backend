#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "../../hurdlewords.h"
#include "../../hurdlestate.h"
#include "../../server_utils/crow_all.h"
#include "../../hurdle.h"
#include "../cppaudit/gtest_ext.h"

using ::testing::HasSubstr;
using ::testing::Not;
using json = nlohmann::json;

// Check that the "gameStatus" key exists in `game_state_json` with the given
// value.
void CheckGameStatus(json game_state_json, const std::string &expected,
                     const std::string &error_message) {
  ASSERT_TRUE(game_state_json.contains("gameStatus"))
      << "JsonFromHurdleState should return a JSON object with the gameStatus set.";
  ASSERT_TRUE(game_state_json.at("gameStatus").is_string())
      << "JsonFromHurdleState: gameStatus should be of type string.";
  std::string status = game_state_json.at("gameStatus");
  ASSERT_EQ(status, expected)
      << "The gameStatus should be " << expected << " " << error_message;
}

TEST(HurdleGame, PublicMethodsPresent) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('a');
  game.WordSubmitted();
  game.LetterDeleted();
  game.JsonFromHurdleState();
}

TEST(HurdleGame, JsonFromHurdleStateHasActiveGameStatusWhenNewHurdle) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGameStatus(game_state_json, "active", "when a new game has started.");
}

TEST(HurdleGame, JsonFromHurdleStateHasWinGameStatusWhenVictory) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('l');
  game.LetterEntered('i');
  game.LetterEntered('g');
  game.LetterEntered('h');
  game.LetterEntered('t');
  game.WordSubmitted();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGameStatus(game_state_json, "win",
                  "when the user correctly guesses the secret Hurdle.");
}

TEST(HurdleGame, JsonFromHurdleStateHasLoseGameStatusWhenSixIncorrectGuesses) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  for (int i = 0; i < 6; i++) {
    game.LetterEntered('h');
    game.LetterEntered('e');
    game.LetterEntered('l');
    game.LetterEntered('l');
    game.LetterEntered('o');
    game.WordSubmitted();
  }
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGameStatus(game_state_json, "lose",
                  "when a user guesses incorrectly six times.");
}

TEST(HurdleGame, NewHurdleAfterWinShouldResetGameStatusToActive) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('l');
  game.LetterEntered('i');
  game.LetterEntered('g');
  game.LetterEntered('h');
  game.LetterEntered('t');
  game.WordSubmitted();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGameStatus(game_state_json, "win",
                  "when the user correctly guesses the secret Hurdle.");
  game.NewHurdle();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGameStatus(game_state_json, "active", "when a new game has started.");
}

TEST(HurdleGame, NewHurdleAfterLossShouldResetGameStatusToActive) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  for (int i = 0; i < 6; i++) {
    game.LetterEntered('h');
    game.LetterEntered('e');
    game.LetterEntered('l');
    game.LetterEntered('l');
    game.LetterEntered('o');
    game.WordSubmitted();
  }
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGameStatus(game_state_json, "lose",
                  "when a user guesses incorrectly six times.");
  game.NewHurdle();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGameStatus(game_state_json, "active", "when a new game has started.");
}

TEST(HurdleGame,
     JsonFromHurdleStateHasWinGameStatusWhenVictoryExtraCharactersPressed) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('l');
  game.LetterEntered('i');
  game.LetterEntered('g');
  game.LetterEntered('h');
  game.LetterEntered('t');
  game.LetterEntered('t');
  game.LetterEntered('t');
  game.LetterEntered('t');
  game.LetterEntered('t');
  game.LetterEntered('t');
  game.LetterEntered('t');
  game.WordSubmitted();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGameStatus(game_state_json, "win",
                  "when the user correctly guesses the secret Hurdle.");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
