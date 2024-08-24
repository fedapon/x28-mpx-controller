#ifndef MPXController_H
#define MPXController_H

#include <Arduino.h>

// Trama MPX
//----------
// Largo del paquete: 16 bits + 1 bit de start
// Tiempo del paquete: 61ms para 16 bits + bit de start
// Bit de start: 1.27ms
// Bit de datos: 3.8125ms
// 0 is represented by 100
// 1 is represented by 110     

#define BIT_TIME  (1270)  // [useg]
#define ZERO_TIME (2000)  // [useg] must be < (2 * BIT_TIME)
#define IDLE_TIME (5000)  // [useg]
#define CTS_TIME  (25000) // [useg] 5 * IDLE_TIME

#define MPX_BUFFER_LENGTH 64 // size of the buffer of the packages

enum MPXKey {
  MPX_KEY_0, MPX_KEY_1, MPX_KEY_2, MPX_KEY_3, MPX_KEY_4, MPX_KEY_5, MPX_KEY_6, MPX_KEY_7, MPX_KEY_8, MPX_KEY_9,
  MPX_KEY_P, MPX_KEY_P_LONG_PRESS,
  MPX_KEY_F, MPX_KEY_F_LONG_PRESS,
  MPX_KEY_ZONA_IN, MPX_KEY_ZONA_OUT,
  MPX_KEY_MODO,
  MPX_KEY_PANIC, MPX_KEY_PANIC_LONG_PRESS,
  MPX_KEY_FIRE, MPX_KEY_FIRE_LONG_PRESS,
};

enum MPXEvent {
  MPX_EVENT_ALARM_ARMED,
  MPX_EVENT_ALARM_DISARMED,
  MPX_EVENT_ESTOY,
  MPX_EVENT_ME_VOY,
};

class Buffer {
private:
  uint16_t _buffer[MPX_BUFFER_LENGTH];
  uint8_t _writeIndex = 0;
  uint8_t _readIndex = 0;

public:
  void push(uint16_t word) {
    if (isFull()) return; // buffer complete

    if (this->_writeIndex == MPX_BUFFER_LENGTH) {
      if (this->_readIndex == 0) return; // buffer complete
      this->_writeIndex = 0; // if not complete, let's go to the beginning
    }
    this->_buffer[this->_writeIndex++] = word;
  }

  uint16_t read() {
    if (this->isEmpty()) return 0; // buffer empty

    if (this->_readIndex == MPX_BUFFER_LENGTH) {
      if (this->_writeIndex == 0) return 0; // buffer complete
      this->_readIndex = 0; // if not complete, let's go to the beginning
    }
    return this->_buffer[this->_readIndex++];
  }

  bool isEmpty() {
    return this->_readIndex == this->_writeIndex;
  }

  bool isFull() {
    return this->_writeIndex + 1 == this->_readIndex;
  }
};

struct pin_s {
  uint8_t pin = 0;
  uint8_t high = 1;
  uint8_t low = 0;
};

class MPXPacket {
private:
  uint16_t _word;

public:
  MPXPacket(uint16_t word) {
    this->_word = word;
  }
  uint16_t getWord() {
    return this->_word;
  }
  uint16_t getParity() {
    return ((this->_word >> 15) & 0x01);
  }
  uint16_t getId() {
    return ((this->_word >> 12) & 0x07);
  }
  uint16_t getData() {
    return ((this->_word >> 4) & 0xFF);
  }
  uint16_t getChecksum() {
    return (this->_word & 0x0F);
  }
  bool isValid() {
    int count = 0;
    for (int i = 0; i <= 15; i++)
      count += (this->_word >> i) & 0x01;
    return (count % 2 == 0);
  }
};

class MPXController {
private:
  bool _debug = false;
  pin_s _rxPin, _txPin;
  uint16_t _receivedBuffer = 0;
  uint16_t _bitNumber = 0;
  unsigned long _previousMicros = 0;

  Buffer* _buffer;

  void (*_newEventCallback)(MPXEvent) = NULL;

  static void _interruptHandler();
  bool _isKeyboardCode(uint16_t code);
  void _enableTransmit();
  void _disableTransmit();
  void _debugPacket(uint16_t data, bool out = false);

public:
  MPXController(bool debug = false);
  void begin(uint8_t txPin = 0, uint8_t rxPin = 0, bool invertTxPin = true, bool invertRxPin = true);
  void loop(unsigned long timeout = 0);
  void sendPacket(uint16_t payload);
  void sendKey(MPXKey key);
  void sendKeys(String code);
  void setNewEventCallback(void (*callback)(MPXEvent event));
};

#endif // MPXController_H