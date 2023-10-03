#include <string>
#include <unordered_set>
#include <vector>

#ifndef HURDLEWORDS_H
#define HURDLEWORDS_H

class HurdleWords {
 public:
  HurdleWords(const std::string &valid_hurdles_filename,
             const std::string &valid_guesses_filename);

  // Returns true if the given word is a valid guess
  // according to the words in this class.
  bool IsGuessValid(const std::string &word) const;

  // Returns a random hurdle from the list of valid
  // hurdles (secret words) stored in the words in this class.
  const std::string &GetRandomHurdle() const;

 private:
  // valid_hurdles_ is a list of valid words from which the
  // secret hurdle may be chosen.
  std::vector<std::string> valid_hurdles_;
  // valid_guesses_ is a list of valid words that are considered
  // acceptable gueses a user can submit. We use a set here because
  // checking if a word is in a set is more time efficient than a vector.
  std::unordered_set<std::string> valid_guesses_;
};

#endif  // HURDLEWORDS_H