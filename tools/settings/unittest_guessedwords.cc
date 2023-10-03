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

// Check that the "guessedWords" key exists in `game_state_json` with the
// expected size.
void CheckGuessedWords(json game_state_json, int expected_size) {
  ASSERT_TRUE(game_state_json.contains("guessedWords"))
      << "JsonFromHurdleState should return a JSON object with the guessedWords"
         " set.";
  ASSERT_TRUE(game_state_json.at("guessedWords").is_array())
      << "JsonFromHurdleState: guessedWords should be a vector.";
  ASSERT_EQ(game_state_json.at("guessedWords").size(), expected_size)
      << "JsonFromHurdleState: guessedWords should be a vector of size "
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

TEST(HurdleGame, JsonFromHurdleStateHasGuessedWordsWhenNewHurdle) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 0);
}

TEST(HurdleGame, JsonFromHurdleStateHasGuessedWordsWhenOneLetterTyped) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  game.LetterEntered('d');
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
}

TEST(HurdleGame, JsonFromHurdleStateHasGuessedWordsWhenOneWordSubmitted) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game.WordSubmitted();
  game.LetterEntered('h');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 2);
}

TEST(HurdleGame, JsonFromHurdleStateHasGuessedWordsWhenMultipleWordsSubmitted) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  game.WordSubmitted();
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 2);
  game.WordSubmitted();
  game.LetterEntered('h');
  game.LetterEntered('e');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 3);
}

TEST(HurdleGame, JsonFromHurdleStateHasGuessedWordsAfterNewHurdleReset) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  game.WordSubmitted();
  game.LetterEntered('h');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 2);
  game.NewHurdle();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 0);
}

TEST(HurdleGame, PressingMoreThanFiveCharactersDoesNotChangeState) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  game.LetterEntered('s');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  game.LetterEntered('s');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
}

TEST(HurdleGame, GuessedWordsStoresPartiallyTypedWord) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HE")
      << "After typing 'h' and 'e', guessedWords should record that the word "
         "HE is a partially typed word.";
}

TEST(HurdleGame, GuessedWordsStoresOneWord) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HELLO")
      << "guessedWords should record that the word HELLO was typed as a guess.";
}

TEST(HurdleGame, PressingMoreThanFiveCharactersDoesNotChangeGuessedWord) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game.LetterEntered('s');
  game.LetterEntered('s');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HELLO")
      << "After typing 'helloss', the extra 'ss' is ignored and guessedWords "
         "should record the word HELLO.";
}

TEST(HurdleGame, GuessedWordsStoresMultipleSubmittedWords) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HELLO")
      << "guessedWords should record that the word HELLO was typed as a guess.";
  game.WordSubmitted();
  game.LetterEntered('n');
  game.LetterEntered('i');
  game.LetterEntered('g');
  game.LetterEntered('h');
  game.LetterEntered('t');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 2);
  std::string guess2 = game_state_json.at("guessedWords").at(1);
  std::for_each(guess2.begin(), guess2.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess2, "NIGHT")
      << "guessedWords should record that the word NIGHT was typed as a guess.";
}

TEST(HurdleGame, DeletingCharactersChangesGuessedWord) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game.LetterDeleted();
  game.LetterDeleted();
  game.LetterDeleted();
  game.LetterEntered('a');
  game.LetterEntered('r');
  game.LetterEntered('t');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HEART")
      << "After typing 'hello' and backspacing 3 times and typing 'art', "
         "guessedWords should record the word HEART.";
}

TEST(HurdleGame,
     DeletingCharactersAfterMultipleSubmittedWordsChangesGuessedWords) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game.LetterDeleted();
  game.LetterDeleted();
  game.LetterDeleted();
  game.LetterEntered('a');
  game.LetterEntered('r');
  game.LetterEntered('t');
  game.LetterEntered('s');
  game.LetterEntered('y');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HEART")
      << "After typing 'hello' and backspacing 3 times and typing 'artsy', "
         "guessedWords should record the word HEART.";
  game.WordSubmitted();
  game.LetterEntered('n');
  game.LetterEntered('i');
  game.LetterEntered('g');
  game.LetterEntered('h');
  game.LetterEntered('t');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 2);
  std::string guess2 = game_state_json.at("guessedWords").at(1);
  std::for_each(guess2.begin(), guess2.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess2, "NIGHT")
      << "guessedWords should record that the word NIGHT was typed as a guess.";
  game.WordSubmitted();
  game.LetterEntered('t');
  game.LetterDeleted();
  game.LetterEntered('c');
  game.LetterEntered('p');
  game.LetterEntered('p');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 3);
  std::string guess3 = game_state_json.at("guessedWords").at(2);
  std::for_each(guess3.begin(), guess3.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess3, "CPP")
      << "After typing 't', backspace, then 'cpp', guessedWords should record "
         "that the word CPP was typed as a guess.";
}

TEST(HurdleGame, SubmittingInvalidWordDoesNotChangeGuessedWords) {
  HurdleWords hurdlewords("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  HurdleGame game(hurdlewords);
  game.NewHurdle();
  json game_state_json = json::parse(game.JsonFromHurdleState().dump());
  game.LetterEntered('h');
  game.LetterEntered('e');
  game.WordSubmitted();
  game.LetterEntered('l');
  game.LetterEntered('l');
  game.LetterEntered('o');
  game.LetterDeleted();
  game.LetterDeleted();
  game.LetterDeleted();
  game.LetterEntered('a');
  game.LetterEntered('r');
  game.LetterEntered('t');
  game.LetterEntered('s');
  game.LetterEntered('y');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HEART")
      << "After typing 'hello' and backspacing 3 times and typing 'artsy', "
         "guessedWords should record the word HEART.";
  game.WordSubmitted();
  game.LetterEntered('n');
  game.LetterEntered('i');
  game.WordSubmitted();
  game.LetterEntered('g');
  game.LetterEntered('h');
  game.WordSubmitted();
  game.LetterEntered('t');
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 2);
  std::string guess2 = game_state_json.at("guessedWords").at(1);
  std::for_each(guess2.begin(), guess2.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess2, "NIGHT")
      << "guessedWords should record that the word NIGHT was typed as a guess.";
  game.WordSubmitted();
  game.LetterEntered('t');
  game.LetterDeleted();
  game.LetterEntered('c');
  game.LetterEntered('p');
  game.WordSubmitted();
  game.LetterEntered('p');
  game.WordSubmitted();
  game_state_json = json::parse(game.JsonFromHurdleState().dump());
  CheckGuessedWords(game_state_json, 3);
  std::string guess3 = game_state_json.at("guessedWords").at(2);
  std::for_each(guess3.begin(), guess3.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess3, "CPP")
      << "After typing 't', backspace, then 'cpp', guessedWords should record "
         "that the word CPP was typed as a guess.";
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
