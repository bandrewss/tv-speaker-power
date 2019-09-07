// bandrewss
// 2019.08.25

//#define VERBOSE

#include <IRremote.h>
#include <IRremoteInt.h>

#define OVERRIDE_BUTTON (2)
#define RED_LED (4)
#define GREEN_LED (5)
#define RELAY_PIN (6)
#define RECV_PIN (7)

#define RED (0)
#define GREEN (1)

#define DEBOUNCE_TIME (200)
#define POWER_CODE (0x61A0F00F)
// other power code?: 0x438E0879

static IRrecv recv(RECV_PIN);
static decode_results results;

static int relay_state = false;
static int rg_state = RED;

static volatile long time_override = 0;

void override_button()
{
    if( (millis() - time_override) < DEBOUNCE_TIME )
        return;

    time_override = millis();

#ifdef VERBOSE
    Serial.println("Button Pressed, Overriding");
#endif

    toggle();

#ifdef VERBOSE
    Serial.print("\n");
#endif
}


void rg_toggle()
{
    if(rg_state == RED)
    {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        rg_state = GREEN;
#ifdef VERBOSE
        Serial.print("Light is now Green\n");
#endif
    }
    else
    {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        rg_state = RED;
#ifdef VERBOSE
        Serial.print("Light is now RED\n");
#endif
    }
}


void toggle()
{
    rg_toggle();

    relay_state = !relay_state;
    digitalWrite(RELAY_PIN, relay_state);
#ifdef VERBOSE
        Serial.print("Relay is now ");
        Serial.print(relay_state ? "On\n" : "Off\n");
#endif
}


void setup() 
{
    Serial.begin(115200);
#ifdef VERBOSE
    Serial.print("Starting\n");
    Serial.print("Light is Red\n");
    Serial.print("Relay is Off\n\n");
#endif

    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);

    pinMode(GREEN_LED, OUTPUT);
    digitalWrite(GREEN_LED, LOW);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, relay_state);

    pinMode(OVERRIDE_BUTTON, INPUT);
    attachInterrupt(digitalPinToInterrupt(OVERRIDE_BUTTON), override_button, RISING);
  
    recv.enableIRIn();
}


void loop() 
{
    if(recv.decode(&results))
    {
#ifdef VERBOSE
        Serial.print("Found IR code:");
        Serial.println(results.value, HEX);
#endif

        if(results.value == POWER_CODE)
            toggle();

#ifdef VERBOSE
        Serial.print("\n");
#endif
        
        recv.resume();
    }
}
