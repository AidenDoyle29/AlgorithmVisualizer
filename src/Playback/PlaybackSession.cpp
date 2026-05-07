#include "Playback/PlaybackSession.h"

namespace av::playback {

PlaybackSession::PlaybackSession(std::vector<algorithm::PlaybackFrame> frames) : frames_(std::move(frames)) {}

bool PlaybackSession::empty() const noexcept {
    return frames_.empty();
}

std::size_t PlaybackSession::frameCount() const noexcept {
    return frames_.size();
}

std::size_t PlaybackSession::currentIndex() const noexcept {
    return currentIndex_;
}

bool PlaybackSession::isPaused() const noexcept {
    return paused_;
}

bool PlaybackSession::isFinished() const noexcept {
    return !frames_.empty() && currentIndex_ + 1 >= frames_.size();
}

const algorithm::PlaybackFrame& PlaybackSession::currentFrame() const {
    if (frames_.empty()) {
        throw PlaybackError("no active playback session");
    }
    return frames_.at(currentIndex_);
}

void PlaybackSession::load(std::vector<algorithm::PlaybackFrame> frames) {
    frames_ = std::move(frames);
    currentIndex_ = 0;
    paused_ = true;
}

void PlaybackSession::pause() noexcept {
    paused_ = true;
}

void PlaybackSession::resume() noexcept {
    if (!frames_.empty()) {
        paused_ = false;
    }
}

void PlaybackSession::reset() noexcept {
    currentIndex_ = 0;
    paused_ = true;
}

bool PlaybackSession::stepForward() noexcept {
    if (frames_.empty() || currentIndex_ + 1 >= frames_.size()) {
        return false;
    }
    ++currentIndex_;
    return true;
}

bool PlaybackSession::stepBackward() noexcept {
    if (frames_.empty() || currentIndex_ == 0) {
        return false;
    }
    --currentIndex_;
    return true;
}

bool PlaybackSession::advanceTick() noexcept {
    if (paused_) {
        return false;
    }
    return stepForward();
}

}  // namespace av::playback
