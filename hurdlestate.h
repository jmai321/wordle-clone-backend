#pragma once

#include <string>
#include <vector>

#ifndef HURDLESTATE_H
#define HURDLESTATE_H

class HurdleState {
 public:
  HurdleState();

  std::string GetHurdle() const;
  void SetHurdle(const std::string& hurdle);

  std::vector<std::string> GetColors() const;
  std::vector<std::string>& GetMutableColors();
  void SetColors(const std::vector<std::string>& colors);
  void ClearColors();

  std::vector<std::string> GetGuesses() const;
  std::vector<std::string>& GetMutableGuesses();
  void SetGuesses(const std::vector<std::string>& guesses);
  void ClearGuesses();

  std::string GetStatus() const;
  void SetStatus(const std::string& status);

  std::string GetErrorMessage() const;
  void SetErrorMessage(const std::string& errorMessage);

 private:
  std::string hurdle_;
  std::vector<std::string> colors_;
  std::vector<std::string> guesses_;
  std::string status_;
  std::string errorMessage_;

};

#endif  // HURDLESTATE_H