#include <string>
#include <vector>

#include "hurdlewords.h"
#include "hurdlestate.h"
#include "server_utils/crow_all.h"

#ifndef HURDLE_H
#define HURDLE_H

class HurdleGame {
 public:
  HurdleGame(HurdleWords words);

  void NewHurdle();
  void LetterEntered(char key);
  void WordSubmitted();
  void LetterDeleted();
  crow::json::wvalue JsonFromHurdleState();

 private:
  HurdleState hurdle_state_;
  HurdleWords hurdle_words_;

  void UpdateStatus();
  std::string CalculateColors(const std::string& guess);
};

#endif  // HURDLE_H
