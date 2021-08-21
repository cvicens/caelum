#!/bin/sh
PORT=$(cat .vscode/arduino.json | jq -r .port)
BOARD=$(cat .vscode/arduino.json | jq -r .board)

arduino-cli compile -p ${PORT} --fqbn ${BOARD} .