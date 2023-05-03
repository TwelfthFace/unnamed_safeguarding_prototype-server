#pragma once
enum MessageType {
    TEXT = 0,
    LOCK_SCREEN = 1,
    UNLOCK_SCREEN = 2,
    SCREENSHOT = 3,
    SCREENSHOT_REQ = 4
};

struct Header {
  MessageType type;
  std::size_t size;
};

