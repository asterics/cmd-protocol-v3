/*
    parser.cpp
*/

#include <Arduino.h>
#include "parser.h"

char   workingmem[WORKINGMEM_SIZE];
String parList[MAX_PARAMETERS];      // Note: parameter list will be overwritten when next AT command was received! TBD: protect!

extern void processCommand(String cmd);  // callback function when an AT command was received and parsed 

int numParams() {
  int i;
  for (i = 0; parList[i] != ""; i++) ;
  return (i);
}

String getParameter(String parName) {
  int parNameLen;
  for (int i = 0; parList[i] != ""; i++) {
    String pn=parList[i];
    pn.toLowerCase();
    if (pn.startsWith(parName)) {
      parNameLen = parName.length();
      String parValue = parList[i].substring(parNameLen);
      parValue.trim();
      if (parValue.startsWith("=")) {
        parValue=parValue.substring(1);
        parValue.trim();
        return (parValue);      
      }
    }
  }
  return ("");
}

String getParameter(int pos) {
  if ((numParams() > pos) && (pos > -1))
    return (parList[pos]);
  return ("");
}

void parseCommand (char * cmdstr)
{
  int actpar = 0;
  String atCommand = cmdstr;
  atCommand.trim();
  // Serial.print("AT Command:"); Serial.println (atCommand);

  int s1 = atCommand.indexOf(' ');
  int s2 = atCommand.indexOf(',', s1 + 1);
  parList[actpar] = atCommand.substring(0, s1);
  parList[actpar++].trim();
  while ((s1 > -1) && (actpar < MAX_PARAMETERS - 1)) {
    parList[actpar] = atCommand.substring(s1 + 1, s2);
    parList[actpar++].trim();
    atCommand = atCommand.substring(s1 + 1);
    s1 = atCommand.indexOf(',');
    s2 = atCommand.indexOf(',', s1 + 1);
  }
  parList[actpar] = "";

  //for (int i=0;parList[i]!="";i++) { Serial.print("  parameter: ");Serial.println(parList[i]); }
  //Serial.println();

  processCommand(getParameter(0));
}


void parseByte (int newByte)  // parse an incoming commandbyte from serial interface, perform command if valid
{
  static uint8_t cmdlen = 0;
  static uint8_t readstate = 0;

  switch (readstate) {
    case 0:
      if ((newByte == 'A') || (newByte == 'a')) readstate++;
      break;
    case 1:
      if ((newByte == 'T') || (newByte == 't')) readstate++; else readstate = 0;
      break;
    case 2:
      if ((newByte == '\r') || (newByte == '\n')) // AT reply: "OK"
      {
        Serial.println("OK");
        readstate = 0;
      }
      else if (newByte == ' ') {
        cmdlen = 0;
        readstate++;
      }
      else goto err;
      break;
    case 3:
      if ((newByte == '\r') || (newByte == '\n') || (cmdlen >= WORKINGMEM_SIZE - 5))
      { workingmem[cmdlen] = 0;  parseCommand((char *)workingmem);
        readstate = 0;
      }
      else workingmem[cmdlen++] = newByte;
      break;
    default:
    err: 
      Serial.println("?"); readstate = 0;
  }
}
