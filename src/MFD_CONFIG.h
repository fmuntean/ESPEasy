

#undef DEFAULT_TIME_ZONE
#define DEFAULT_TIME_ZONE            -300               // Time Offset (in minutes)
#undef DEFAULT_USE_DST
#define DEFAULT_USE_DST              true   // (true|false) Use Daily Time Saving


#define DEFAULT_LATITUDE    42.78F
#define DEFAULT_LONGITUDE   -71.07F

// MQTT_KEEPALIVE : keepAlive interval in Seconds
// Keepalive timeout for default MQTT Broker is 10s
#define MQTT_KEEPALIVE 60

// MQTT_SOCKET_TIMEOUT: socket timeout interval in Seconds the default is 15s
#define MQTT_SOCKET_TIMEOUT 30

#define DEFAULT_IP_BLOCK_LEVEL               0 // 0: ALL_ALLOWED  1: LOCAL_SUBNET_ALLOWED  2: ONLY_IP_RANGE_ALLOWED

// --- Default Controller ------------------------------------------------------------------------------
#define DEFAULT_CONTROLLER   true                                          // true or false enabled or disabled, set 1st controller
                                                                            // defaults
#define DEFAULT_CONTROLLER_ENABLED true                                     // Enable default controller by default
#define DEFAULT_CONTROLLER_USER    ""                                       // Default controller user
#define DEFAULT_CONTROLLER_PASS    ""                                       // Default controller Password

// using a default template, you also need to set a DEFAULT PROTOCOL to a suitable MQTT protocol !
#define DEFAULT_PUB         "MFD/%sysname%/%tskname%/%valname%" // Enter your pub
#define DEFAULT_SUB         "MFD/%sysname%/#"                   // Enter your sub
#define DEFAULT_SERVER      "192.168.1.254"                     // Enter your Server IP address
#define DEFAULT_SERVER_HOST ""                                  // Server hostname
#define DEFAULT_SERVER_USEDNS false                             // true: Use hostname.  false: use IP

 // Enter your Server port value
#define DEFAULT_PORT        1883


#define DEFAULT_PROTOCOL    5                                   // Protocol used for controller communications
                                                                    //   0 = Stand-alone (no controller set)
                                                                    //   1 = Domoticz HTTP
                                                                    //   2 = Domoticz MQTT
                                                                    //   3 = Nodo Telnet
                                                                    //   4 = ThingSpeak
                                                                    //   5 = Home Assistant (openHAB) MQTT
                                                                    //   6 = PiDome MQTT
                                                                    //   7 = EmonCMS
                                                                    //   8 = Generic HTTP
                                                                    //   9 = FHEM HTTP


#define DEFAULT_MQTT_RETAIN                     false             // (true|false) Retain MQTT messages?
#define DEFAULT_MQTT_DELAY                      100               // Time in milliseconds to retain MQTT messages
#define DEFAULT_MQTT_LWT_TOPIC                  ""                // Default lwt topic
#define DEFAULT_MQTT_LWT_CONNECT_MESSAGE        "Connected"       // Default lwt message
#define DEFAULT_MQTT_LWT_DISCONNECT_MESSAGE     "Connection Lost" // Default lwt message
#define DEFAULT_MQTT_USE_UNITNAME_AS_CLIENTID   1
#define DEFAULT_DELETE_OLDEST                   true              //delete oldest message when queue is full

#define DEFAULT_USE_NTP                         true             // (true|false) Use NTP Server

#define DEFAULT_USE_RULES                       true             // (true|false) Enable Rules?
#define DEFAULT_RULES_OLDENGINE                 true

#define DEFAULT_SYSLOG_IP                       "192.168.100.5"                // Syslog IP Address
#define DEFAULT_SYSLOG_LEVEL                    LOG_LEVEL_INFO                 // Syslog Log Level

#ifdef ESP32
    #define DEFAULT_PIN_I2C_SDA                     21
    #define DEFAULT_PIN_I2C_SCL                     22
    #define DEFAULT_I2C_CLOCK_SPEED                 400000            // Use 100 kHz if working with old I2C chips
#else
    #define DEFAULT_PIN_I2C_SDA                     4
    #define DEFAULT_PIN_I2C_SCL                     5
    #define DEFAULT_I2C_CLOCK_SPEED                 400000            // Use 100 kHz if working with old I2C chips
#endif