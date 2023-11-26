#include "esphome.h"
#include "nixlabs_system.h"

// Turn off the display and LEDs
void dispOff() {

  ESP_LOGD("NIXlabs Commands", "Display Off");

  std::vector<unsigned char> data(65);
  data[0] = 0x20;	    // special function command to turn display on/off, byte 0 in the manual
  data[0x10] = 2;     // off

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Turn on the display and LEDs
void dispOn() {

  ESP_LOGD("NIXlabs Commands", "Display On");

  std::vector<unsigned char> data(65);
  data[0] = 0x20;	    // special function command to turn display on/off, byte 0 in the manual
  data[0x10] = 3;     // off

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Show the temperature screen for about 10 seconds
void showTemperature() {

  ESP_LOGD("NIXlabs Commands", "Show Temperature");

  std::vector<unsigned char> data(65);
  data[0] = 0x20;	    // special function command to show alternate screen for 3s
  data[0x11] = 1;     // Temperature screen

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Show the date screen for about 10 seconds
void showDate() {

  ESP_LOGD("NIXlabs Commands", "Show Date");

  std::vector<unsigned char> data(65);   
  data[0] = 0x20;	    // special function command to show alternate screen for 3s
  data[0x11] = 2;     // Date screen

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Show one round of display/LED demo
void showDemo() {

  ESP_LOGD("NIXlabs Commands", "Show Demo");

  std::vector<unsigned char> data(65);
  data[0] = 0x20;	    // special function command to show alternate screen
  data[0x11] = 3;     // One round of antipoisoning (like a display demo)

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Start a counter
void startCounter() {

  ESP_LOGD("NIXlabs Commands", "Start Counter");

  std::vector<unsigned char> data(65);
  data[0] = 0x20;	    // special function command to start timer/counter
  // data[1-4] will accept a starting value for the counter, by default it starts at zero
  data[0x05] = 3;     // Begin up counter
  // Bit 0 : Up counter? Set Bit 1 to 1 to use the value.
  //    0 = Count down (timer) with beep on finish
  //    1 = Count up (stopwatch), no beep (same 99:59:59 time limit)
  // Bit 2 : Counter pause? Set Bit 3 to 1 to use the value.
  //    0 = Normal operation
  //    1 = Counter paused (to nearest second)
  // Bit 4 : Prefer to show time screen? Always read when a new timer is set (ie: valid Tmr0-3 value).
  //    0 = Counter is shown except for 3s every minute (time shows). Button press shows time for 3s 

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Pause Counter/Timer
void pauseCounterTimer() {

  ESP_LOGD("NIXlabs Commands", "Pause Counter/Timer");

  std::vector<unsigned char> data(65);
  data[0] = 0x20;	    // special function command to start timer/counter
  data[1] = 0xFF;     // Time value is ignored if >359999 (which setting MSByte 0xFF does do) so that we can pause without changing the time
  data[0x05] = 0x0C;  // Pause counter/timer
  // Bit 0 : Up counter? Set Bit 1 to 1 to use the value.
  //    0 = Count down (timer) with beep on finish
  //    1 = Count up (stopwatch), no beep (same 99:59:59 time limit)
  // Bit 2 : Counter pause? Set Bit 3 to 1 to use the value.
  //    0 = Normal operation
  //    1 = Counter paused (to nearest second)
  // Bit 4 : Prefer to show time screen? Always read when a new timer is set (ie: valid Tmr0-3 value).
  //    0 = Counter is shown except for 3s every minute (time shows). Button press shows time for 3s 

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Resume Counter/Timer (from pause)
void resumeCounterTimer() {

  ESP_LOGD("NIXlabs Commands", "Resume Counter/Timer");

  std::vector<unsigned char> data(65);    
  data[0] = 0x20;	    // special function command to start timer/counter
  data[1] = 0xFF;     // Time value is ignored if >359999 (which setting MSByte 0xFF does do) so that we can pause without changing the time
  data[0x05] = 0x08;  // Resume counter/timer
  // Bit 0 : Up counter? Set Bit 1 to 1 to use the value.
  //    0 = Count down (timer) with beep on finish
  //    1 = Count up (stopwatch), no beep (same 99:59:59 time limit)
  // Bit 2 : Counter pause? Set Bit 3 to 1 to use the value.
  //    0 = Normal operation
  //    1 = Counter paused (to nearest second)
  // Bit 4 : Prefer to show time screen? Always read when a new timer is set (ie: valid Tmr0-3 value).
  //    0 = Counter is shown except for 3s every minute (time shows). Button press shows time for 3s 

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Cancel Counter/Timer
void cancelCounterTimer() {

  ESP_LOGD("NIXlabs Commands", "Cancel Counter/Timer");

  std::vector<unsigned char> data(65);
  data[0] = 0x20;	    // special function command to start timer/counter
  data[0x05] = 0x02;  // Down counter with zero in data[1-4] cancells timer/counter if running
  // Bit 0 : Up counter? Set Bit 1 to 1 to use the value.
  //    0 = Count down (timer) with beep on finish
  //    1 = Count up (stopwatch), no beep (same 99:59:59 time limit)
  // Bit 2 : Counter pause? Set Bit 3 to 1 to use the value.
  //    0 = Normal operation
  //    1 = Counter paused (to nearest second)
  // Bit 4 : Prefer to show time screen? Always read when a new timer is set (ie: valid Tmr0-3 value).
  //    0 = Counter is shown except for 3s every minute (time shows). Button press shows time for 3s 

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Begin a timer from the desired starting time in seconds
void startTimer(uint32_t time_val) {

  if( time_val > 359999 ){
    ESP_LOGW("NIXlabs Commands", "Timer value is out of range!");
    return;
  }

  ESP_LOGD("NIXlabs Commands", "Start Timer for %u sec", time_val);

  std::vector<unsigned char> data(65);
  data[0] = 0x20;	    // special function command to start timer/counter
  data[1] = (time_val & 0xFF000000) >> 24;  // Start time in sec, MSByte
  data[2] = (time_val & 0x00FF0000) >> 16;  // 
  data[3] = (time_val & 0x0000FF00) >> 8;   // 
  data[4] = (time_val & 0x000000FF);        // Start time in sec, LSByte
  data[0x05] = 0x02;  // Down counter with zero in data[1-4] cancells timer/counter if running
  // Bit 0 : Up counter? Set Bit 1 to 1 to use the value.
  //    0 = Count down (timer) with beep on finish
  //    1 = Count up (stopwatch), no beep (same 99:59:59 time limit)
  // Bit 2 : Counter pause? Set Bit 3 to 1 to use the value.
  //    0 = Normal operation
  //    1 = Counter paused (to nearest second)
  // Bit 4 : Prefer to show time screen? Always read when a new timer is set (ie: valid Tmr0-3 value).
  //    0 = Counter is shown except for 3s every minute (time shows). Button press shows time for 3s 

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


// Play a tone on the buzzer
// Invalid or unspecified values will give the default tone of 2048Hz at max volume for 500ms
void beep(uint16_t freq=2000, uint8_t volume=7, uint16_t duration=500) {

  if(freq > 8191 || freq < 50){
    ESP_LOGW("NIXlabs Commands", "Frequency is out of range, using default");
    freq = 2000;
  }
  if(volume > 7 ){
    ESP_LOGW("NIXlabs Commands", "Volume is out of range, using default");
    volume = 7;
  }
  if(duration > 2550 || freq < 10){
    ESP_LOGW("NIXlabs Commands", "Duration is out of range, using default");
    duration = 500;
  }

  ESP_LOGD("NIXlabs Commands", "Beep: %uHz, volume %u/7, duration %ums", freq, volume, duration);

  std::vector<unsigned char> data(65);

  uint16_t freq_vol = freq | (volume << 13);  // Two bytes are sent with top 3 bits being volume and the rest is frequency
    
  data[0] = 0x12;	                        // Command to set system paramters
  data[0x0A] = 1;                         // Play tone bit
  data[0x07] = (freq_vol & 0xFF00) >> 8;  // Frequency upper byte, top 3 bits are volume
  data[0x08] = (freq_vol & 0x00FF);       // Frequency lower byte
  data[0x09] = duration/10;               // Accepts duration in tens of milliseconds (ie: 50 = 500ms)

  send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

  return;
}


class nix_backlight : public Component, public LightOutput {
 public:
  void setup() override {
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  }

  LightTraits get_traits() override {
    // return the traits this light supports
    auto traits = LightTraits();
    traits.set_supported_color_modes({ColorMode::RGB, ColorMode::BRIGHTNESS});
    return traits;
  }

  void write_state(LightState *state) override {
    // This will be called by the light to get a new state to be written.
    float red, green, blue;
    // use any of the provided current_values methods
    state->current_values_as_rgb(&red, &green, &blue);
    // Write red, green and blue to HW
    ESP_LOGD("NIXlabs Commands", "RGB = %f, %f, %f", red, green, blue);

    std::vector<unsigned char> data(65);
    data[0] = 0x08;	      // Command for LED colour and display brightness
    data[5] = 0x01;		    // Show the requested colour but dont save to memory
    
    data[2] = green*30 +1;   // Accepts values 0-31, 0 is off so add 1 so that dimmest is still on
    data[3] = red*30 +1;
    data[4] = blue*30 +1;

    send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

    return;
  }
};


class nix_display : public Component, public LightOutput {
 public:
  void setup() override {
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  }

  LightTraits get_traits() override {
    // return the traits this light supports
    auto traits = LightTraits();
    traits.set_supported_color_modes({ColorMode::BRIGHTNESS});
    return traits;
  }

  void write_state(LightState *state) override {
    // This will be called by the light to get a new state to be written.
    float brightness;
    // use any of the provided current_values methods
    state->current_values_as_brightness (&brightness);
    // Write to HW
    ESP_LOGD("NIXlabs Commands", "Display = %f", brightness);

    std::vector<unsigned char> data(65);     
    data[0] = 0x08;	            // Command for LED colour and display brightness
    data[0x13] = brightness*7;  // Accepts display brightness 0-7
    data[0x14] = 3;	            // Apply to: 1=night level, 2=day level, 3=day and night, 4=day level and calculate night based on past ratio

    send_packet_get_response(data);  // Returns 0 if success, 1 if fail. It will print the warning for us so not anything else we need to do if failed

    return;
  }
};


class NIXsensors : public PollingComponent { 
 public:
  Sensor *temperature = new Sensor();
  Sensor *lightLevel = new Sensor();

  NIXsensors() : PollingComponent(60000) {}  // Every 60sec

  void setup() override {
  }

  void update() override {    // Every 60sec

    ESP_LOGD("NIXlabs Commands", "Requesting sensor data..");

    std::vector<unsigned char> data(65); 
    std::vector<unsigned char> response(65);
    data[0] = 0x11;	    // Request to read the measurement data

    response = send_packet_get_response_get_response(data);  // Will also auto-retry, if it fails, byte0=0

    if(response[0] != 0){ // Byte0=0 is set by the function and can only mean a failed transaction

      float rx_temperature;
      uint16_t rx_light;

      rx_light = ( (response[0x08] << 8) | response[0x09] ) / 25;           // RX Value is in mV max being 2500mV *100 = 25
      rx_temperature = ( (response[0x14] << 8) | response[0x15] ) / 1000.0; // RX Value is in millidegrees

      temperature->publish_state(rx_temperature);
      lightLevel->publish_state(rx_light);

    }

    return;
  }
};