

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
#define DEFAULT_CONTROLLER_DELETE_OLDEST        true              //delete oldest message when queue is full

#define DEFAULT_USE_NTP                         true             // (true|false) Use NTP Server

#define DEFAULT_USE_RULES                       true             // (true|false) Enable Rules?
#define DEFAULT_RULES_OLDENGINE                 true

#define DEFAULT_SYSLOG_IP                       "192.168.100.5"                // Syslog IP Address
#define DEFAULT_SYSLOG_LEVEL                    LOG_LEVEL_INFO                 // Syslog Log Level
#ifdef ESP32S2
        #define DEFAULT_PIN_I2C_SDA                     4
        #define DEFAULT_PIN_I2C_SCL                     5
        #define DEFAULT_I2C_CLOCK_SPEED                 400000            // Use 100 kHz if working with old I2C chips
#else
    #ifdef ESP32
        #define DEFAULT_PIN_I2C_SDA                     21
        #define DEFAULT_PIN_I2C_SCL                     22
        #define DEFAULT_I2C_CLOCK_SPEED                 400000            // Use 100 kHz if working with old I2C chips
    #else
        #define DEFAULT_PIN_I2C_SDA                     4
        #define DEFAULT_PIN_I2C_SCL                     5
        #define DEFAULT_I2C_CLOCK_SPEED                 400000            // Use 100 kHz if working with old I2C chips
    #endif
#endif

#ifdef BUILD_GIT
	#undef BUILD_GIT
#endif

#define BUILD_GIT "MFD-mega-2022_11_02"

//###########################################################################################
//##                         MFD BOARDS                                                    ##
//###########################################################################################

#ifdef ENV_MFD_1M_OTA
    //#define CONTROLLER_SET_NONE //Should remove all controller related UI
    #define NOTIFIER_SET_NONE   //Should remove any notifier UI
    //#define DEVICE_SET_NONE    //Should remove any device UI
    
    #define LIMIT_BUILD_SIZE
    
    #define FEATURE_ESPEASY_P2P 0 //disable ESPEASY P2P protocol
    #define USES_MQTT             //enable MQTT protocol

    #define USE_I2C_DEVICE_SCAN     false   // turn feature off in OTA builds

    #define BUILD_NO_DEBUG
    #define BUILD_NO_RAM_TRACKER
    #define BUILD_NO_DIAGNOSTIC_COMMANDS
    #define BUILD_NO_SPECIAL_CHARACTERS_STRINGCONVERTER
    #define WEBSERVER_CUSTOM_BUILD_DEFINED
    //    #define WEBSERVER_SYSVARS
    //    #define WEBSERVER_I2C_SCANNER
    //    #define WEBSERVER_FAVICON
    //    #define WEBSERVER_CSS
    //    #define WEBSERVER_INCLUDE_JS
    //    #define WEBSERVER_LOG
        #define WEBSERVER_ROOT
        #define WEBSERVER_ADVANCED
        #define WEBSERVER_CONFIG
    //    #define WEBSERVER_CONTROL
        #define WEBSERVER_CONTROLLERS
        #define WEBSERVER_DEVICES
    //    #define WEBSERVER_FACTORY_RESET
    //    #define WEBSERVER_FILELIST
    //    #define WEBSERVER_HARDWARE
        #define WEBSERVER_RULES
    //    #define WEBSERVER_SETUP
    //    #define WEBSERVER_SYSINFO
    //    #define WEBSERVER_TOOLS

    #define SHOW_SYSINFO_JSON  //enable sysinfo_json 
    #define VTABLES_IN_IRAM   //store VTABLES in ram

    # define USES_C005 // OpenHAB MQTT

    # define USES_P001 // Switch

    #if FEATURE_ADC_VCC==false
        # define USES_P002 // ADC
    #endif

    # define USES_P026 // SysInfo
    # define USES_P033 // Dummy
    # define USES_P248 // AHT10 Humidity and Temperature sensor

#endif


//###########################################################################################
//##                         MFD PLUGIN SETS                                               ##
//###########################################################################################

