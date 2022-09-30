// https://github.com/platformio/platform-espressif32/tree/master/examples


// az-delivery-devkit-v4
// #include <Arduino.h>

// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLMA0Id31S"
#define BLYNK_DEVICE_NAME "Quickstart Template"

#define BLYNK_FIRMWARE_VERSION "0.7.01"

#define BLYNK_PRINT Serial
#define BLYNK_DEBUG     // prints comm exchange
#define APP_DEBUG       // allow custom print or not


// Uncomment your board, or configure a custom board in Settings.h
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7
//#define USE_TTGO_T_OI
//#define USE_ESP32C3_DEV_MODULE
//#define USE_ESP32S2_DEV_KIT

#include "BlynkEdgent.h"


String incoming_mess = "";


BlynkTimer timer_1s;
BlynkTimer timer_5s;
BlynkTimer timer_30s;
BlynkTimer timer_BLE;

unsigned int cnt;
// int hall = 88;
static boolean ble_en = false;



uint16_t p0 = 0;
uint16_t p1 = 0;
uint16_t p2 = 0;
uint16_t p3 = 0;
uint16_t p4 = 0;
uint16_t p5 = 0;

uint16_t p0_last = 0;
uint16_t p1_last = 0;
uint16_t p2_last = 0;
uint16_t p3_last = 0;
uint16_t p4_last = 0;
uint16_t p5_last = 0;

unsigned long fheap_last = 0;
unsigned long min_fheap_last = 0;


#define PIN_UPTIME          V0
#define PIN_COUNTER         V1
#define PIN_PRODRATIO       V2    // double
#define PIN_INCOMINGMESS    V3    // String from terminal
#define PIN_HALL            V4    // Hall sensor
#define PIN_FREE_HEAP       V5    // free heap
#define PIN_FREE_MIN_HEAP   V6    // free min heap

#define PIN_P0              V7     // p0
#define PIN_P1              V8     // p1
#define PIN_P2              V9     // p2
#define PIN_P3              V10    // p3
#define PIN_P4              V11    // p4
#define PIN_P5              V12    // p5

//...
#define PIN_REBOOTMCU       V99 

void timer_BLE_message(void);

#include "client_BLE.h"


// This function sends Arduino's uptime every second.
void timer_1sEvent() {

  unsigned long mlls;
  // double prod_ratio;

  mlls = millis();
  // prod_ratio = (float)cnt / mlls;
  // BLYNK_PRINT.println(prod_ratio);

  Blynk.virtualWrite(PIN_UPTIME, mlls / 1000);
  Blynk.virtualWrite(PIN_COUNTER, cnt); cnt = 0;
  
  // Blynk.virtualWrite(PIN_PRODRATIO, prod_ratio); 

  // Serial.println(hall);
  // Blynk.virtualWrite(PIN_HALL, hall);

}

// This function sends Arduino's uptime every 5 seconds.
void timer_5sEvent() {

  unsigned long fheap = esp_get_free_heap_size();
  // Serial.println(fheap);
  if (fheap_last != fheap)
  {
    Blynk.virtualWrite(PIN_FREE_HEAP, fheap);
    fheap_last = fheap;
  }

  unsigned long min_fheap = esp_get_minimum_free_heap_size();
  // Serial.println(fmheap);
  if (min_fheap_last != min_fheap)
  {
    Blynk.virtualWrite(PIN_FREE_MIN_HEAP, min_fheap);
    min_fheap_last = min_fheap;
  }


  if (min_fheap < 1100) ESP.restart();

  // heap_caps_print_heap_info(MALLOC_CAP_8BIT);
  // Serial.println(heap_caps_check_integrity_all(true));
}


void timer_BLE_message() {
  
  // Serial.println("#");

  if (p0_last != p0)
  {
    Blynk.virtualWrite(PIN_P0, p0);
    p0_last = p0;
  }

  if (p1_last != p1)
  {
    Blynk.virtualWrite(PIN_P1, p1);
    p1_last = p1;
  }
  
  if (p2_last != p2)
  {
    Blynk.virtualWrite(PIN_P2, p2);
    p2_last = p2;
  }
  
  if (p3_last != p3)
  {
    Blynk.virtualWrite(PIN_P3, p3);
    p3_last = p3;
  }
  
  if (p4_last != p4)
  {
    Blynk.virtualWrite(PIN_P4, p4);
    p4_last = p4;
  }
  
  if (p5_last != p5)
  {
    Blynk.virtualWrite(PIN_P5, p5);
    p5_last = p5;
  }
  
  
  // Blynk.virtualWrite(PIN_P1, p1);
  // Blynk.virtualWrite(PIN_P2, p2);
  // Blynk.virtualWrite(PIN_P3, p3);
  // Blynk.virtualWrite(PIN_P4, p4);
  // Blynk.virtualWrite(PIN_P5, p5);
  
}


BLYNK_WRITE(PIN_REBOOTMCU)
{
  String incom_str = param.asStr();
  // Serial.println(incom_str);
  
  Blynk.virtualWrite(PIN_INCOMINGMESS, incom_str);

  // if (incom_str == "reboot") {
  if (incom_str.equals("reboot")) {

    Serial.println("Rebooting...");
  
    // TODO: Perform any neccessary preparation here,
    // i.e. turn off peripherals, write state to EEPROM, etc.
    // NOTE: You may need to defer a reboot,
    // if device is in process of some critical operation.
    ESP.restart();

  } else if (incom_str.equals("ble_on")) {

    Serial.println("ble_on...");

  } else if (incom_str.equals("ble_off")) {

    Serial.println("ble_off...");

  } else {

    send2Ble_server(incom_str);

  }
}


// This function sends Arduino's uptime every second.
void timer_BleEvent() {

  loop_client_BLE();

}


void setup()
{
  Serial.begin(115200);
  delay(1000);

  setup_client_BLE();
  delay(1000);

  BlynkEdgent.begin();

  timer_1s.setInterval(1000L, timer_1sEvent);
  timer_5s.setInterval(5008L, timer_5sEvent);
  timer_30s.setTimeout(300, timer_BLE_message);
  timer_BLE.setInterval(15000L, timer_BleEvent);

}


void loop() {
  
  cnt++;

  BlynkEdgent.run();

  timer_1s.run();
  timer_5s.run();
  // timer_30s.run();
  // timer_BLE.run();

}

