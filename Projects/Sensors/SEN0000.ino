#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "IPAddress.h"

//Config device
const char *USER_SHA1 = "82C8058388F48C2C9B9A7685EC29C16375E74B71";
const char *DEVICE_UUID = "af9ab3dc-0505-4e15-ad08-4eb76a4b10ec";

// Config WiFi
// const char *SSID = "mysayasan-WiFi-ext";
const char *SSID = "mysayasan-WiFi";
const char *WIFI_PASS = "miliafinayesha2013";
IPAddress ip(192, 168, 0, 24);
IPAddress gw(192, 168, 0, 1);
IPAddress sn(255, 255, 255, 0);

// Config MQTT
WiFiClient espClient;
PubSubClient client(espClient);
const char *mqtt_server = "192.168.0.69";
// const uint8_t mqtt_server[] = {192,168,0,141};
// const IPAddress mqtt_server(192, 168, 0, 69);
// const IPAddress mqtt_server(192, 168, 0, 69);
// byte mqtt_server[] = { 192,168,0,69 };
int mqtt_port = 1883;
unsigned long msg_interval = 1000;
unsigned long msg_previousMillis = 0;
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
String TOPIC_ID;
String TOPIC_CMD;
String TOPIC_MSG;
unsigned long wifi_previousMillis = 0;
unsigned long wifi_interval = 30000;

// Config Switch
struct Switch
{
    String btnName;
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
    unsigned long TRIG_PREV_MILIS;
    unsigned long TRIG_DELAY;
};
Switch SWITCH_01 = {"SWITCH_01", 14, 0, false};
Switch SWITCH_02 = {"SWITCH_02", 12, 0, false};
Switch SWITCH_03 = {"SWITCH_03", 15, 0, false};
Switch SWITCH_04 = {"SWITCH_04", 14, 0, false};
Switch LED_BUILTIN_BLUE = {"LED_BUILTIN_BLUE", 2, 0, false};

struct Sensor
{
    String sensorNm;
    const uint8_t PIN;
    unsigned long TRIG_PREV_MILIS;
    unsigned long TRIG_DELAY;
};

Sensor CURRENT_01 = {"CURRENTIN_01", A0, millis(), 1000};
Sensor SWITCH_01_OPEN_STAT = {"SWITCH_01_OPEN_STAT", 13, millis(), 1000};

