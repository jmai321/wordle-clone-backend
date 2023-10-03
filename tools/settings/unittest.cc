#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "../../hurdlestate.h"
#include "../../hurdle.h"
#include "../../hurdlewords.h"
#include "../../server_utils/crow_all.h"
#include "../cppaudit/gtest_ext.h"

using ::testing::HasSubstr;
using ::testing::Not;
using json = nlohmann::json;

// Check that the "boardColors" key exists in `game_state_json` with the
// expected size.
void CheckBoardColors(json game_state_json, int expected_size) {
  ASSERT_TRUE(game_state_json.contains("boardColors"))
      << "JsonFromHurdleState should return a JSON object with the boardColors "
         "set.";
  ASSERT_TRUE(game_state_json.at("boardColors").is_array())
      << "JsonFromHurdleState: boardColors should be a vector.";
  ASSERT_EQ(game_state_json.at("boardColors").size(), expected_size)
      << "JsonFromHurdleState: boardColors should be a vector of size "
      << expected_size;
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

TEST(HurdleGame, JsonFromHurdleStateHasExpectedKeys) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  crow::json::wvalue game_state = game.JsonFromHurdleState();
  std::vector<std::string> keys = game_state.keys();
  ASSERT_TRUE(std::find(keys.begin(), keys.end(), "answer") != keys.end())
      << "JsonFromHurdleState should return a JSON object with the answer set.";
  ASSERT_TRUE(std::find(keys.begin(), keys.end(), "boardColors") != keys.end())
      << "JsonFromHurdleState should return a JSON object with the boardColors "
         "set.";
  ASSERT_TRUE(std::find(keys.begin(), keys.end(), "guessedWords") != keys.end())
      << "JsonFromHurdleState should return a JSON object with the guessedWords "
         "set.";
  ASSERT_TRUE(std::find(keys.begin(), keys.end(), "gameStatus") != keys.end())
      << "JsonFromHurdleState should return a JSON object with the gameStatus set.";
}

TEST(HurdleGame, JsonFromHurdleStateHasAnswerWhenNewHurdle) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  ASSERT_TRUE(game_state_json.contains("answer"))
      << "JsonFromHurdleState should return a JSON object with the answer set.";
  ASSERT_TRUE(game_state_json.at("answer").is_string())
      << "JsonFromHurdleState: answer should be of type string.";
  std::string answer = game_state_json.at("answer");
  ASSERT_EQ(answer.length(), 5)
      << "The secret Hurdle answer should be 5 characters long.";
}

TEST(HurdleGame, JsonFromHurdleStateHasBoardColorsWhenNewHurdle) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 0);
}

TEST(HurdleGame, JsonFromHurdleStateHasGuessedWordsWhenNewHurdle) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  ASSERT_TRUE(game_state_json.contains("guessedWords"))
      << "JsonFromHurdleState should return a JSON object with the guessedWords "
         "set.";
  ASSERT_TRUE(game_state_json.at("guessedWords").is_array())
      << "JsonFromHurdleState: guessedWords should be a vector.";
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
