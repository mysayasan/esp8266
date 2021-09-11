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

// Config Button
struct Button
{
    String btnName;
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
    unsigned long TRIG_PREV_MILIS;
    unsigned long TRIG_DELAY;
};
Button DOOR_01 = {"DOOR_01", 14, 0, false};
Button DOOR_02 = {"DOOR_02", 12, 0, false};
Button DOOR_03 = {"DOOR_03", 15, 0, false};
Button DOOR_01_OPEN_STAT = {"DOOR_01_OPEN_STAT", 13, 0, false};
Button SIREN_01 = {"SIREN_01", 14, 0, false};
Button LED_BUILTIN_BLUE = {"LED_BUILTIN_BLUE", 2, 0, false};

struct Sensor
{
    String sensorNm;
    const uint8_t PIN;
    unsigned long TRIG_PREV_MILIS;
    unsigned long TRIG_DELAY;
};

Sensor CURRIN_01 = {"CURRENT_01", 5, millis(), 1000};

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

    if (String(topic) == TOPIC_CMD + "/siren/1/toggle")
    {
        char *ptr;
        SIREN_01.TRIG_DELAY = strtoul(cmd.c_str(), &ptr, 10);
        SIREN_01.TRIG_PREV_MILIS = millis();

        SIREN_01.numberKeyPresses += 1;
        digitalWrite(SIREN_01.PIN, HIGH);
        // delay(uldelay);
        // digitalWrite(SIREN_01.PIN, LOW);

        topicmsg = TOPIC_MSG + "/siren/1/toggle";
        msg = String(SIREN_01.numberKeyPresses);
    }

    if (String(topic) == TOPIC_CMD + "/door/1/toggle")
    {
        char *ptr;
        DOOR_01.TRIG_DELAY = strtoul(cmd.c_str(), &ptr, 10);
        DOOR_01.TRIG_PREV_MILIS = millis();

        DOOR_01.numberKeyPresses += 1;
        digitalWrite(DOOR_01.PIN, HIGH);
        // delay(uldelay);
        // digitalWrite(DOOR_01.PIN, LOW);

        topicmsg = TOPIC_MSG + "/door/1/toggle";
        msg = String(DOOR_01.numberKeyPresses);
    }

    if (String(topic) == TOPIC_CMD + "/door/2/toggle")
    {

        char *ptr;
        DOOR_02.TRIG_DELAY = strtoul(cmd.c_str(), &ptr, 10);
        DOOR_02.TRIG_PREV_MILIS = millis();

        DOOR_02.numberKeyPresses += 1;
        digitalWrite(DOOR_02.PIN, HIGH);
        // delay(uldelay);
        // digitalWrite(DOOR_02.PIN, LOW);

        topicmsg = TOPIC_MSG + "/door/2/toggle";
        msg = String(DOOR_02.numberKeyPresses);
    }

    if (String(topic) == TOPIC_CMD + "/door/3/toggle")
    {
        char *ptr;
        DOOR_03.TRIG_DELAY = strtoul(cmd.c_str(), &ptr, 10);
        DOOR_03.TRIG_PREV_MILIS = millis();

        DOOR_03.numberKeyPresses += 1;
        digitalWrite(DOOR_03.PIN, HIGH);
        // delay(uldelay);
        // digitalWrite(DOOR_03.PIN, LOW);

        topicmsg = TOPIC_MSG + "/door/3/toggle";
        msg = String(DOOR_03.numberKeyPresses);
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

void setupButton()
{
    // Setup pin
    pinMode(DOOR_01.PIN, OUTPUT);
    pinMode(DOOR_02.PIN, OUTPUT);
    // pinMode(DOOR_01_OPEN_STAT.PIN, INPUT_PULLUP);
    pinMode(SIREN_01.PIN, OUTPUT);
    pinMode(DOOR_01_OPEN_STAT.PIN, INPUT);
    pinMode(LED_BUILTIN_BLUE.PIN, OUTPUT);
    //Sensor Pin
    pinMode(CURRIN_01.PIN, INPUT);
    // attachInterrupt(DOOR_01_OPEN_STAT.PIN, isr, RISING);
    // pinMode(DOOR_03.PIN, OUTPUT);
    if (DOOR_01.pressed)
        digitalWrite(DOOR_01.PIN, LOW);
    if (DOOR_02.pressed)
        digitalWrite(DOOR_02.PIN, LOW);
    if (DOOR_03.pressed)
        digitalWrite(DOOR_03.PIN, LOW);
    if (SIREN_01.pressed)
        digitalWrite(SIREN_01.PIN, LOW);
    if (LED_BUILTIN_BLUE.pressed)
        digitalWrite(LED_BUILTIN_BLUE.PIN, LOW);
}

void setup()
{
    Serial.begin(115200);
    setupMQTT();
    connectWifi();
    delay(1500);
    setupButton();
    //   client.setServer(mqtt_server, 1883);
    //   client.setCallback(callback);
}

// Input Voltage Range is 1V to 3.3V
// ESP8266 ADC resolution is 10-bit. 2^10 = 1024
float resolution = 3.3 / 1024;

// const int sensorIn = A0;
int mVperAmp = 185; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

float getVPP()
{
    float result;

    int readValue;       //value read from the sensor
    int maxValue = 0;    // store max value here
    int minValue = 1024; // store min value here

    uint32_t start_time = millis();

    while ((millis() - start_time) < 1000) //sample for 1 Sec
    {
        readValue = analogRead(CURRIN_01.PIN);
        // see if you have a new maxValue
        if (readValue > maxValue)
        {
            /*record the maximum sensor value*/
            maxValue = readValue;
        }
        if (readValue < minValue)
        {
            /*record the maximum sensor value*/
            minValue = readValue;
        }
        /*       Serial.print(readValue);
       Serial.println(" readValue ");
       Serial.print(maxValue);
       Serial.println(" maxValue ");
       Serial.print(minValue);
       Serial.println(" minValue ");
       delay(1000); */
    }

    // Subtract min from max
    result = ((maxValue - minValue) * 5) / 1024.0;

    return result;
}

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

    // DOOR_01_OPEN_STAT.pressed = digitalRead(DOOR_01_OPEN_STAT.PIN) == 1;
    // if ((DOOR_01_OPEN_STAT.pressed) && (currentMillis - msg_previousMillis >= msg_interval))
    // {
    //     String topicmsg = TOPIC_MSG + "/door/1/open";
    //     client.publish(topicmsg.c_str(), "1");
    //     msg_previousMillis = currentMillis;
    // }

    if ((SIREN_01.TRIG_PREV_MILIS > 0) && (currentMillis - SIREN_01.TRIG_PREV_MILIS >= SIREN_01.TRIG_DELAY))
    {
        Serial.printf("Siren stop\n");
        SIREN_01.TRIG_PREV_MILIS = 0;
        digitalWrite(SIREN_01.PIN, HIGH);
    }

    if ((DOOR_01.TRIG_PREV_MILIS > 0) && (currentMillis - DOOR_01.TRIG_PREV_MILIS >= DOOR_01.TRIG_DELAY))
    {
        Serial.printf("Door 1 stop\n");
        DOOR_01.TRIG_PREV_MILIS = 0;
        digitalWrite(DOOR_01.PIN, HIGH);
    }

    if ((DOOR_02.TRIG_PREV_MILIS > 0) && (currentMillis - DOOR_02.TRIG_PREV_MILIS >= DOOR_02.TRIG_DELAY))
    {
        Serial.printf("Door 2 stop\n");
        DOOR_02.TRIG_PREV_MILIS = 0;
        digitalWrite(DOOR_02.PIN, HIGH);
    }

    if ((DOOR_03.TRIG_PREV_MILIS > 0) && (currentMillis - DOOR_03.TRIG_PREV_MILIS >= DOOR_03.TRIG_DELAY))
    {
        Serial.printf("Door 3 stop\n");
        DOOR_03.TRIG_PREV_MILIS = 0;
        digitalWrite(DOOR_03.PIN, HIGH);
    }

    if ((CURRIN_01.TRIG_PREV_MILIS > 0) && (currentMillis - CURRIN_01.TRIG_PREV_MILIS >= CURRIN_01.TRIG_DELAY))
    {
        Serial.print("analogRead = ");
        Serial.println(analogRead(CURRIN_01.PIN)); // Function to read from pin A0

        Serial.print("Voltage (when zero current) = ");
        Serial.print(analogRead(CURRIN_01.PIN) * resolution, 3); // ADC value multp. by resolution will give your the corresponding voltage value.
        Serial.println(" V");
        CURRIN_01.TRIG_PREV_MILIS = millis();
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