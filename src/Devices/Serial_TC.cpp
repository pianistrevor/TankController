#include "Devices/Serial_TC.h"

#include "DateTime_TC.h"
#include "SD_TC.h"
#include "TC_util.h"

/**
 * global serial() functions
 */
void serial(const __FlashStringHelper *format...) {
  va_list args;
  va_start(args, format);
  Serial_TC::instance()->vprintf(format, args);
  va_end(args);
}

void serial(const char *buffer) {
  serial(F("%s"), buffer);
}

/**
 * static variable for singleton
 */
Serial_TC *Serial_TC::_instance = nullptr;

/**
 * static member function to return singleton
 */
Serial_TC *Serial_TC::instance() {
  if (!_instance) {
    _instance = new Serial_TC();
  }
  return _instance;
}

/**
 * constructor (private so clients use the singleton)
 */
Serial_TC::Serial_TC() {
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial)
    ;
}

/**
 * printf() uses a variant of snprintf() so supports the expected formats
 */
// void Serial_TC::vprintf(const char *format, va_list args) {
void Serial_TC::vprintf(const __FlashStringHelper *format, va_list args) {
  char buffer[128];
  vsnprintf_P(buffer, sizeof(buffer), (PGM_P)format, args);
  Serial.println(buffer);
  Serial.flush();
  // need to avoid recursion since SD_TC could call serial()
  if (!printIsActive) {
    printIsActive = true;
    SD_TC::instance()->appendToLog(buffer);
    printIsActive = false;
  }
#ifdef MOCK_PINS_COUNT
#if DEBUG
  std::cout << "\tSERIAL: " << buffer << std::endl;
#endif
#endif
}
