#include <lorae5.h>
#include <stdio.h>


/// 
///  CONSTRUCTOR
///

LoraE5::LoraE5(
    const uint8_t sf, 
    const bool adr,
    bool modeOtaa, // true if OTAA, false if ABP
    char * devEUI, 
    char * devAddr, 
    char * nwkSKey, 
    char * appSKey, 
    char * appKey, 
    char * appEUI
){
  initConfig(sf, adr, modeOtaa, devEUI, devAddr, nwkSKey, appSKey, appKey, appEUI);
  debugFunction("Lorae5 Initialization Done");
}

/// 
///  TRANSMISSION COMMANDS
///

// use sendATCommand as public -- will be removed soon
void LoraE5::sendMsg(const char* ATcmd){
  sendATCommand(ATcmd); 
}

// Tx from Arduino to computer
void LoraE5::debugFunction(const char* debugMsg){
  Serial.println(debugMsg);
}

// Tx from Arduino to LoraE5
void LoraE5::sendATCommand(const char* ATcmd){
  debugFunction("Sending ... ");
  Serial1.println(ATcmd);
  Serial.println(ATcmd);
  this->previousTime = delay(1000);
}

bool LoraE5::sendATTest(
  const char* ATcmd,
  const char* okResp,
  const char* errResp,
  const char * ending, 
  uint32_t timeoutMs
  ){
  if ( this->runningCommand ) {
    debugFunction("LoRaE5 - AT commande already processing\r\n");
    return false;
  }
  this->runningCommand = true;
  this->startTime = millis();
  this->maxDuration = timeoutMs;
  strcpy(this->bufOkResp,okResp); 
  strcpy(this->bufErrResp,errResp);
  // Check if the ending condition is specified
  if ( ending != NULL && strlen(ending) > 0 ) {
    strcpy(this->bufEnding,ending);
    this->withEndingCondition = true;
  } else {
    this->withEndingCondition = false;
  }


  this->indexRespBuffer = 0;
  // line processing deleted 
  Serial1.println(ATcmd);

  Serial.println("LoRaE5 - send ");
  Serial.println(ATcmd);
  // synchrone/asynchrone deleted
  return true;

}


void readTest(){
  if (Serial1.available() > 0) {
   char c = Serial1.read();

   Serial.print("Caractere recu : ");
   Serial.println(c);
}
}

void LoraE5::readAT(){
  //if ( !this->runningCommand ) return true; 
  uint16_t cpt=0;
  while(!Serial1.available()){
    if(cpt%10==1){
      debugFunction("busy");
    }
  }
  while(Serial1.available()){
    char c;
    c= Serial1.read();
    if ( (c == '\0' || c == '\r' || c == '\n' ) ) {
        if ( this->indexRespBuffer > 0 ) {
          // process line response
          this->bufResponse[this->indexRespBuffer] = '\0';
          
        }
    }
    else{
      if ( this->indexRespBuffer < __LORAE5_ATRESP_BUFF_SZ ) {
            this->bufResponse[this->indexRespBuffer] = c;
            this->indexRespBuffer++;    
      }
      else{
        Serial.println("Size overflow");
      }
    }
  }
  char response[127];
  strcpy(response, "BUFFER RESPONSE");
  strcat(response, this->bufResponse);
  sendATCommand(response);
}

/// 
///  CONFIGURATION
///

void LoraE5::initConfig(
      const uint8_t sf, 
      const bool adr,
      bool modeOtaa, 
      char * devEUI, 
      char * devAddr, 
      char * nwkSKey, 
      char * appSKey, 
      char * appKey, 
      char * appEUI
){
  setSF(sf);
  setADR(adr);
  this->modeOtaa=modeOtaa;
  this->devEUI=devEUI;
  this->devAddr=devAddr;
  this->nwkSKey=nwkSKey;
  this->appSKey=appSKey;
  this->appKey=appKey;
  this->appEUI=appEUI;
}


/// 
///  SETUP PARAMETERS VIA AT COMMANDS
///

void LoraE5::sendATConfig(){
  sendDR(this->sf);
  sendADR(this->adr);
  if(this->modeOtaa){
    sendConfigOTAA();
  }
  else{
    sendConfigABP();
  }
  sendATCommand("AT+MSG"); // Initialize Connection 
}

bool LoraE5::sendDR(uint8_t sf){
  bool ret = true;
  switch(sf){
    case 7: 
      sendATCommand("AT+DR=DR5");
      break;
    case 8: 
      sendATCommand("AT+DR=DR4");
      break;
    case 9: 
      sendATCommand("AT+DR=DR3");
      break;    
    case 10: 
      sendATCommand("AT+DR=DR2");
      break;
    case 11: 
      sendATCommand("AT+DR=DR1");
      break;
    case 12: 
      sendATCommand("AT+DR=DR0");
      break;
    default:
      ret = false;
      break;
  }
  //readAT();
  //readTest();
  return ret;
}

void LoraE5::sendADR(bool adr){
  if(adr){
    sendATCommand("AT+ADR=ON");
  }
  else{
    sendATCommand("AT+ADR=OFF");
  }
}

void LoraE5::sendConfigOTAA(){
  sendATCommand("AT+MODE=LWOTAA");
  char CMD_appKey[60];
  char CMD_appEUI[60];
  // appKey config
  strcpy(CMD_appKey, "AT+KEY=APPKEY,");
  strcat(CMD_appKey, this->appKey);
  sendATCommand(CMD_appKey); 
  // appEUI config
  strcpy(CMD_appEUI, "AT+ID=APPEUI,");
  strcat(CMD_appEUI, this->appEUI);
  sendATCommand(CMD_appEUI);
  sendATCommand("AT+JOIN");
}

void LoraE5::sendConfigABP(){
  sendATCommand("AT+MODE=LWABP");
  char CMD_devAddr[60];
  char CMD_nwkSKey[60];
  char CMD_appSKey[60];
  // devAddr config
  strcpy(CMD_devAddr, "AT+ID=DEVADDR,");
  strcat(CMD_devAddr, this->devAddr);
  sendATCommand(CMD_devAddr);
  // NwSKey config
  strcpy(CMD_nwkSKey, "AT+KEY=NWKSKEY,");
  strcat(CMD_nwkSKey, this->nwkSKey);
  sendATCommand(CMD_nwkSKey);
  // appSKey config
  strcpy(CMD_appSKey, "AT+KEY=APPSKEY,");
  strcat(CMD_appSKey, this->appSKey);
  sendATCommand(CMD_appSKey);
}

uint64_t LoraE5::delay(uint64_t t){
  uint64_t currentTime = millis();
  while((currentTime-t)<(this->previousTime)){
      currentTime = millis();
  }
  return currentTime; // becomes the new previous time
}

// Getters & Setters

void LoraE5::setSF(uint8_t sf){
  this->sf = sf;
}

void LoraE5::setADR(bool adr){
  this->adr = adr;
}

uint8_t LoraE5::getSF(){
  return this->sf;
}

bool LoraE5::getADR(){
  return this->adr;
}


// Info message

void LoraE5::printInfo(){
  debugFunction("#######  LoRaWAN Training Session ######");
  debugFunction("##### Savoie Mont Blanc University #####");
  debugFunction("#######                           ######");
  debugFunction("#######           Hello  !        ######");
  debugFunction("#######         LoRa E5 HT        ######");
}
