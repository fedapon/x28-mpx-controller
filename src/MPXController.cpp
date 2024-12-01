#include "MPXController.h"

#if defined(ESP8266) || defined(ESP32)
#define RECEIVE_ATTR IRAM_ATTR
#else
#define RECEIVE_ATTR
#endif

#define debuggerPort    Serial

static MPXController* _mpxInstance;

const uint16_t KEYBOARD_CODES[] = {
    0x0000, 0x8013, 0x8025, 0x0036, 0x8046, 0x0055, 0x0063, 0x8070,
    0x8089, 0x009A, 0x00AC, 0x80BF, 0x00CF, 0x80DC, 0x80EA, 0x00F9,
    0x00AC, 0x810A, 0x80BF, 0x813C, 0x00F9, 0x0119, 0x80EA, 0x012F,
    0x8169
};

enum MPXCODE {
  MPX_CODE_ALARM_ARMED = 0x49C1,
  MPX_CODE_ALARM_DISARMED = 0xC92B,
  MPX_CODE_ESTOY = 0x4BE8,
  MPX_CODE_ME_VOY = 0xCBAE,
  MPX_CODE_Z1_MPXH = 0x1615,
  MPX_CODE_Z2_MPXH = 0x1623,
  MPX_CODE_Z3_MPXH = 0x9630,
  MPX_CODE_Z4_MPXH = 0x1640,
  MPX_CODE_Z1_WIRED = 0xB08A,
  MPX_CODE_Z2_WIRED = 0xB045,
  MPX_CODE_Z3_WIRED = 0xB026,
  MPX_CODE_Z4_WIRED = 0xB010
};

MPXController::MPXController(bool debug) {
  this->_debug = debug;
}

void MPXController::debugln(String log) {
  if (this->_debug) {
    String msg = String(millis()) + " | MPX -> " + log;
    if (this->_loggerCallback != NULL)
      this->_loggerCallback(msg);
    else
      debuggerPort.println(msg);
  }
}

void MPXController::begin(uint8_t txPin, uint8_t rxPin, bool invertTxPin, bool invertRxPin) {
  _mpxInstance = this;
  this->_buffer = new Buffer();
  this->_txPin.pin = txPin;
  if (invertTxPin) {
    this->_txPin.high = 0;
    this->_txPin.low = 1;
  }
  this->_rxPin.pin = rxPin;
  if (invertRxPin) {
    this->_rxPin.high = 0;
    this->_rxPin.low = 1;
  }
  pinMode(this->_rxPin.pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(this->_rxPin.pin), this->_interruptHandler, CHANGE);
  debugln(F("begin()"));
  debugln("txPin: " + String(this->_txPin.pin) + " | txPin.high: " + String(this->_txPin.high) + " | txPin.low: " + String(this->_txPin.low));
  debugln("rxPin: " + String(this->_rxPin.pin) + " | rxPin.high: " + String(this->_rxPin.high) + " | rxPin.low: " + String(this->_rxPin.low));
}

void MPXController::setNewEventCallback(void (*callback)(MPXEvent event)) {
  this->_newEventCallback = callback;
}

void MPXController::setLoggerCallback(void (*callback)(String log)) {
  this->_loggerCallback = callback;
}

void MPXController::loop(unsigned long timeout) {
  unsigned long startMillis = millis();
  do {
    if (!this->_buffer->isEmpty()) {
      MPXPacket packet(this->_buffer->read());
      if (!packet.isValid())
        continue;
      this->_debugPacket(packet.getWord());

      switch (packet.getWord()) {
      case MPX_CODE_ALARM_ARMED:
        if (this->_newEventCallback != NULL)
          this->_newEventCallback(MPX_EVENT_ALARM_ARMED);
        break;
      case MPX_CODE_ALARM_DISARMED:
        if (this->_newEventCallback != NULL)
          this->_newEventCallback(MPX_EVENT_ALARM_DISARMED);
        break;
      case MPX_CODE_ESTOY:
        if (this->_newEventCallback != NULL)
          this->_newEventCallback(MPX_EVENT_ESTOY);
        break;
      case MPX_CODE_ME_VOY:
        if (this->_newEventCallback != NULL)
          this->_newEventCallback(MPX_EVENT_ME_VOY);
        break;
      case MPX_CODE_Z1_MPXH:
      case MPX_CODE_Z1_WIRED:
        if (this->_newEventCallback != NULL)
          this->_newEventCallback(MPX_EVENT_SENSORS_Z1);
        break;
      case MPX_CODE_Z2_MPXH:
      case MPX_CODE_Z2_WIRED:
        if (this->_newEventCallback != NULL)
          this->_newEventCallback(MPX_EVENT_SENSORS_Z2);
        break;
      case MPX_CODE_Z3_MPXH:
      case MPX_CODE_Z3_WIRED:
        if (this->_newEventCallback != NULL)
          this->_newEventCallback(MPX_EVENT_SENSORS_Z3);
        break;
      case MPX_CODE_Z4_MPXH:
      case MPX_CODE_Z4_WIRED:
        if (this->_newEventCallback != NULL)
          this->_newEventCallback(MPX_EVENT_SENSORS_Z4);
        break;
      }
    }
    yield();
  } while (((millis() - startMillis) < timeout) | !this->_buffer->isEmpty());
}

