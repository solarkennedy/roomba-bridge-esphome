#include "reconfig_buttons.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

static const char *const TAG = "IrobotBridge.button";

namespace esphome {
namespace irobot_bridge {

void StartRoombaButton::press_action() { this->parent_->start_roomba_action(); }
void StopRoombaButton::press_action() { this->parent_->stop_roomba_action(); }
void PauseRoombaButton::press_action() { this->parent_->pause_roomba_action(); }
void ResumeRoombaButton::press_action() { this->parent_->resume_roomba_action(); }

}  // namespace irobot_bridge
}  // namespace esphome
