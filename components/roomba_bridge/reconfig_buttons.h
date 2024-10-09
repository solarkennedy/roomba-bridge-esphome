#pragma once

#include "esphome/core/helpers.h"
#include "esphome/components/button/button.h"
#include "roomba_bridge.h"

namespace esphome {
namespace roomba_bridge {

class StartRoombaButton : public button::Button, public Parented<Roomba_Bridge> {
 public:
  StartRoombaButton() = default;

 protected:
  void press_action() override;
};

class StopRoombaButton : public button::Button, public Parented<Roomba_Bridge> {
 public:
  StopRoombaButton() = default;

 protected:
  void press_action() override;
};

class PauseRoombaButton : public button::Button, public Parented<Roomba_Bridge> {
 public:
  PauseRoombaButton() = default;

 protected:
  void press_action() override;
};

class ResumeRoombaButton : public button::Button, public Parented<Roomba_Bridge> {
 public:
  ResumeRoombaButton() = default;

 protected:
  void press_action() override;
};

class DockRoombaButton : public button::Button, public Parented<Roomba_Bridge> {
 public:
  DockRoombaButton() = default;

 protected:
  void press_action() override;
};

class FindRoombaButton : public button::Button, public Parented<Roomba_Bridge> {
 public:
  FindRoombaButton() = default;

 protected:
  void press_action() override;
};

class EvacRoombaButton : public button::Button, public Parented<Roomba_Bridge> {
 public:
  EvacRoombaButton() = default;

 protected:
  void press_action() override;
};

class TrainRoombaButton : public button::Button, public Parented<Roomba_Bridge> {
 public:
  TrainRoombaButton() = default;

 protected:
  void press_action() override;
};

}  // namespace ld2420
}  // namespace esphome