#ifdef MFD_PLUGIN_SET_NONE
    # define PLUGIN_SET_NONE
    # define NOTIFIER_SET_NONE
    # define CONTROLLER_SET_NONE
    #ifdef USES_SERVO
        # undef USES_SERVO
    #endif
    # define BUILD_MINIMAL_OTA
    #ifndef BUILD_NO_DEBUG
        # define BUILD_NO_DEBUG
    #endif
    # define LIMIT_BUILD_SIZE
    #define BUILD_NO_SPECIAL_CHARACTERS_STRINGCONVERTER
    #define BUILD_NO_DIAGNOSTIC_COMMANDS
    #define BUILD_NO_RAM_TRACKER

    //#ifdef USES_SSDP
    //    #undef USES_SSDP
    //#endif

    #define WEBSERVER_CUSTOM_BUILD_DEFINED
        #ifndef WEBSERVER_SYSVARS
            #define WEBSERVER_SYSVARS
        #endif
        #ifndef WEBSERVER_CSS
           // #define WEBSERVER_CSS
        #endif
        #ifndef WEBSERVER_INCLUDE_JS
           // #define WEBSERVER_INCLUDE_JS
        #endif
        #ifndef WEBSERVER_LOG
            #define WEBSERVER_LOG
        #endif
        #ifndef WEBSERVER_GITHUB_COPY
            //#define WEBSERVER_GITHUB_COPY
        #endif
        #ifndef WEBSERVER_ROOT
            #define WEBSERVER_ROOT
        #endif
        #ifndef WEBSERVER_ADVANCED
            #define WEBSERVER_ADVANCED
        #endif
        #ifndef WEBSERVER_CONFIG
            #define WEBSERVER_CONFIG
        #endif
        
        #ifndef WEBSERVER_CONTROLLERS
            #define WEBSERVER_CONTROLLERS
        #endif
        #ifndef WEBSERVER_DEVICES
            #define WEBSERVER_DEVICES
        #endif
        #ifndef WEBSERVER_HARDWARE
            #define WEBSERVER_HARDWARE
        #endif
        #ifndef WEBSERVER_RULES
            #define WEBSERVER_RULES
        #endif
        #ifndef WEBSERVER_SETUP
            #define WEBSERVER_SETUP
        #endif
        #ifndef WEBSERVER_SYSINFO
            #define WEBSERVER_SYSINFO
        #endif
        
        #ifndef WEBSERVER_TOOLS
            #define WEBSERVER_TOOLS
        #endif
        #ifndef WEBSERVER_UPLOAD
            //#define WEBSERVER_UPLOAD
        #endif


    #ifdef FEATURE_I2CMULTIPLEXER
        #undef FEATURE_I2CMULTIPLEXER
    #endif

    #define MAIN_PAGE_SHOW_NODE_LIST_BUILD   false
    #define MAIN_PAGE_SHOW_NODE_LIST_TYPE    false

    #define USES_MQTT
#endif

// MFD: plugins for MX3166
#ifdef MFD_PLUGIN_SET_MX3166
    # define  PLUGIN_SET_NONE
    # define  CONTROLLER_SET_NONE
    # define  NOTIFIER_SET_NONE
#endif // ifdef MFD_PLUGIN_SET_MX3166


// MFD my minimal plugins for use with 1M devices like ESP07
#ifdef MFD_PLUGIN_SET_MINIMAL
    #ifdef BUILD_MINIMAL_OTA
       #undef BUILD_MINIMAL_OTA
    #endif
    #ifndef PLUGIN_DESCR
      #define PLUGIN_DESCR  "Minimal 1M OTA"
    #endif

    #ifndef BUILD_NO_DEBUG
      #define BUILD_NO_DEBUG
    #endif

    

    #ifdef USE_SETTINGS_ARCHIVE
        #undef USE_SETTINGS_ARCHIVE
    #endif // USE_SETTINGS_ARCHIVE


    #ifdef USES_TIMING_STATS
        #undef USES_TIMING_STATS
    #endif
    
    #ifdef PLUGIN_SET_STABLE
        #undef PLUGIN_SET_STABLE
    #endif

    # define CONTROLLER_SET_NONE

    # ifdef USES_C008 
        #  undef USES_C008 // Generic HTTP
    # endif 
    # ifdef USES_C013
        #  undef USES_C013 // ESPEasy P2P network
    # endif 

    # define USES_C005 // OpenHAB MQTT

    # define PLUGIN_SET_NONE

    # define USES_P001 // Switch

    #if FEATURE_ADC_VCC==false
        # define USES_P002 // ADC
    #else
        # undef USES_P002
    #endif

    # define USES_P026 // SysInfo
    # define USES_P033 // Dummy
    # define USES_P248 //AHT10 Humidity and Temperature sensor
    
    #ifndef NOTIFIER_SET_NONE
        #define  NOTIFIER_SET_NONE
    #endif

    #undef USES_N001
    #undef USES_N002 

#endif // ifdef MFD_PLUGIN_SET_MINIMAL

// This selects only the plugins I use in my prod boards
#ifdef MFD_PLUGIN_SET_PROD
    # define PLUGIN_BUILD_CUSTOM
    # define PLUGIN_SET_NONE

    # define USES_P001 // Switch
    # define USES_P002 // ADC
    # define USES_P003 // Pulse
    # define USES_P014 // SI7021
    # define USES_P017 // PN532 RFID reader
    # define USES_P025 // ADS1115
    # define USES_P026 // SysInfo
    # define USES_P028 // BME280
    # define USES_P033 // Dummy
    # define USES_P069 // LM75A Temperature Sensor

    #define USES_P097   // Touch (ESP32)

    # define USES_P205 //RGBW Light
//    # define USES_P123 // SI7013 Temperature Humidity and ADC (using P014 as I added support ot that plugin)
// # define USES_P130 // RC522 RFID reader
    # define USES_P200 // ADS1115 Thermistor

    # define USES_P248 //AHT10 Humidity and Temperature sensor

    # define CONTROLLER_SET_NONE
    # define USES_C005 // OpenHAB MQTT

    #ifdef NOTIFIER_SET_STABLE
        #undef NOTIFIER_SET_STABLE
    #endif 

//    # define NOTIFIER_SET_NONE //MFD: setting this removes the notification page
    # define USES_N002  // Buzzer
    #define FEATURE_RTTTL 1  // Enable RTTTL 
    

#endif      // ifdef MFD_PLUGIN_SET_PROD


