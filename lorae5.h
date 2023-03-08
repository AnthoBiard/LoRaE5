#include <Arduino.h>
#include <stdint.h>

#ifndef __LORAE5_H__
#define __LORAE5_H__

#define __LORAE5_ATCMD_BUFF_SZ          64          // max size to store ok/err response pattern
#define __LORAE5_ATRESP_BUFF_SZ         128         // max size to store a response from an AT command
// ==========================================================
// LoRaE5

class LoraE5{
private: 
    uint8_t                 sf;             // Spreading Factor
    bool                    adr = false;    // Adaptive Data Rate
    bool                    modeOtaa;       // OTAA = 1, ABP = 0
    char *                  devEUI;         // for both modes
    char *                  devAddr;        // only in ABP mode
    char *                  nwkSKey;        // only in ABP mode
    char *                  appSKey;        // only in ABP mode
    char *                  appKey;         // only in OTAA mode
    char *                  appEUI;         // only in OTAA mode

    
    uint64_t                previousTime;
    char                    bufOkResp[__LORAE5_ATCMD_BUFF_SZ];
    char                    bufErrResp[__LORAE5_ATCMD_BUFF_SZ];
    char                    bufEnding[__LORAE5_ATCMD_BUFF_SZ];
    char                    bufResponse[__LORAE5_ATRESP_BUFF_SZ];
    uint16_t                indexRespBuffer;
    bool                    runningCommand;
    uint32_t                startTime;
    uint32_t                maxDuration;
    bool                    withEndingCondition;
 
    // SendAT & ReadAT
    void sendATCommand(const char* ATcmd);
    bool sendATTest(
        const char* ATcmd,
        const char* okResp,
        const char* errResp,
        const char* ending, 
        uint32_t timeoutMs
    );
    void readAT();
    void debugFunction(const char* debugMsg);
    void readTest();
    
    // SEND AT CONFIG
    bool sendDR(uint8_t dr); 
    void sendADR(bool adr); 
    void sendConfigOTAA(); 
    void sendConfigABP();
    uint64_t delay(uint64_t previousTime);
    
   // SETTERS & GETTERS 
    void setSF(uint8_t sf);
    void setADR(bool adr);

    
    uint8_t getSF();
    bool getADR();

public:
    // CONSTRUCTOR
    LoraE5(
        const uint8_t sf, 
        const bool adr, 
        bool modeOtaa, 
        char * devEUI, 
        char * devAddr, 
        char * nwkSKey, 
        char * appSKey, 
        char * appKey, 
        char * appEUI
    );

    void initConfig(
        const uint8_t sf, 
        const bool adr, 
        bool modeOtaa, 
        char * devEUI, 
        char * devAddr, 
        char * nwkSKey, 
        char * appSKey, 
        char * appKey, 
        char * appEUI
    );
    // SETUP 
    void sendATConfig();

    // MORE
    void sendMsg(const char* ATcmd);
    void printInfo();
};

#endif // __LORAE5_H__ 
