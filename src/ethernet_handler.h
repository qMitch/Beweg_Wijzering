#ifndef ETHERNET_HANDLER_H
#define ETHERNET_HANDLER_H

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "main.h"
#include "webserver_handler.h"  

extern EthernetServer server;
extern bool ethernetEnabled;

bool initEthernet(int sclk, int miso, int mosi, int cs, IPAddress ip, IPAddress gateway, IPAddress subnet);
void handleEthernetClient();

#endif
