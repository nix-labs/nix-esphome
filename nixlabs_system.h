#include "time.h"

#define FW_VERSION                "1.00"  // NIX labs FW version for this code

// All timeouts in milliseconds, size U32 unless stated otherwise
#define DEVICE_PROCESSING_TIMEOUT	100     // Max time between sending the packet and the first byte of the response
#define RESPONSE_RX_TIMEOUT     	50      // Max time between the first byte of the response and the complete 64B being received
#define SERIAL_PORT_BUSY_TIMEOUT	100     // Max time to wait for the serial port to be free

#define RXD2 16 // UART that connects to clock CPU
#define TXD2 17

#define DEFAULT_TIMEZONE			"AEST-10"	// Default is Brisbane time, refer to https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define TIMEZONE_MAXLEN				32

bool serialPortBusy = false;          // Mutex for the serial port


// Modified from http://automationwiki.com/index.php?title=CRC-16-CCITT
// Modified from https://gist.github.com/chitchcock/5112270
uint16_t crc16(std::vector<unsigned char> data_p, uint16_t length) {
    uint16_t crc = 0xFFFF;
    uint16_t j, i, c;
    const std::vector<uint16_t> crcTable{0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0};

    for (i = 0; i < length; i++) {
        c = data_p[i];
        j = (c ^ (crc >> 8)) & 0xFF;
        crc = crcTable[j] ^ (crc << 8);
    }

    return crc; 
}


// Send a 64-byte data packet but do not wait for a response, returns 0 for success and 1 for timeout waiting for port
// Since no response is collected there is no option to auto-retry on failure, and the response data if any will be handled by checkForCommands()
uint8_t send_packet(std::vector<unsigned char> data){
  uint8_t i;
  uint32_t busy_timeout = 0;
  uint16_t crc;

  crc = crc16(data, 62);          // Compute 16b CRC over the preceeding 62 bytes and store in last two bytes for a total of 64B
  data[62] = (crc & 0xFF00) >> 8;
  data[63] = (crc & 0x00FF);

  while(serialPortBusy && busy_timeout < SERIAL_PORT_BUSY_TIMEOUT){   // Wait for the port to be free if it is in use by another process we control
    delay(1);
    busy_timeout++;
  }

  if(busy_timeout >= SERIAL_PORT_BUSY_TIMEOUT){
    ESP_LOGW("NIXlabs System", "Timed out waiting for serial port to be free");
    return 1; // failed due to timeout
  } else {
    ESP_LOGD("NIXlabs System", "Waited %ums for serial port to be free", busy_timeout);
  }

  serialPortBusy = true;      // Lock the resource whilst we transmit
  Serial2.write('*');         // This character instructs that what follows is the 64-byte command protocol
  for(i=0; i< 64; i++){
    Serial2.write( data[i] );	// Send data to the nixie clock CPU
  }
  serialPortBusy = false;     // Release the resource

  return 0; // success
}


// Send a 64-byte data packet and wait to get the response. Response is validated and in the case of wrong response or packet error response, auto-retry is possible:
// maxAttempts=0 or 1 means attempt once but do not retry, autoRetries=2 (default) means try once and then try again if needed. etc.
std::vector<unsigned char> send_packet_get_response(std::vector<unsigned char> data, uint8_t maxAttempts=2){
  uint8_t errors;
  uint8_t attempts = 0;
  std::vector<unsigned char> responseData(65);

  do {
    errors = 0;
    attempts++;

    errors += send_packet(data);

    if( errors == 0 ){  // Only wait for the response if we sent out the request without errors
      serialPortBusy = true;      // Lock the resource whilst we wait for the reply

      uint32_t timeout = 0;
      while (Serial2.available() < 63 && timeout < DEVICE_PROCESSING_TIMEOUT) {	// collect entire packet (always 64B)
        delay(1);
        timeout++;
      }

      if(timeout >= DEVICE_PROCESSING_TIMEOUT){
        ESP_LOGW("NIXlabs System", "Timed out waiting for device to respond");
        errors++;

      } else {
        ESP_LOGD("NIXlabs System", "Waited %ums for device response", timeout);

        // char responseData[64];
        Serial2.read();	// get rid of the asterisk
        Serial2.readBytes(&responseData[0], 64);

        serialPortBusy = false;      // Release the resource

        // First do some easy checks to ensure the response is what we wanted
        if( responseData[0] == 0xFF ){              // Device returned the packet error response
          ESP_LOGW("NIXlabs System", "Device returned the packet error response!");
          errors++;

        } else if ( responseData[0] != data[0] ){   // Device returned a response but not the one associated with our request
          ESP_LOGW("NIXlabs System", "Device returned an incorrect response");
          errors++;
        }

        // Next verify the CRC to confirm the data integrity
        uint16_t crc, rx_crc;
        crc = crc16(responseData, 62);  
        rx_crc = responseData[62] << 8 | responseData[63];

        if( crc != rx_crc ){  // Compare calc CRC vs the CRC reported to us in the data packet
          ESP_LOGW("NIXlabs System", "Device returned a corrupt response. Calc CRC=%u, RX CRC=%u", crc, rx_crc);
          errors++;
        }
        // At this point we have a verified response that can be used by the calling function
        
        // This code only used for printing the reponse as a debug message. Ref: https://stackoverflow.com/a/41173236
        char Str_responseData[(64 * 4) + 1];  // Enough space for 3 digits per array entry plus comma plus end null
        char *ptr = &Str_responseData[0];     // Pointer to the first item (0 index) of the output array
        int i;

        for (i = 0; i < 64; i++) {            // Convert each byte to fixed 3-place decimal
            ptr += sprintf(ptr, "%u,", responseData[i]);   // Null is already added from the last decimal string.
        }
        *(ptr-1) = '\0';   // replace the final ',' with a null to supress it        

        ESP_LOGD("NIXlabs System", "Device data received: %s", Str_responseData);
      }
    }

  } while (errors > 0 && attempts < maxAttempts);   // if there was a problem, retry up to the maximum attemps requested

  if( errors > 0 ){ // If the last attempt still had a problem, then mark the packet as failed by setting byte 0 to 0x0 which is not a valid value.
    responseData[0] = 0;
  }

  return responseData;

}


