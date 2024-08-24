#include <Arduino.h>
#include <MPXController.h>

#define debuggerPort Serial
#define debug(...) debuggerPort.print(String(millis()) + " | " + __VA_ARGS__)
#define debugln(...) debuggerPort.println(String(millis()) + " | " + __VA_ARGS__)

MPXController mpxAlarm(true); // with true activate loggin messages

void setup()
{
    Serial.begin(19200);

    mpxAlarm.begin(D1, D2);
    // mpxAlarm.begin(D1, D2, false, false); // usefull for testing

    mpxAlarm.setNewEventCallback([](MPXEvent event) {
        switch (event) {
            case MPX_EVENT_ALARM_ARMED:
                debugln(F("MPX -> MPX_EVENT_ALARM_ARMED")); break;
            case MPX_EVENT_ALARM_DISARMED:
                debugln(F("MPX -> MPX_EVENT_ALARM_DISARMED")); break;
            case MPX_EVENT_ALARM_RINGING:
                debugln(F("MPX -> MPX_EVENT_ALARM_RINGING")); break;
            case MPX_EVENT_ESTOY:
                debugln(F("MPX -> MPX_EVENT_ESTOY")); break;
            case MPX_EVENT_ME_VOY:
                debugln(F("MPX -> MPX_EVENT_ME_VOY")); break;
        }
    });
}

void loop()
{
    // main loop that process new data
    mpxAlarm.loop();

    // simulate keyboard
    mpxAlarm.sendKey(MPX_KEY_1);
    mpxAlarm.sendKey(MPX_KEY_2);
    mpxAlarm.sendKey(MPX_KEY_3);
    mpxAlarm.sendKey(MPX_KEY_4);
    mpxAlarm.sendKey(MPX_KEY_PANIC);
    mpxAlarm.sendKey(MPX_KEY_F_LONG_PRESS);
    delay(1000);

    // send number keys to activate/desactivate
    mpxAlarm.sendKeys("123456");
    delay(1000);

    // send a packet
    mpxAlarm.sendPacket(0x49C1);
    delay(1000);
}