void connectWifi()
{
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    String hostNm = "SEN0000-";
    hostNm += String(random(0xffff), HEX);

    WiFi.mode(WIFI_STA);
    // WiFi.enableInsecureWEP(true);
    WiFi.hostname(hostNm);
    WiFi.config(ip, gw, sn, gw);
    WiFi.begin(SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// void callback(char *topic, byte *payload, unsigned int length)
// {
//     Serial.print("Message arrived [");
//     Serial.print(topic);
//     Serial.print("] ");
//     for (int i = 0; i < length; i++)
//     {
//         Serial.print((char)payload[i]);
//     }
//     Serial.println();

//     // Switch on the LED if an 1 was received as first character
//     if ((char)payload[0] == '1')
//     {
//         digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
//                                         // but actually the LED is on; this is because
//                                         // it is active low on the ESP-01)
//     }
//     else
//     {
//         digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
//     }
// }

void callback(char *topic, byte *payload, unsigned int length)
{
    String topicmsg;
    String cmd;
    String msg;

    for (int i = 0; i < length; i++)
    {
        cmd += (char)payload[i];
    }

    Serial.printf("callback topic: %s, message: %s\n", topic, cmd.c_str());    

    if (String(topic) == TOPIC_CMD + "/switch/1/timer")
    {
        char *ptr;
        SWITCH_01.TRIG_DELAY = strtoul(cmd.c_str(), &ptr, 10);
        SWITCH_01.TRIG_PREV_MILIS = millis();

        SWITCH_01.numberKeyPresses += 1;
        digitalWrite(SWITCH_01.PIN, HIGH);
        // delay(uldelay);
        // digitalWrite(SWITCH_01.PIN, LOW);

        topicmsg = TOPIC_MSG + "/switch/1/timer";
        msg = String(SWITCH_01.numberKeyPresses);
    }

    if (String(topic) == TOPIC_CMD + "/switch/2/timer")
    {

        char *ptr;
        SWITCH_02.TRIG_DELAY = strtoul(cmd.c_str(), &ptr, 10);
        SWITCH_02.TRIG_PREV_MILIS = millis();

        SWITCH_02.numberKeyPresses += 1;
        digitalWrite(SWITCH_02.PIN, HIGH);
        // delay(uldelay);
        // digitalWrite(SWITCH_02.PIN, LOW);

        topicmsg = TOPIC_MSG + "/switch/2/timer";
        msg = String(SWITCH_02.numberKeyPresses);
    }

    if (String(topic) == TOPIC_CMD + "/switch/3/timer")
    {
        char *ptr;
        SWITCH_03.TRIG_DELAY = strtoul(cmd.c_str(), &ptr, 10);
        SWITCH_03.TRIG_PREV_MILIS = millis();

        SWITCH_03.numberKeyPresses += 1;
        digitalWrite(SWITCH_03.PIN, HIGH);
        // delay(uldelay);
        // digitalWrite(SWITCH_03.PIN, LOW);

        topicmsg = TOPIC_MSG + "/switch/3/timer";
        msg = String(SWITCH_03.numberKeyPresses);
    }

    if (String(topic) == TOPIC_CMD + "/switch/4/timer")
    {
        char *ptr;
        SWITCH_04.TRIG_DELAY = strtoul(cmd.c_str(), &ptr, 10);
        SWITCH_04.TRIG_PREV_MILIS = millis();

        SWITCH_04.numberKeyPresses += 1;
        digitalWrite(SWITCH_04.PIN, HIGH);
        // delay(uldelay);
        // digitalWrite(SWITCH_04.PIN, LOW);

        topicmsg = TOPIC_MSG + "/switch/4/timer";
        msg = String(SWITCH_04.numberKeyPresses);
    }

    if (String(topic) == TOPIC_CMD + "/uptime")
    {
        topicmsg = TOPIC_MSG + "/uptime";

        char buff[20];
        // sprintf(buff, "%" PRIu64, (((esp_timer_get_time() + 500) / 1000) + 500) / 1000);
        msg = String(buff);
    }

    if (msg.length() > 0)
    {
        client.publish(topicmsg.c_str(), msg.c_str());
        Serial.printf("topic: %s, payload: %s\n", topicmsg.c_str(), msg.c_str());
    }
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected() && WiFi.status() == WL_CONNECTED)
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        // String clientId = "ESP8266Client-";
        // clientId += String(random(0xffff), HEX);
        Serial.println("Reconnecting to MQTT Broker..");
        String topic_cmd = String(TOPIC_CMD) + "/#";

        // Bug
        // Attempt to connect
        if (client.connect(DEVICE_UUID, "mysayasan", "Aziand12", TOPIC_MSG.c_str(), 1, false, "offline"))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish(TOPIC_MSG.c_str(), "online");
            // ... and resubscribe
            client.subscribe(topic_cmd.c_str());
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }

    if (!LED_BUILTIN_BLUE.pressed)
    {
        Serial.printf("Blue in \n");
        LED_BUILTIN_BLUE.pressed = true;
        digitalWrite(LED_BUILTIN_BLUE.PIN, LOW);
    }
}

void setupMQTT()
{
    TOPIC_ID = String(USER_SHA1) + "/" + String(DEVICE_UUID);
    TOPIC_CMD = TOPIC_ID + "/cmd";
    Serial.printf("topic cmd set to %s\n", TOPIC_CMD.c_str());
    TOPIC_MSG = TOPIC_ID + "/msg";
    Serial.printf("topic msg set to %s\n", TOPIC_MSG.c_str());
    client.setServer(mqtt_server, mqtt_port);
    // set the callback function
    client.setCallback(callback);
}

