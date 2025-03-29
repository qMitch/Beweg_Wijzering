#ifndef WEBSERVER_HANDLER_H
#define WEBSERVER_HANDLER_H

#include <Arduino.h>
#include <Ethernet.h>
#include "main.h"
#include "sd_handler.h"
#include "mcp_handler.h"
#include "sd_handler.h"
#include "ethernet_handler.h"

void generateWebPage(EthernetClient &client);
void handleClient(EthernetClient &client, String request);
void generateDebugPage(EthernetClient &client);
void initWebserver();

#endif
