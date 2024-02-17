/*
    parser.h
*/

#define WORKINGMEM_SIZE 500
#define MAX_PARAMETERS 20

String getParameter(String parName);   // get parameter with given name
String getParameter(int pos);          // get paramter at given position
void parseCommand (char * cmdstr);
void parseByte (int newByte);
void updateTimeouts ();