void setupSwitches()
{
    // Setup pin
    pinMode(SWITCH_01.PIN, OUTPUT);
    pinMode(SWITCH_02.PIN, OUTPUT);
    // pinMode(SWITCH_01_OPEN_STAT.PIN, INPUT_PULLUP);
    pinMode(SWITCH_04.PIN, OUTPUT);
    pinMode(SWITCH_01_OPEN_STAT.PIN, INPUT);
    pinMode(LED_BUILTIN_BLUE.PIN, OUTPUT);
    // attachInterrupt(SWITCH_01_OPEN_STAT.PIN, isr, RISING);
    // pinMode(SWITCH_03.PIN, OUTPUT);
    if (SWITCH_01.pressed)
        digitalWrite(SWITCH_01.PIN, HIGH);
    if (SWITCH_02.pressed)
        digitalWrite(SWITCH_02.PIN, HIGH);
    if (SWITCH_03.pressed)
        digitalWrite(SWITCH_03.PIN, HIGH);
    if (SWITCH_04.pressed)
        digitalWrite(SWITCH_04.PIN, HIGH);
    if (LED_BUILTIN_BLUE.pressed)
        digitalWrite(LED_BUILTIN_BLUE.PIN, LOW);
}

void setupSensors()
{
    //Sensor Pin
    pinMode(CURRENT_01.PIN, INPUT);
}

void setup()
{
    Serial.begin(115200);
    setupMQTT();                                                                                                 
    connectWifi();
    delay(1500);
    setupSensors();
    setupSwitches();
}

// Input Voltage Range is 1V to 3.3V
// ESP8266 ADC resolution is 10-bit. 2^10 = 1024
const float ADCRES = 3.3 / 1024;

// One period of a periodic waveform hertz (Malaysia 50)
uint32_t COUNTRY_VOLTAGE = 230;
uint32_t period = 1000000 / 50;
uint32_t t_start = 0;
const float ACS712_SENSITIVITY = 0.185; //185 mV/A, 100 mV/A and 0.66 mV/A for ±5A, ±20A and ±30A current range respectively.
const float CURR_OFFSET = 0.0147; //Current offset during initial.

// setup input value when no load
// float zero_ADC_Value = 832, zero_voltageValue = 0;
float zero_ADC_Value = 829, zero_voltageValue = 0;

