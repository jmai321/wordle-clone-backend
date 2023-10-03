#include "hurdlestate.h"

HurdleState::HurdleState() : status_("active"), errorMessage_("") {}

std::string HurdleState::GetHurdle() const {
    return hurdle_;
}

void HurdleState::SetHurdle(const std::string& hurdle) {
    hurdle_ = hurdle;
}

std::vector<std::string> HurdleState::GetColors() const {
    return colors_;
}

std::vector<std::string>& HurdleState::GetMutableColors() {
    return colors_;
}

void HurdleState::SetColors(const std::vector<std::string>& colors) {
    colors_ = colors;
}

void HurdleState::ClearColors() {
    colors_.clear();
}

std::vector<std::string> HurdleState::GetGuesses() const {
    return guesses_;
}

std::vector<std::string>& HurdleState::GetMutableGuesses() {
    return guesses_;
}

void HurdleState::SetGuesses(const std::vector<std::string>& guesses) {
  guesses_ = guesses;
}

void HurdleState::ClearGuesses() {
    guesses_.clear();
}

std::string HurdleState::GetStatus() const {
    return status_;
}

void HurdleState::SetStatus(const std::string& status) {
    status_ = status;
}

std::string HurdleState::GetErrorMessage() const {
    return errorMessage_;
}

void HurdleState::SetErrorMessage(const std::string& errorMessage) {
    errorMessage_ = errorMessage;
}