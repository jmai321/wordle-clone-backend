#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "../../hurdlewords.h"
#include "../../hurdlestate.h"
#include "../../hurdle.h"
#include "../cppaudit/gtest_ext.h"

using ::testing::HasSubstr;
using ::testing::Not;

TEST(HurdleWords, PublicMethodsPresent) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  hurdlewords.GetRandomHurdle();
  std::string guess = "tuffy";
  hurdlewords.IsGuessValid(guess);
}

TEST(HurdleWords, IsGuessValidTrueForValidGuess) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  std::string guess = "hello";
  ASSERT_TRUE(hurdlewords.IsGuessValid(guess))
      << "The word hello should be a valid guess.";
}

TEST(HurdleWords, IsGuessValidFalseForInvalidGuess) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  std::string guess = "zzzzz";
  ASSERT_FALSE(hurdlewords.IsGuessValid(guess))
      << "The word zzzzz should be a invalid guess.";
}

TEST(HurdleWords, IsGuessValidFalseForTooShortGuess) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  std::string guess = "ab";
  ASSERT_FALSE(hurdlewords.IsGuessValid(guess))
      << "A guess is invalid if it's less than 5 characters long.";
  guess = "";
  ASSERT_FALSE(hurdlewords.IsGuessValid(guess))
      << "A guess is invalid if it's less than 5 characters long.";
}

TEST(HurdleWords, IsGuessValidFalseForTooLongGuess) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  std::string guess = "ababab";
  ASSERT_FALSE(hurdlewords.IsGuessValid(guess))
      << "A guess is invalid if it's more than 5 characters long.";
  guess = "hellohello";
  ASSERT_FALSE(hurdlewords.IsGuessValid(guess))
      << "A guess is invalid if it's more than 5 characters long.";
}

TEST(HurdleWords, GetRandomHurdleValidLength) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  std::string hurdle1 = hurdlewords.GetRandomHurdle();
  ASSERT_EQ(hurdle1.length(), 5)
      << "A word returned by GetRandomHurdle should be 5 characters long.";
}

TEST(HurdleWords, GetRandomHurdleSubsequentWordsAreDifferent) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  std::string hurdle1 = hurdlewords.GetRandomHurdle();
  std::string hurdle2 = hurdlewords.GetRandomHurdle();
  std::string hurdle3 = hurdlewords.GetRandomHurdle();
  ASSERT_NE(hurdle1, hurdle2)
      << "Subsequent calls to GetRandomHurdle should return different words.";
  ASSERT_NE(hurdle2, hurdle3)
      << "Subsequent calls to GetRandomHurdle should return different words.";
}

TEST(HurdleWords, RandomHurdleIsGuessValid) {
  HurdleWords hurdlewords("data/valid_hurdles.txt", "data/valid_guesses.txt");
  std::string hurdle = hurdlewords.GetRandomHurdle();
  ASSERT_TRUE(hurdlewords.IsGuessValid(hurdle))
      << "The word " << hurdle << " should be a valid guess.";
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  ::testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
