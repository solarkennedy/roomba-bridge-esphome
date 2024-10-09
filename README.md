# Roomba Bridge (ESPHome Component)

This `Roomba Bridge` allows you to control your wifi-connected roomba using ESPHome.
If you flash ESPHome with this component to an ESP32 (for example), it uses the Roomba's local MQTT-based API and give you access to all its command functions.

Once in place, your Roomba no longer needs to connect to the internet, nor do you need the app!
You will have future-proofed your device and ensured it will not join the internet-of-shit.
(assuming you disconnect it from the internet to prevent future updates from breaking anything)

![alt text](https://github.com/solarkennedy/roomba-bridge-esphome/blob/master/examples/screenshot.png?raw=true)

**Note**: This is an [ESPHome External Component](https://esphome.io/components/external_components), which needs to run on a microcontroller you provide.
You may be interested in some other related work:

* [Home Assistant Roomba Integration](https://www.home-assistant.io/integrations/roomba/) - Runs on your exising HA installation
* [Roomba980-Python](https://github.com/NickWaterton/Roomba980-Python) - the Python library upon which the HA extension is based
* [Rest980](https://github.com/koalazak/rest980) - REST API for Roombas written in NodeJS
* [Dorita980](https://github.com/koalazak/dorita980) - Node library upon which Rest980 is based

## Getting Started

### Prereqs

First, get a microcontroller (ESP32) and install [ESPHome](https://esphome.io/) on it.
It MUST be able to reach your Roomba (usually this means being on the same wifi network).

Next you will need:

* Your Roomba's IP Address
* Your Roomba's [Username and Password](https://github.com/koalazak/dorita980?tab=readme-ov-file#how-to-get-your-usernameblid-and-password)
* [Optional] Your Roomba's [room and map configuration](https://github.com/koalazak/dorita980?tab=readme-ov-file#cleanroomargs) (`pmap_id`, `user_pmapv_id`, and `regions`)

Also, the lastest release of ESPHome has a bug which makes it crash on the large JSON payloads that Roombas produce.
This [Patch](https://github.com/esphome/esphome/pull/5786) is required. 

### Initial Configuration

Your minimal configuration will look something like this:

```
esphome:
  name: "roomba-bridge"
  platform: ESP32
  board: ...

wifi:
  ssid: "wifi_ssid"
  ...

mqtt:
  broker: 0.0.0.0

# Highly recommended to clone/save the files and reference them locally
# See https://esphome.io/components/external_components for more options
external_components:
  - source: ../irobot-bridge-esphome/components

roomba_bridge:
  - id: myroomba
    address: "192.168.0.42"
    blid: "FD...."
    password: ":1:ID:STRING_PLACEHOLDER"
```

### Room Configuration (optional)

If you want per-room controll, you can fetch your [room and map configuration](https://github.com/koalazak/dorita980?tab=readme-ov-file#cleanroomargs) (`pmap_id`, `user_pmapv_id`, and `regions`) and provide them in your YAML:

```
roomba_bridge:
  - id: myroomba
    address: "192.168.0.42"
    blid: "FD...."
    password: ":1:ID:STRING_PLACEHOLDER"
    pmap_id: "ABCDEFG123456FGKS789"
    user_pmapv_id: "190917T20125Z"
    regions:
    - { "region_id": "5", "region_name": "Hallway", "region_type": "hallway", "type": "rid"}
    - { "region_id": "0", "region_name": "living room", "region_type": "familiy room", "type": "rid"}
    - { "region_id": "1", "region_name": "kitchen", "region_type": "kitchen", "type": "rid"}
```

With those defined, you can create custom buttons to clean exactly the rooms you want:

```
button:
  - platform: template
    name: "Clean Hallway"
    id: clean_hallway
    on_press:
      then:
        - lambda: |-
            id(irobot_bridge_component).clean_room_by_id("5");

  - platform: template
    name: "Clean Living Room and Kitchen"
    id: clean_living_kitchen
    on_press:
      then:
        - lambda: |-
            std::vector<std::string> room_ids = {"0", "1"};
            id(irobot_bridge_component).clean_rooms_by_ids(room_ids);
```

### Advanced Configuration

With ESPHome, the limit really only is your imagination when it comes to connecting components together.

For example, I use a different ESP32 with [rf-bridge](https://esphome.io/components/rf_bridge.html) component, which puts codes onto MQTT, giving me a remote control to my roomba!

```
mqtt:
  broker: !secret mqtt_broker
  on_message:
    topic: "rf-bridge/rx-code"
    then:
      - lambda: |-
          if (x == "00bcac22") {
            id(start_roomba).press();
          } else if (x == "00bcac28") {
            id(stop_roomba).press();
          } else if (x == "00bcac24") {
            id(dock_roomba).press();
          }
```

I also recommend adding the `web_server` component for a basic web interface:

```
web_server:
  port: 80
  ota: false
  local: true
  version: 3
```

See `examples/` for more examples.

## Why?

I originally bought this internet-connected Roomba, simply accepting the tradeoff between depending on Irobot servers, depending on an App for functionality, the privacy implications, and the inevitable EOL of this product before it is physically incabable of cleaning.

Indeed I also have a "dumb" Roomba that bounces off the walls, and honestly it will probably outlive this fancy Roomba, simply because it doesn't have... x509 certs, 802.11, etc.

Also, robot vacuum security sucks.
Just see the [recent](https://www.abc.net.au/news/2024-10-04/robot-vacuum-hacked-photos-camera-audio/104414020) [news](https://www.technologyreview.com/2022/12/19/1065306/).

### Why Not Home Assistant? (HA)

I used to run HA, but eventually the server it ran on crashed and I lost it's automations and control.
It is too much power in once place to be a control plane.

Instead, I prefer to use ESPHome directly on devices, and build automation directly into the controller.
HA can still be a good dashboard, but I want the actual automation logic to be in the devices themselves.