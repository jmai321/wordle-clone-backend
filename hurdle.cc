#include "hurdle.h"

HurdleGame:: HurdleGame(HurdleWords words) : hurdle_words_(words) {
  NewHurdle();
}

void HurdleGame::NewHurdle() {
  std::string new_hurdle = hurdle_words_.GetRandomHurdle();
  hurdle_state_.SetHurdle(new_hurdle);
  hurdle_state_.ClearGuesses();
  hurdle_state_.SetColors({});
  hurdle_state_.SetErrorMessage("");
  UpdateStatus();
}

void HurdleGame::LetterEntered(char key) {
  hurdle_state_.SetErrorMessage("");
  std::vector<std::string>& guesses = hurdle_state_.GetMutableGuesses();
  std::vector<std::string>& colors = hurdle_state_.GetMutableColors();

  if (guesses.empty() || guesses.back().size() == 5) {
    // Start a new word if no words exist or the current word is completed
    guesses.push_back("");
    colors.push_back("");
  }

  std::string& currentGuess = guesses.back();
  std::string& currentColor = colors.back();

  if (currentGuess.size() < 5) {
    if (currentGuess.size() + 1 > 5) {
      hurdle_state_.SetErrorMessage("Word cannot exceed 5 letters");
    } else {
      currentGuess += key;
      currentColor = CalculateColors(currentGuess);
    }
  }
}


void HurdleGame::WordSubmitted() {
  std::vector<std::string>& guesses = hurdle_state_.GetMutableGuesses();
  std::vector<std::string>& colors = hurdle_state_.GetMutableColors();

  if (!guesses.empty()) {
    std::string guess = guesses.back();
    if (guess.length() != 5) {
      hurdle_state_.SetErrorMessage("Not Enough Letters");
      UpdateStatus();
      return;
    }
    if (!hurdle_words_.IsGuessValid(guess)) {
      hurdle_state_.SetErrorMessage("Not a valid guess");
      UpdateStatus();
      return;
    }
  }
  hurdle_state_.SetErrorMessage("");
  UpdateStatus();
}

void HurdleGame::LetterDeleted() {
  hurdle_state_.SetErrorMessage("");
  std::vector<std::string>& guesses = hurdle_state_.GetMutableGuesses();
  std::vector<std::string>& colors = hurdle_state_.GetMutableColors();
  if (!guesses.empty()) {
    std::string& last_guess = guesses.back();
    if (!last_guess.empty()) {
      last_guess.pop_back();
      colors.back().pop_back();
    } else {
      // Remove the empty guess if there are other guesses available
      if (guesses.size() > 1) {
        guesses.pop_back();
        colors.pop_back();
      }
    }
  }
  // If there are no guesses left, set an empty guess and color
  if (guesses.empty()) {
    guesses.push_back("");
    colors.push_back("");
  }
}

crow::json::wvalue HurdleGame::JsonFromHurdleState() {
  crow::json::wvalue hurdle_state_json({});

  hurdle_state_json["answer"] = hurdle_state_.GetHurdle();
  hurdle_state_json["boardColors"] = hurdle_state_.GetColors();
  hurdle_state_json["guessedWords"] = hurdle_state_.GetGuesses();
  hurdle_state_json["gameStatus"] = hurdle_state_.GetStatus();
  hurdle_state_json["errorMessage"] = hurdle_state_.GetErrorMessage();

  return hurdle_state_json;
}

void HurdleGame::UpdateStatus() {
  const std::vector<std::string>& guesses = hurdle_state_.GetGuesses();
  const std::string& hurdle = hurdle_state_.GetHurdle();
  
  if (guesses.empty()) {
    hurdle_state_.SetStatus("active");
    return;
  }
  
  if (std::find(guesses.begin(), guesses.end(), hurdle) != guesses.end()) {
    hurdle_state_.SetStatus("win");
    return;
  }
  
  if (guesses.size() >= 6) {
    hurdle_state_.SetStatus("lose");
  } else {
    hurdle_state_.SetStatus("active");
  }
}

std::string HurdleGame::CalculateColors(const std::string& guess) {
  const std::string hurdle = hurdle_state_.GetHurdle();
  std::string colors(guess.length(), 'B');

  if (guess.length() > hurdle.length()) {
    return colors;
  }

  std::unordered_map<char, int> hurdleCharCount;
  std::unordered_map<char, int> guessCharCount;

  for (char h : hurdle) {
    hurdleCharCount[h]++;
  }

  for (char g : guess) {
    guessCharCount[g]++;
  }

  size_t i = 0;
  for (char g : guess) {
    if (g == hurdle[i]) {
      colors[i] = 'G';
      guessCharCount[g]--;
      hurdleCharCount[g]--;
    }
    i++;
  }

  i = 0;
  for (char g : guess) {
    if (colors[i] != 'G' && guessCharCount[g] > 0 && hurdleCharCount[g] > 0) {
      colors[i] = 'Y';
      guessCharCount[g]--;
      hurdleCharCount[g]--;
    }
    i++;
  }

  return colors;
}