void MPXController::sendKey(MPXKey key) {
  switch (key) {
  case MPX_KEY_0:
    return this->sendPacket(0x0000);
  case MPX_KEY_1:
    return this->sendPacket(0x8013);
  case MPX_KEY_2:
    return this->sendPacket(0x8025);
  case MPX_KEY_3:
    return this->sendPacket(0x0036);
  case MPX_KEY_4:
    return this->sendPacket(0x8046);
  case MPX_KEY_5:
    return this->sendPacket(0x0055);
  case MPX_KEY_6:
    return this->sendPacket(0x0063);
  case MPX_KEY_7:
    return this->sendPacket(0x8070);
  case MPX_KEY_8:
    return this->sendPacket(0x8089);
  case MPX_KEY_9:
    return this->sendPacket(0x009A);
  case MPX_KEY_P:
    return this->sendPacket(0x00AC);
  case MPX_KEY_P_LONG_PRESS:
    this->sendPacket(0x00AC);
    this->sendPacket(0x810A);
    return;
  case MPX_KEY_F:
    return this->sendPacket(0x80BF);
  case MPX_KEY_F_LONG_PRESS:
    this->sendPacket(0x80BF);
    this->sendPacket(0x813C);
    return;
  case MPX_KEY_ZONA_IN:
    this->sendPacket(0x00CF);
    this->sendPacket(0x0000);
    return;
  case MPX_KEY_ZONA_OUT:
    this->sendPacket(0x00CF);
    this->sendPacket(0x8169);
    return;
  case MPX_KEY_MODO:
    return this->sendPacket(0x80DC);
  case MPX_KEY_PANIC:
    return this->sendPacket(0x80EA);
  case MPX_KEY_PANIC_LONG_PRESS:
    this->sendPacket(0x80EA);
    this->sendPacket(0x012F);
    return;
  case MPX_KEY_FIRE:
    return this->sendPacket(0x00F9);
  case MPX_KEY_FIRE_LONG_PRESS:
    this->sendPacket(0x00F9);
    this->sendPacket(0x0119);
    return;
  }
}

void MPXController::sendKeys(String keys) {
  debugln(F("Sending keys: ") + keys);
  for (unsigned int i = 0; i < keys.length(); i++) {
    const int key = keys.charAt(i) - '0';
    this->sendKey((MPXKey)key);
  }
}

bool MPXController::_isKeyboardCode(uint16_t keyCode) {
  for (size_t i = 0; i < sizeof(KEYBOARD_CODES) / 2; i++) {
    if (KEYBOARD_CODES[i] == keyCode)
      return true;
  }
  return false;
}

void MPXController::_enableTransmit() {
  noInterrupts();
  pinMode(this->_txPin.pin, OUTPUT);
}
void MPXController::_disableTransmit() {
  pinMode(this->_txPin.pin, INPUT);
  interrupts();
}

void RECEIVE_ATTR MPXController::_interruptHandler() {
  if (!_mpxInstance) return;
  unsigned long currentMicros = micros();
  unsigned long length = currentMicros - _mpxInstance->_previousMicros;

  if (digitalRead(_mpxInstance->_rxPin.pin) == _mpxInstance->_rxPin.low) {
    if (length > IDLE_TIME) {
      _mpxInstance->_receivedBuffer = 0;
      _mpxInstance->_bitNumber = 0;
    }
    else {
      _mpxInstance->_receivedBuffer = _mpxInstance->_receivedBuffer << 1;
      if (length > ZERO_TIME)
        _mpxInstance->_receivedBuffer |= 1;

      if (++_mpxInstance->_bitNumber == 16) {
        _mpxInstance->_buffer->push(_mpxInstance->_receivedBuffer);
        _mpxInstance->_receivedBuffer = 0;
        _mpxInstance->_bitNumber = 0;
      }
    }
  }
  _mpxInstance->_previousMicros = currentMicros;
}

void MPXController::sendPacket(uint16_t payload) {
  this->_debugPacket(payload, true);
  // wait until bus is free
  unsigned long currentMicros = micros();
  do {
    if (digitalRead(this->_rxPin.pin) == _mpxInstance->_rxPin.low)
      currentMicros = micros();
    yield();
  } while ((micros() - currentMicros) < CTS_TIME);

  this->_enableTransmit();
  // start bit
  digitalWrite(this->_txPin.pin, this->_rxPin.low);
  delayMicroseconds(BIT_TIME);

  // 16 bits word
  for (int i = 0; i < 16; i++) {
    if (!(payload & (unsigned int)0x8000)) {
      // 0 is represented by 100
      digitalWrite(this->_txPin.pin, this->_txPin.high);
      delayMicroseconds(BIT_TIME);
      digitalWrite(this->_txPin.pin, this->_txPin.low);
      delayMicroseconds(2 * BIT_TIME);
    }
    else {
      // 1 is represented by 110      
      digitalWrite(this->_txPin.pin, this->_txPin.high);
      delayMicroseconds(2 * BIT_TIME);
      digitalWrite(this->_txPin.pin, this->_txPin.low);
      delayMicroseconds(BIT_TIME);
    }
    payload = payload << 1;
  }

  // stop bit
  digitalWrite(this->_txPin.pin, this->_txPin.high);
  delayMicroseconds(BIT_TIME);
  this->_disableTransmit();
}

void MPXController::_debugPacket(uint16_t data, bool out) {
  if (!this->_debug) return;
  MPXPacket packet(data);
  String direction = out ? ">>" : "<<";
  String isKeyboard = this->_isKeyboardCode(packet.getWord()) ? "1" : "0";
  debugln("Packet | " + direction + " | isKB: " + isKeyboard
    + " | word: " + String(packet.getWord(), BIN) + "b (0x" + String(packet.getWord(), HEX) + ")"
    + " | parity: " + String(packet.getParity())
    + " | id: " + String(packet.getId())
    + " | data: " + String(packet.getData())
    + " | checksum: " + String(packet.getChecksum()));
}