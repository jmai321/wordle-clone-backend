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

// Check that the "answers" key exists in `game_state_json` with the given
// value.
void CheckAnswer(json game_state_json, const std::string& expected) {
  ASSERT_TRUE(game_state_json.contains("answer"))
      << "JsonFromHurdleState should return a JSON object with the answer set.";
  ASSERT_TRUE(game_state_json.at("answer").is_string())
      << "JsonFromHurdleState: answer should be of type string.";
  std::string answer = game_state_json.at("answer");
  ASSERT_EQ(answer, expected)
      << "The secret Hurdle answer should be " << expected;
}

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

// Guessing 'drawn' should have no correct letters when the correct answer is
// 'light'
TEST(HurdleGame, BoardColorsAllIncorrectLettersInGuess) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterEntered('d');
  game.LetterEntered('r');
  game.LetterEntered('a');
  game.LetterEntered('w');
  game.LetterEntered('n');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BBBBB")
      << "Correct Hurdle: light\nGuess made: drawn\nboardColors should be "
         "BBBBB when the guess has no correct letters.";
}

// Guessing 'plays' should have a yellow letter when the correct answer is
// 'light'
TEST(HurdleGame, BoardColorsCorrectLetterWrongPlace) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterEntered('p');
  game.LetterEntered('l');
  game.LetterEntered('a');
  game.LetterEntered('y');
  game.LetterEntered('s');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BYBBB")
      << "Correct Hurdle: light\nGuess made: plays\nboardColors should be "
         "BYBBB when the guess has a correct letter in the wrong position.";
}

// Guessing 'gloss' should have 2 yellow letters when the correct answer is
// 'light'
TEST(HurdleGame, BoardColorsCorrectLettersWrongPlace) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterEntered('g');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game.LetterEntered('s');
  game.LetterEntered('s');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "YYBBB")
      << "Correct Hurdle: light\nGuess made: gloss\nboardColors should be "
         "YYBBB when the guess has two correct letters in the wrong position.";
}

// Guessing 'light' should have all green letters when the correct answer is
// 'light'
TEST(HurdleGame, BoardColorsAllCorrectLettersInGuess) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterEntered('l');
  game.LetterEntered('i');
  game.LetterEntered('g');
  game.LetterEntered('h');
  game.LetterEntered('t');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "GGGGG")
      << "Correct Hurdle: light\nGuess made: light\nboardColors should be "
         "GGGGG when the guess is correct.";
}

TEST(HurdleGame, BoardColorsMultipleGuesses) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterEntered('d');
  game.LetterEntered('r');
  game.LetterEntered('a');
  game.LetterEntered('w');
  game.LetterEntered('n');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BBBBB")
      << "Correct Hurdle: light\nGuess made: drawn\nboardColors should be "
         "BBBBB when the guess has no correct letters.";
  game.LetterEntered('p');
  game.LetterEntered('l');
  game.LetterEntered('a');
  game.LetterEntered('y');
  game.LetterEntered('s');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 2);
  std::string guess2 = game_state_json.at("boardColors").at(1);
  ASSERT_EQ(guess2, "BYBBB")
      << "Correct Hurdle: light\nGuess made: plays\nboardColors should be "
         "BYBBB when the guess has a correct letter in the wrong position.";
  game.LetterEntered('g');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game.LetterEntered('s');
  game.LetterEntered('s');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 3);
  std::string guess3 = game_state_json.at("boardColors").at(2);
  ASSERT_EQ(guess3, "YYBBB")
      << "Correct Hurdle: light\nGuess made: gloss\nboardColors should be "
         "YYBBB when the guess has two correct letters in the wrong position.";
  game.LetterEntered('l');
  game.LetterEntered('i');
  game.LetterEntered('g');
  game.LetterEntered('h');
  game.LetterEntered('t');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 4);
  std::string guess4 = game_state_json.at("boardColors").at(3);
  ASSERT_EQ(guess4, "GGGGG")
      << "Correct Hurdle: light\nGuess made: light\nboardColors should be "
         "GGGGG when the guess is correct.";
}

TEST(HurdleGame, BoardColorsAllIncorrectLettersInGuessForTuffy) {
  HurdleWords hurdlewords("tools/settings/data/tuffy.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckAnswer(game_state_json, "tuffy");
  game.LetterEntered('d');
  game.LetterEntered('r');
  game.LetterEntered('a');
  game.LetterEntered('w');
  game.LetterEntered('n');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BBBBB")
      << "Correct Hurdle: tuffy\nGuess made: drawn\nboardColors should be "
         "BBBBB when the guess has no correct letters.";
}

TEST(HurdleGame,
     BoardColorsDuplicateLettersInGuessShouldPrioritizeGreenLetters) {
  HurdleWords hurdlewords("tools/settings/data/tuffy.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckAnswer(game_state_json, "tuffy");
  game.LetterEntered('y');
  game.LetterEntered('u');
  game.LetterEntered('m');
  game.LetterEntered('m');
  game.LetterEntered('y');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BGBBG")
      << "Correct Hurdle: tuffy\nGuess made: yummy\nboardColors should be "
         "BGBBG when the guess has no correct letters.";
}

TEST(HurdleGame, BoardColorsDuplicateFLettersInGuessForTuffy) {
  HurdleWords hurdlewords("tools/settings/data/tuffy.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckAnswer(game_state_json, "tuffy");
  game.LetterEntered('s');
  game.LetterEntered('t');
  game.LetterEntered('a');
  game.LetterEntered('f');
  game.LetterEntered('f');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BYBGY")
      << "Correct Hurdle: tuffy\nGuess made: staff\nboardColors should be "
         "BYBGY when the guess has no correct letters.";
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
