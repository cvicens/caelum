#!/bin/sh
PORT=$(cat .vscode/arduino.json | jq -r .port)
BOARD=$(cat .vscode/arduino.json | jq -r .board)

arduino-cli upload -p ${PORT} --fqbn ${BOARD} .