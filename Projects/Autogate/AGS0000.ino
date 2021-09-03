#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Config device
const char *USER_SHA1 = "82C8058388F48C2C9B9A7685EC29C16375E74B71";
const char *DEVICE_UUID = "8168b630-0221-11ec-9a03-0242ac130003";

// Config WiFi
// const char *SSID = "mysayasan-WiFi-ext";
const char *SSID = "mysayasan";
const char *WIFI_PASS = "miliafinayesha2013";

// Config MQTT
WiFiClient espClient;
PubSubClient client(espClient);
const char *mqtt_server = "mysayasan.asuscomm.com";
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
};
Button DOOR_01 = {"DOOR_01", 14, 0, true};
Button DOOR_02 = {"DOOR_02", 12, 0, true};
Button DOOR_03 = {"DOOR_03", 15, 0, true};
Button DOOR_01_OPEN_STAT = {"DOOR_01_OPEN_STAT", 13, 0, true};
Button SIREN_01 = {"SIREN_01", 14, 0, true};
Button LED_BUILTIN_BLUE = {"LED_BUILTIN_BLUE", 2, 0, true};

void setupWiFi()
{

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.mode(WIFI_STA);
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

    if (String(topic) == TOPIC_CMD + "/door/1/toggle")
    {
        if (cmd == "1")
        {
            DOOR_01.numberKeyPresses += 1;
            digitalWrite(DOOR_01.PIN, LOW);
            delay(1000);
            digitalWrite(DOOR_01.PIN, HIGH);

            topicmsg = TOPIC_MSG + "/door/1/toggle";
            msg = String(DOOR_01.numberKeyPresses);
        }
    }

    if (String(topic) == TOPIC_CMD + "/door/2/toggle")
    {

        if (cmd == "1")
        {
            DOOR_02.numberKeyPresses += 1;
            digitalWrite(DOOR_02.PIN, LOW);
            delay(1000);
            digitalWrite(DOOR_02.PIN, HIGH);

            topicmsg = TOPIC_MSG + "/door/2/toggle";
            msg = String(DOOR_02.numberKeyPresses);
        }
    }

    if (String(topic) == TOPIC_CMD + "/door/3/toggle")
    {
        if (cmd == "1")
        {
            DOOR_03.numberKeyPresses += 1;
            digitalWrite(DOOR_03.PIN, LOW);
            delay(1000);
            digitalWrite(DOOR_03.PIN, HIGH);

            topicmsg = TOPIC_MSG + "/door/3/toggle";
            msg = String(DOOR_03.numberKeyPresses);
        }
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
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        // String clientId = "ESP8266Client-";
        // clientId += String(random(0xffff), HEX);
        Serial.println("Reconnecting to MQTT Broker..");
        String topic_cmd = String(TOPIC_CMD) + "/#";
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
    // attachInterrupt(DOOR_01_OPEN_STAT.PIN, isr, RISING);
    // pinMode(DOOR_03.PIN, OUTPUT);
    if (DOOR_01.pressed)
        digitalWrite(DOOR_01.PIN, HIGH);
    if (DOOR_02.pressed)
        digitalWrite(DOOR_02.PIN, HIGH);
    if (DOOR_03.pressed)
        digitalWrite(DOOR_03.PIN, HIGH);
    if (SIREN_01.pressed)
        digitalWrite(SIREN_01.PIN, HIGH);
    if (LED_BUILTIN_BLUE.pressed) {
        digitalWrite(LED_BUILTIN_BLUE.PIN, HIGH);
    }
}

void setup()
{
    Serial.begin(115200);
    setupWiFi();
    setupMQTT();
    setupButton();
    //   client.setServer(mqtt_server, 1883);
    //   client.setCallback(callback);
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
        WiFi.reconnect();
        wifi_previousMillis = currentMillis;
    }

    if (!client.connected())
    {
        reconnect();
    }

    DOOR_01_OPEN_STAT.pressed = digitalRead(DOOR_01_OPEN_STAT.PIN) == 1;
    if ((DOOR_01_OPEN_STAT.pressed) && (currentMillis - msg_previousMillis >= msg_interval))
    {
        String topicmsg = TOPIC_MSG + "/door/1/open";
        client.publish(topicmsg.c_str(), "1");
        msg_previousMillis = currentMillis;
    }

    if (DOOR_01_OPEN_STAT.pressed)
    {
        if (!LED_BUILTIN_BLUE.pressed)
        {
            Serial.printf("Blue in %s\n", String(DOOR_01_OPEN_STAT.pressed));
            LED_BUILTIN_BLUE.pressed = true;
            digitalWrite(LED_BUILTIN_BLUE.PIN, LOW);
        }
    }
    else
    {
        if (LED_BUILTIN_BLUE.pressed)
        {
            Serial.printf("Blue out %s\n", String(DOOR_01_OPEN_STAT.pressed));
            LED_BUILTIN_BLUE.pressed = false;
            digitalWrite(LED_BUILTIN_BLUE.PIN, HIGH);
        }
    }

    client.loop();

    // unsigned long now = millis();
    // if (now - lastMsg > 2000)
    // {
    //     lastMsg = now;
    //     ++value;
    //     snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    //     Serial.print("Publish message: ");
    //     Serial.println(msg);
    //     client.publish("outTopic", msg);
    // }
}