BewegWijzering

Is de basis voor toekomstige Led-e-Lux Esp32 projecten in dit project staan verschillende bibliotheken met basisfuncties die hergebruikt kunnen worden en is zo geschreven dat het makkelijk herbruikbaar is.

Dit project is opgedeeld in verschillende mappen op basis van de structuur uit Visual Studio Code, deze IDE is namelijk gebruikt hiervoor werd ook nog een extensie gebruikt PlatformIO. 
Met deze extensie kan je in de Arduino omgeving werken en is dit ook goed te combineren met de ESP chips.
In de backupfiles staan korte stukjes code die een module of bibliotheek uit testen met de devkit. 
In de src staat alle code die het hart vormt van dit project, de eigen geschreven bibliotheken gebaseerd op de modules of functionaliteiten die zijn ingebouwd, daarnaast staat hier ook de main file die gedraaid wordt op de ESP. 
Als laatste is er nog een map met een test main file die gebruik maakt van alle test files die erin staan om op die manier verschillende functie in een unit te testen.



- Backupfiles
  - BackupEthernet
  - BackupMain
  - BackupMCP23017
  - BackupSDIO
- src
  - README  
  - main.h
  - main.cpp
  - ethernet_handler.h
  - ethernet_handler.cpp
  - mcp_handler.h
  - mcp_handler.cpp
  - sd_handler.h
  - sd_handler.cpp
  - webserver_handler.h
  - webserver_handler.cpp
  - lcd_handler.h
  - lcd_handler.cpp
- test
  - README
  - test_main.h
  - test_main.cpp
  - test_ethernet_handler.h
  - test_ethernet_handler.cpp
  - test_mcp_handler.h
  - test_mcp_handler.cpp
  - test_sd_handler.h
  - test_sd_handler.cpp
  - test_webserver_handler.h
  - test_webserver_handler.cpp
  - test_lcd_handler.h
  - test_lcd_handler.cpp

De gebruikte externe bibliotheken zijn:
-  arduino-libraries/Ethernet
-  adafruit/Adafruit MCP23017 Arduino Library
-  unity
-  ovyan03/LovyanGFX@^1.1.6
-  odmer/JPEGDecoder@^2.0.0

Deze files worden weergegen in de platform.ini file, hier worden ook configuratie instellingen weergegeven zoals monitor speed, platform en framework.

Via de main.cpp kan je de pin configuratie vinden van alle modules die zijn aangesloten.
ook wordt hier de Ethernet configuratie weergegeven voor de webpagina.

De indexering van de SD kaart wordt gedaan zoals het voorbeeld hier onder:

/SD kaart/

├── 001_image.jpg  

├── 002_image.bmp

├── ...
├── /006_VideoKone/

│	├──frame0001.bmp

│	├──frame0002.bmp

│	├──…

├── /007_VideoJumbo/

├── ...

Deze volgorde moet bestaan uit de volgende prompt 001_randomNaam tot maximaal 254_randomNaam. De bestanden die in de hoofdmap staan worden gebruikt als foto, de foto moet de maximale resolutie hebben van 380x480. Deze foto kan dan worden toegevoegd als .bmp of .jpg, voorkeur gaat uit naar jpg omdat deze ook via de webserver getoond kan worden. Video wordt opgedeeld in frames in bestandsformaat .bpm en bevat de prompt frame0001 t/m bijvoorbeeld frame0240.
De bestanden formatten kan gedaan worden via verschillende websites of ai.

Dit Project wordt beoordeeld voor mijn semester 4 eindproject AD.
