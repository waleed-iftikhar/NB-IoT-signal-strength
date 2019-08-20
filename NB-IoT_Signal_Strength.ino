

#define DEBUG_STREAM Serial 
#define MODEM_STREAM Serial1
#define MODEM_ON_OFF_PIN 7

const String ResponseOK = "OK";
const String Error = "ERROR";

 
#define DEBUG_STREAM_BAUD 9600
#define MODEM_STREAM_BAUD 9600

void init(int onoffPin)
{
    if (onoffPin >= 0) {
        // First write the output value, and only then set the output mode.
        digitalWrite(onoffPin, HIGH);
        pinMode(onoffPin, OUTPUT);
    }
}

bool isAlive()
{
  String response;
  MODEM_STREAM.write("AT\r\n");         // Send AT command to check if modem is communicating
  response = readResponse();            // Read response from the NB-IoT module
  if (response == ResponseOK)
  return true;
  else if (response == Error)
  return false;
}


bool selectBand(){
  String response;
  MODEM_STREAM.write("AT+NBAND=8,20\r\n");  // Send AT+NBAND command to connect to 8,20 bands of NB-ioT protocol
  return (readResponse());                  // Read response from the NB-IoT module
  response = readResponse();
  if (response == ResponseOK)
  return true;
  else if (response == Error)
  return false; 
}


bool configure(){
  String response;
  MODEM_STREAM.write("AT+NCONFIG=AUTOCONNECT,TRUE\r\n"); // Configure modem to autoconnect mode
  response = readResponse();                             // Read response from the NB-IoT module
  if (response == ResponseOK)
  return true;
  else if (response == Error)
  return false;
  
}


bool radioSwitchOn(){
  String response;
  MODEM_STREAM.write("AT+CFUN=1\r\n");    // Switch on the radio
  response = readResponse();              // Read response from the NB-IoT module
  //DEBUG_STREAM.println(response);    // Switch on the radio
  if (response == ResponseOK)
  return true;
  else if (response == Error)
  return false;
}

void ip(){
  String response;
  DEBUG_STREAM.println("My IP Address");
  MODEM_STREAM.write("AT+CGPADDR\r\n");   // Read your IP- Address
  response = readResponse();
  DEBUG_STREAM.println(response);
}


bool reboot(){
  String response;
  MODEM_STREAM.write("AT+NRB\r\n");
  readResponse();
  delay(1000);
  response = readResponse();
  if (response == ResponseOK)
  return true;
  else if (response == Error)
  return false;
}


bool isNetwork(){
  String response,readValue;
  int index=0,value=0;
  //DEBUG_STREAM.println("AT+CGATT?\r\n");
  delay(1000);
  
  MODEM_STREAM.write("AT+CGATT?\r\n");
  response = readResponse();
  //DEBUG_STREAM.println(response);
  if (response == ResponseOK)
  return true;
  else if (response == Error)
  return false;
  else {
  index = response.indexOf(':');
  readValue = response.substring(index+1);
  value = readValue.toInt();
  return value;
  }
}

void ping(){
  String response,readValue;
  int index=0,value=0;
  DEBUG_STREAM.println("PING 172.217.22.174\r\n");
  MODEM_STREAM.write("AT+NPING=172.217.22.174\r\n");
  //MODEM_STREAM.write("AT+NPING=8.8.8.8\r\n");
  response = readResponse();
  DEBUG_STREAM.println(response);
  if (response == ResponseOK) {
   response = readResponse();
   DEBUG_STREAM.println(response);  
  //return true;
  }
  else if(response == Error) {
   response = readResponse();
   DEBUG_STREAM.println(response);  
  }
}



void signalStrength(){
  String incomingByte = "";
  int startIndex = 0, endIndex = 0, rcvedValue=0;
  String readValue;
  //DEBUG_STREAM.println("AT+CSQ\r\n");
  MODEM_STREAM.write("AT+CSQ\r\n");
  while(!MODEM_STREAM.available());
  incomingByte = MODEM_STREAM.readString();
  //DEBUG_STREAM.print(incomingByte);
  incomingByte.trim();                     //  Remove white spaces from the received Data
  startIndex = incomingByte.indexOf(':');
  endIndex = incomingByte.indexOf(',');
 
  readValue = incomingByte.substring(startIndex+1,endIndex);
  //DEBUG_STREAM.println(readValue);

  rcvedValue = readValue.toInt();
  DEBUG_STREAM.println(rcvedValue);
  strengthTable(rcvedValue);
}

void strengthTable(int pRcvedValue) {
  if(pRcvedValue == 0)
  DEBUG_STREAM.println("-113 dBm <= RSSI of the network");
  
  else if(pRcvedValue == 1)
  DEBUG_STREAM.println("-111 dBm");
  
  else if(pRcvedValue >= 2 && pRcvedValue <= 30)
  DEBUG_STREAM.println("-109 dBm <= RSSI of the network <= -53 dBm");

  else if(pRcvedValue == 31)
  DEBUG_STREAM.println("-51 dBm <= RSSI of the network");

  else
  DEBUG_STREAM.println("Not Detectable");
}


String readResponse(){
  String incomingByte = "";
  while(!MODEM_STREAM.available());
  incomingByte = MODEM_STREAM.readString();
  //DEBUG_STREAM.print(incomingByte);
  incomingByte.trim();                     //  Remove white spaces from the received Data
  return incomingByte;
   }


void setup() {
  
    init(MODEM_ON_OFF_PIN);
    DEBUG_STREAM.begin(DEBUG_STREAM_BAUD);
    MODEM_STREAM.begin(MODEM_STREAM_BAUD);
    delay(1000);
 
    DEBUG_STREAM.println("Initializing ..... ");
    while(!(isAlive()));
    configure();
    reboot();
    delay(3000);
 
    while(!radioSwitchOn());
    selectBand();
    delay(1000);
    while(!(isAlive()));
    DEBUG_STREAM.println("Connecting to Network ...... ");
    while(!isNetwork());
    DEBUG_STREAM.println("Successfully Connected ..... ");
    ip();
    delay(100);
    ping();
  }

void loop() {
  
  if(isNetwork())
  {
  DEBUG_STREAM.println("Signal Strength of NB-IoT Band 8,20 is");
  signalStrength();
  DEBUG_STREAM.println("\r\n");
  DEBUG_STREAM.println("\r\n");
  delay(2000);
  }
  else
  {
    while(!(isAlive()));
    selectBand();
    delay(100);
    DEBUG_STREAM.println("Connecting to Network ...... ");
    while(!isNetwork());
    DEBUG_STREAM.println("Successfully Connected ..... ");
    delay(100);
    ip();
    delay(100);
    ping();
  }
  
  

}

