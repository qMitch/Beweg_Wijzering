#ifndef MCP_HANDLER_H
#define MCP_HANDLER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include "main.h"

extern Adafruit_MCP23X17 mcp;

bool initMCP(int sda, int scl);
int getBinaryInput();

#endif