void loop()
{
    unsigned long currentMillis = millis();
    // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - wifi_previousMillis > wifi_interval))
    {
        Serial.print(millis());
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        connectWifi();
        wifi_previousMillis = currentMillis;
    }

    if (!client.connected())                                                                                                                                                                                                                                                                         
    {
        if (LED_BUILTIN_BLUE.pressed)
        {
            Serial.printf("Blue out %s\n");
            LED_BUILTIN_BLUE.pressed = false;
            digitalWrite(LED_BUILTIN_BLUE.PIN, HIGH);
        }
        reconnect();
    }

    // SWITCH_01_OPEN_STAT.pressed = digitalRead(SWITCH_01_OPEN_STAT.PIN) == 1;
    // if ((SWITCH_01_OPEN_STAT.pressed) && (currentMillis - msg_previousMillis >= msg_interval))
    // {
    //     String topicmsg = TOPIC_MSG + "/switch/1/open";
    //     client.publish(topicmsg.c_str(), "1");
    //     msg_previousMillis = currentMillis;
    // }    

    if ((SWITCH_01.TRIG_PREV_MILIS > 0) && (currentMillis - SWITCH_01.TRIG_PREV_MILIS >= SWITCH_01.TRIG_DELAY))
    {
        Serial.printf("Switch 1 stop\n");
        SWITCH_01.TRIG_PREV_MILIS = 0;
        digitalWrite(SWITCH_01.PIN, LOW);
    }

    if ((SWITCH_02.TRIG_PREV_MILIS > 0) && (currentMillis - SWITCH_02.TRIG_PREV_MILIS >= SWITCH_02.TRIG_DELAY))
    {
        Serial.printf("Switch 2 stop\n");
        SWITCH_02.TRIG_PREV_MILIS = 0;
        digitalWrite(SWITCH_02.PIN, LOW);
    }

    if ((SWITCH_03.TRIG_PREV_MILIS > 0) && (currentMillis - SWITCH_03.TRIG_PREV_MILIS >= SWITCH_03.TRIG_DELAY))
    {
        Serial.printf("Switch 3 stop\n");
        SWITCH_03.TRIG_PREV_MILIS = 0;
        digitalWrite(SWITCH_03.PIN, LOW);
    }

    if ((SWITCH_04.TRIG_PREV_MILIS > 0) && (currentMillis - SWITCH_04.TRIG_PREV_MILIS >= SWITCH_04.TRIG_DELAY))
    {
        Serial.printf("Switch 4 stop\n");
        SWITCH_04.TRIG_PREV_MILIS = 0;
        digitalWrite(SWITCH_04.PIN, LOW);
    }

    if ((CURRENT_01.TRIG_PREV_MILIS > 0) && (currentMillis - CURRENT_01.TRIG_PREV_MILIS >= CURRENT_01.TRIG_DELAY))
    {
        // Serial.print("analogRead = ");
        // Serial.println(analogRead(CURRENT_01.PIN)); // Function to read from pin A0

        // Serial.print("Voltage (when zero current) = ");
        // Serial.print(analogRead(CURRENT_01.PIN) * ADCRES, 3); // ADC value multp. by resolution will give your the corresponding voltage value.
        // Serial.println(" V");
        // CURRENT_01.TRIG_PREV_MILIS = millis();

        float ADC = 0, Vrms = 0, Current = 0, Q = 0.0147, Watt = 0;        
        uint32_t ADC_Dif = 0, ADC_SUM = 0, m = 0;

        t_start = micros();
        while (micros() - t_start < period)
        {                                                         // Defining one period of the waveform. US frequency(f) is 60Hz. Period = 1/f = 0.016 seg = 16,666 microsec
            ADC_Dif = zero_ADC_Value - analogRead(CURRENT_01.PIN); // To start from 0V we need to subtracting our initial value when no current passes through the current sensor, (i.e. 750 or 2.5V).
            ADC_SUM += ADC_Dif * ADC_Dif;                         // SUM of the square
            m++;                                                  // counter to be used for avg.
        }
        ADC = sqrt(ADC_SUM / m);                   // The root-mean-square ADC value.
        Vrms = ADC * ADCRES;                       // The root-mean-square analog voltage value.
        // Current = (Vrms / ACS712_SENSITIVITY) - Q; // The root-mean-square analog current value. Note: Q
        Current = (Vrms / ACS712_SENSITIVITY) - CURR_OFFSET; // The root-mean-square analog current value. Note: Q
        Watt = COUNTRY_VOLTAGE * Current;
        //------------------------------//

        Serial.print("analogRead = ");
        Serial.println(analogRead(CURRENT_01.PIN));

        Serial.print("Vrms = ");
        Serial.print(Vrms, 6);
        Serial.println(" V");

        Serial.print("Watt = ");
        Serial.println(Watt, 6);

        Serial.print("Irms = ");
        Serial.print(Current, 6);
        Serial.println(" A");
        Serial.print("\n");

        char msg[30];
        sprintf(msg, "%g,%g,%g", Vrms,Current,Watt);
        
        String topicmsg = TOPIC_MSG + "/current/1/read";        
        client.publish(topicmsg.c_str(), msg);
        CURRENT_01.TRIG_PREV_MILIS = millis();
    }

    // Voltage = getVPP();
    // VRMS = (Voltage / 2.0) * 0.707; // sq root
    // AmpsRMS = (VRMS * 1000) / mVperAmp;
    // float Wattage = (220 * AmpsRMS) - 18; //Observed 18-20 Watt when no load was connected, so substracting offset value to get real consumption.
    // Serial.print(AmpsRMS);
    // Serial.println(" Amps RMS ");
    // Serial.print(Wattage);
    // Serial.println(" Watt ");

    client.loop();
}