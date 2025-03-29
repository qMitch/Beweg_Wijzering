#ifdef UNIT_TEST
#include <Arduino.h>
// Zorg dat variabelen ook beschikbaar zijn tijdens testen
extern String lastPlayedFile;
extern bool debugMode;
extern bool mcpConnected;
extern bool sdConnected;
extern String selectedFile;
extern String selectionSource;
#define MCP_PIN_ETHERNET 0  // Ethernet aan/uit
#define MCP_PIN_START 1      // Eerste pin voor binaire input
#endif
#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "sd_handler.h"
#include "mcp_handler.h"
#include "webserver_handler.h"
#include "ethernet_handler.h"
#include "lcd_handler.h"

// ✅ Externe variabelen voor bestandsbeheer
extern String selectedFile;
extern String selectionSource;

// ✅ MCP23017 Pin Definities
#define MCP_PIN_ETHERNET 0  // Ethernet aan/uit
#define MCP_PIN_START 1      // Eerste pin voor binaire input

// 📂 **Debug Status Variabel**
extern bool debugMode;
extern bool mcpConnected;
extern bool sdConnected;
extern String lastPlayedFile;

#endif
