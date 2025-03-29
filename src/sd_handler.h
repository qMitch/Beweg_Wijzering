#ifndef SD_HANDLER_H
#define SD_HANDLER_H

#include <Arduino.h>
#include "FS.h"
#include "SD_MMC.h"
#include <Ethernet.h>
#include "mcp_handler.h"
#include "main.h"

bool initSDCard(int clk, int cmd, int d0, int d1, int d2, int d3);
void serveFile(EthernetClient &client, String filename);
void handleFileUpload(EthernetClient &client);
String getSelectedFile();

#endif
