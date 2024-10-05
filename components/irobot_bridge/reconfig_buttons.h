#pragma once

#include "esphome/core/helpers.h"
#include "esphome/components/button/button.h"
#include "irobot_bridge.h"

namespace esphome {
namespace irobot_bridge {

class StartRoombaButton : public button::Button, public Parented<Irobot_Bridge> {
 public:
  StartRoombaButton() = default;

 protected:
  void press_action() override;
};

class StopRoombaButton : public button::Button, public Parented<Irobot_Bridge> {
 public:
  StopRoombaButton() = default;

 protected:
  void press_action() override;
};

class PauseRoombaButton : public button::Button, public Parented<Irobot_Bridge> {
 public:
  PauseRoombaButton() = default;

 protected:
  void press_action() override;
};

class ResumeRoombaButton : public button::Button, public Parented<Irobot_Bridge> {
 public:
  ResumeRoombaButton() = default;

 protected:
  void press_action() override;
};

}  // namespace ld2420
}  // namespace esphome
