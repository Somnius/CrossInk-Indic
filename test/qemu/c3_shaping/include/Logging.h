#pragma once
#include <Arduino.h>
// Minimal logging for the QEMU test firmware (the firmware's Logging pulls in board config).
#define LOG_ERR(origin, format, ...) Serial.printf("[ERR] [%s] " format "\n", origin, ##__VA_ARGS__)
#define LOG_INF(origin, format, ...) Serial.printf("[INF] [%s] " format "\n", origin, ##__VA_ARGS__)
#define LOG_DBG(origin, format, ...) Serial.printf("[DBG] [%s] " format "\n", origin, ##__VA_ARGS__)
