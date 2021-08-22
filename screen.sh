#!/bin/sh
COM=$(cat .vscode/arduino.json | jq -r .port)
PORT=115200

screen ${COM} ${PORT}