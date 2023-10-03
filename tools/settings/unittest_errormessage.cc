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

// Check that the "errorMessage" key exists in `game_state_json`
void CheckErrorMessage(json game_state_json) {
  ASSERT_TRUE(game_state_json.contains("errorMessage"))
      << "JsonFromHurdleState should return a JSON object with the errorMessage "
         "set.";
  ASSERT_TRUE(game_state_json.at("errorMessage").is_string())
      << "JsonFromHurdleState: errorMessage should be of type string.";
  std::string error = game_state_json.at("errorMessage");
  ASSERT_TRUE(error.length() > 0)
      << "JsonFromHurdleState: errorMessage should be set to a message.";
}

// Check that there is no set error message.
void CheckEmptyOrNoErrorMessage(json game_state_json) {
  if (game_state_json.contains("errorMessage")) {
    ASSERT_TRUE(game_state_json.at("errorMessage").is_string())
        << "JsonFromHurdleState: errorMessage should be of type string.";
    std::string error = game_state_json.at("errorMessage");
    ASSERT_TRUE(error.length() == 0) << "JsonFromHurdleState: errorMessage should "
                                        "not be set when there is no error.";
  }
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

TEST(HurdleGame, JsonFromHurdleStateHasErrorMessageWhenNotEnoughLetters) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('z');
  game.LetterEntered('z');
  game.WordSubmitted();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckErrorMessage(game_state_json);
}

TEST(HurdleGame, JsonFromHurdleStateHasErrorMessageWhenInvalidGuess) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('z');
  game.LetterEntered('z');
  game.LetterEntered('z');
  game.LetterEntered('z');
  game.LetterEntered('z');
  game.WordSubmitted();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckErrorMessage(game_state_json);
}

TEST(HurdleGame, NoErrorMessageWhenNewHurdle) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckEmptyOrNoErrorMessage(game_state_json);
}

TEST(HurdleGame, NoErrorMessageWhenNoError) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('h');
  game.LetterEntered('i');
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckEmptyOrNoErrorMessage(game_state_json);
}

TEST(HurdleGame, ErrorMessageShouldBeEmptyWhenNoError) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.WordSubmitted();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckErrorMessage(game_state_json);
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckEmptyOrNoErrorMessage(game_state_json);
}

TEST(HurdleGame, NoErrorWhenSubmitThenErrorAfterTooFewLetters) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game.WordSubmitted();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckEmptyOrNoErrorMessage(game_state_json);
  game.LetterEntered('h');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckErrorMessage(game_state_json);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
