#pragma once
enum MessageType { TEXT, SCREENSHOT, SCREENSHOT_REQ };

struct Header {
  MessageType type;
  std::size_t size;
};