void checkForCommands(){
  String cmd = "";

  // Process serial port command
  // Special settings transfer command is handled in the websocket code
  if( Serial2.available() ) { 

    if(Serial2.peek() == '*'){    // Special command response packet shall be handled as data rather than string.
      
      uint32_t timeout = 0;
      while (Serial2.available() < 63 && timeout < RESPONSE_RX_TIMEOUT) {	// collect entire packet (always 64B)
        delay(1);
        timeout++;
      }

      if(timeout >= RESPONSE_RX_TIMEOUT){

        ESP_LOGW("NIXlabs System", "Timed out waiting for device response to finish");

      } else {

        // std::vector<unsigned char> data(65);
        char data[64];

        Serial2.read();	// get rid of the asterisk
        Serial2.readBytes(data, 64);

        if(data[0] == 0xFF){  // Device returned the packet error response
          ESP_LOGW("NIXlabs System", "Device returned the packet error response!");
        }

        // This code only used for printing the reponse as a debug message. Ref: https://stackoverflow.com/a/41173236
        char Str_responseData[(64 * 4) + 1];  // Enough space for 3 digits per array entry plus comma plus end null
        char *ptr = &Str_responseData[0];     // Pointer to the first item (0 index) of the output array
        int i;

        for (i = 0; i < 64; i++) {            // Convert each byte to fixed 3-place decimal
            ptr += sprintf(ptr, "%u,", data[i]);   // Null is already added from the last decimal string.
        }
        *(ptr-1) = '\0';   // replace the final ',' with a null to supress it        

        ESP_LOGD("NIXlabs System", "Device data received: %s", Str_responseData);
      }

    } else {    // Now we handle the normal string-based commands

      cmd = Serial2.readString();
      
      ESP_LOGD("NIXlabs System", "Received %s:", cmd);  // Echo the received command

      if( cmd == "STATUS?" ){
        ESP_LOGD("NIXlabs System", "Status response: ACTIVE");
        Serial2.println("ACTIVE");

      } else if ( cmd == "WIFINETIP?" ){
        char strbuf[20];
        sprintf(strbuf, "IP: %s", WiFi.localIP().toString());
        ESP_LOGD("NIXlabs System", "Status response: %s", strbuf);
				Serial2.print(strbuf);
				
			} else if( cmd == "_FWVERSION?" ){
        ESP_LOGD("NIXlabs System", "Status response: %s", FW_VERSION);
				Serial2.println(FW_VERSION);

			} else if( cmd == "TIME?" ){

        struct tm NTP_DateTime;

        if( !getLocalTime(&NTP_DateTime) ) {
          ESP_LOGD("NIXlabs System", "Status response: ERR failed to get NTP time");
          Serial2.println("ERR");

        } else {
          char strbuf[21];
          sprintf(strbuf, "TIME %u-%02u-%02uT%02u:%02u:%02uZ%u", NTP_DateTime.tm_year+1900, NTP_DateTime.tm_mon+1, NTP_DateTime.tm_mday, NTP_DateTime.tm_hour, NTP_DateTime.tm_min, NTP_DateTime.tm_sec, NTP_DateTime.tm_wday);

          // Validation check of the received values
          if( NTP_DateTime.tm_year+1900 >= 2023 && NTP_DateTime.tm_mon+1 <= 12 && NTP_DateTime.tm_mday <= 31 && NTP_DateTime.tm_hour <= 23 && NTP_DateTime.tm_min <= 59 && NTP_DateTime.tm_sec <= 59 && NTP_DateTime.tm_wday <= 6 ){
            ESP_LOGD("NIXlabs System", "Status response: %s", strbuf);
            Serial2.println(strbuf);

          } else {
            ESP_LOGD("NIXlabs System", "Status response: ERR invalid NTP time %s", strbuf);
            Serial2.println("ERR");
          }
        }

      } else {
        ESP_LOGW("NIXlabs System", "Status response: ERR unknown command");
        Serial2.println("ERR");
      }
    }
  }
  return;
}


class NIXsystem : public Component {
 public:

 float get_setup_priority() const override { return esphome::setup_priority::AFTER_CONNECTION; }

  void setup() override {

    // Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    // Configure NTP
    struct tm NTP_DateTime;
    configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset

    setenv("TZ", DEFAULT_TIMEZONE, 1);  // Set the user's timezone via posix string, see https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
    tzset();
    // End NTP config
  }


  void loop() override {

    checkForCommands();

  }

};