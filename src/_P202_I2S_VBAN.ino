#ifdef USES_P202
#include "_Plugin_Helper.h"

  #ifdef ESP32 //only available for ESP32 due to the cpu power required and the limited ADC capabilities for ESP8266

    #include "_Plugin_Helper.h"
    #include "src/DataStructs/PinMode.h"
    #include <driver/adc.h>
    #include <WiFiUdp.h>

    #define PLUGIN_202
    #define PLUGIN_ID_202        202
    #define PLUGIN_NAME_202       "I2S VBAN Audio Transmitter [DEVELOPMENT]"
    #define PLUGIN_VALUENAME1_202 "Level"

    boolean Plugin_202_init = false;

    typedef struct tagVBAN_HEADER     
    {         
        char        vban[4];          /* contains 'V' 'B', 'A', 'N' */         
        uint8_t     format_SR;     /* SR index (see SRList above) */         
        uint8_t     format_nbs;    /* nb sample per frame (1 to 256) */         
        uint8_t     format_nbc;    /* nb channel (1 to 256) */         
        uint8_t     format_bit;    /* mask = 0x07 (nb Byte integer from 1 to 4) */         
        char        streamName[16];/* stream name */         
        uint32_t    nuFrame;       /* growing frame number. */     
    }  __attribute__((packed)) T_VBAN_HEADER;     
    
    #define VBAN_HEADER_SIZE (4 + 4 + 16 + 4) 

    /*
    #define VBAN_SR_MAXNUMBER 21  
    static long VBAN_SRList[VBAN_SR_MAXNUMBER]= {6000, 12000, 24000, 48000, 96000, 192000, 384000, 8000, 16000, 32000, 64000, 128000, 256000, 512000, 11025, 22050, 44100, 88200, 176400, 352800, 705600}; 
    */

    #define VBAN_PROTOCOL_AUDIO  0x00 
    #define VBAN_PROTOCOL_SERIAL  0x20 
    #define VBAN_PROTOCOL_TXT   0x40 
    #define VBAN_PROTOCOL_SERVICE  0x60 
    #define VBAN_PROTOCOL_UNDEFINED_1 0x80 
    #define VBAN_PROTOCOL_UNDEFINED_2 0xA0 
    #define VBAN_PROTOCOL_UNDEFINED_3 0xC0 
    #define VBAN_PROTOCOL_USER   0xE0 

    #define VBAN_DATATYPE_BYTE8   0x00 
    #define VBAN_DATATYPE_INT16   0x01 
    #define VBAN_DATATYPE_INT24   0x02 
    #define VBAN_DATATYPE_INT32   0x03 
    #define VBAN_DATATYPE_FLOAT32  0x04 
    #define VBAN_DATATYPE_FLOAT64  0x05 
    #define VBAN_DATATYPE_12BITS   0x06 
    #define VBAN_DATATYPE_10BITS   0x07 

    #define VBAN_CODEC_PCM   0x00 
    #define VBAN_CODEC_VBCA   0x10 //VB-AUDIO AOIP CODEC 
    #define VBAN_CODEC_VBCV   0x20 //VB-AUDIO VOIP CODEC  
    #define VBAN_CODEC_UNDEFINED_1  0x30 
    #define VBAN_CODEC_UNDEFINED_2  0x40 
    #define VBAN_CODEC_UNDEFINED_3  0x50 
    #define VBAN_CODEC_UNDEFINED_4  0x60 
    #define VBAN_CODEC_UNDEFINED_5  0x70 
    #define VBAN_CODEC_UNDEFINED_6  0x80 
    #define VBAN_CODEC_UNDEFINED_7  0x90 
    #define VBAN_CODEC_UNDEFINED_8  0xA0 
    #define VBAN_CODEC_UNDEFINED_9  0xB0 
    #define VBAN_CODEC_UNDEFINED_10  0xC0 
    #define VBAN_CODEC_UNDEFINED_11  0xD0 
    #define VBAN_CODEC_UNDEFINED_12  0xE0 
    #define VBAN_CODEC_USER   0xF0

    #define VBAN_PORT 6980 

    



    /*-----Internal variables and buffers ----- */
    uint8_t *buffer;  //a circular buffer for vban package with audio data  



    uint32_t frameCounter=0; //the incrementing frame counter for vban packages
    uint16_t sample_idx=VBAN_HEADER_SIZE; //index for audio aquisition; set to beginning of the first data portion


    uint16_t audio_power=0;


    TaskHandle_t audioHandlerTask; //task to send UDP packages 
    hw_timer_t * timer = NULL; //timer used to aquire audio data


    WiFiUDP p202UDP; //the UDP object
    IPAddress UDP_IP;// = IPAddress(192,168,100,62);
    uint16_t UDP_PORT = VBAN_PORT;


    #define SAMPLES 256 //number of samples per package
    #define CHANNELS 1 //number of channels
    #define SAMPLE_SIZE_BYTES 3

    #define AUDIO_SIZE (SAMPLES * CHANNELS * SAMPLE_SIZE_BYTES)
    #define VBAN_PACKET_SIZE  (VBAN_HEADER_SIZE + AUDIO_SIZE)
    #define BLOCKS 1 
    #define p202_BUFFER_SIZE (BLOCKS * VBAN_PACKET_SIZE)



#include <driver/i2s.h>

const i2s_port_t I2S_PORT = I2S_NUM_0;

// The I2S config as per the example
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
      .sample_rate = 16000,                         // 16KHz
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, //requires 32 pulses where only first 24bits are used (MSB --- LSB) could only get it to work with 32bits
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, 
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S), //data starts at second BCK
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,     // Interrupt level 1
      .dma_buf_count = 4,                           // number of buffers
      .dma_buf_len = 1024                     // samples per buffer
    
  };

  // The pin config as per the setup
  const i2s_pin_config_t pin_config = {
      .bck_io_num = 32,   // SCK
      .ws_io_num = 33,    // WS
      .data_out_num = -1, // not used (only for speakers)
      .data_in_num = 36   // SO
  };



    void P202_fillHeader(struct tagVBAN_HEADER * header){
      strcpy_P(header->vban, PSTR("VBAN")); 
      header->format_SR = VBAN_PROTOCOL_AUDIO | 8; // 9; //32KHz or use  8; //16Khz or use 7; //8Khz
      header->format_nbs = SAMPLES-1;  
      header->format_nbc = CHANNELS-1; //mono   
      header->format_bit = VBAN_CODEC_PCM | VBAN_DATATYPE_INT24;    /* mask = 0x07 (nb Byte integer from 1 to 4) */         
      strcpy_P(header->streamName , Settings.Name );/* stream name max 16 chars*/         
      header->nuFrame = 0;       /* growing frame number. */     
    }



    #define ismulticast(addr1) (((uint32_t)(addr1) & PP_HTONL(0xf0000000UL)) == PP_HTONL(0xe0000000UL))


    int P202_sendUDP(IPAddress toIP, uint16_t port, uint8_t * package, size_t length){
      if (WiFiConnected()) 
      {
        if (ismulticast(toIP))
        {
          if (p202UDP.beginMulticastPacket()) //(toIP, port, WiFi.localIP(),2))
          {
            p202UDP.write(package,length);
            p202UDP.endPacket();
          }
        }
        else
          if (p202UDP.beginPacket(toIP, port))
          {
            p202UDP.write(package,length);
            p202UDP.endPacket();
          }
        return 0; 
      }
      return -1;
    }


const int INT24_MAX = 8388607;
    uint32_t  buf[SAMPLES];
    void p202_audioHandler(void *param) {
      while (true) {
        if (WiFiConnected()) 
        {
            //filling the frame counter on the header before sending.
            uint8_t *header = (buffer);
            
            *(header+24) = (uint8_t)frameCounter; //LSB
            *(header+25) = (uint8_t)(frameCounter>>8);
            *(header+26) = (uint8_t)(frameCounter>>16);
            *(header+27) = (uint8_t)(frameCounter>>24); //MSB

            int num_bytes_read = i2s_read_bytes(I2S_PORT, &buf, (SAMPLES-1)*4,portMAX_DELAY);  
            
            uint8_t samplesRead = num_bytes_read / 4 ;
            uint8_t *ptr = (buffer+VBAN_HEADER_SIZE);
            for(int i=0;i<samplesRead;i++)
            {
                //uint8_t lsb = buf[i*4+0];
                //uint8_t mid = buf[i*4+1];
                //uint8_t msb = buf[i*4+2];
                //*(ptr++)=lsb;
                //*(ptr++)=mid;
                uint32_t b = buf[i];
                
                *(ptr++)=(uint8_t)(b>>8);
                *(ptr++)=(uint8_t)(b>>16);
                *(ptr++)=(uint8_t)(b>>24);
                
                //uint16_t d = map(b>>8,0,INT24_MAX,0,65535);  //amplify
                //*(ptr++)= (uint8_t)(d); //lsb
                //*(ptr++)= (uint8_t)(d>>8); //msb
                
                //*(ptr++)=(uint8_t)(b);

            }

            P202_sendUDP(UDP_IP, UDP_PORT, (buffer),VBAN_PACKET_SIZE);
            frameCounter++;
            
            yield();
          /*
          //processing the buffer further
          int samples_read = num_bytes_read / 8;
          if (samples_read > 0) {
            float mean = 0;
            for (int i = 0; i < samples_read; ++i) {
              mean += (*(buffer+VBAN_HEADER_SIZE+i) >> 14);
            }
            mean /= samples_read;

            float maxsample = -1e8, minsample = 1e8;
            for (int i = 0; i < samples_read; ++i) {
              minsample = min(minsample, samples[i] - mean);
              maxsample = max(maxsample, samples[i] - mean);
            }
            Serial.println(maxsample - minsample);
          }
          */
      }
      }
    }


    //A plugin has to implement the following function
    boolean Plugin_202(byte function, struct EventStruct *event, String& string)
    {
      //function: reason the plugin was called
      //event: ??add description here??
      //string: ??add description here??

      boolean success = false;

      switch (function)
      {
        case PLUGIN_DEVICE_ADD:
        {
            //This case defines the device characteristics, edit appropriately
            Device[++deviceCount].Number = PLUGIN_ID_202;
            Device[deviceCount].Type = DEVICE_TYPE_TRIPLE;  //2 data pins for I2S
            Device[deviceCount].VType = Sensor_VType::SENSOR_TYPE_SINGLE; //type of value the plugin will return, used only for Domoticz
            Device[deviceCount].Ports = 0;
            Device[deviceCount].PullUpOption = false;
            Device[deviceCount].InverseLogicOption = false;
            Device[deviceCount].FormulaOption = false;
            Device[deviceCount].ValueCount = 1;             //number of output variables. The value should match the number of keys PLUGIN_VALUENAME1_xxx
            Device[deviceCount].SendDataOption = true;
            Device[deviceCount].TimerOption = true;
            Device[deviceCount].TimerOptional = true;
            Device[deviceCount].GlobalSyncOption = true;
            Device[deviceCount].DecimalsOnly = true;
            break;
        }

        case PLUGIN_GET_DEVICENAME:
        {
          //return the device name
          string = F(PLUGIN_NAME_202);
          break;
        }

        case PLUGIN_GET_DEVICEVALUENAMES:
        {
          //called when the user opens the module configuration page
          //it allows to add a new row for each output variable of the plugin
          strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_202));
          break;
        }

        case PLUGIN_SET_DEFAULTS:
        {
          PCONFIG(0)=VBAN_PORT;
          PCONFIG(3) = 2048; //the zero position
          CONFIG_PIN1 = 36; 
          Settings.TaskDevicePin2[event->TaskIndex] = 33;
          Settings.TaskDevicePin3[event->TaskIndex] = 32;
        }

        case PLUGIN_WEBFORM_LOAD:
        {
          //this case defines what should be displayed on the web form, when this plugin is selected
          //The user's selection will be stored in
          //PCONFIG(x) (custom configuration)

          // Make sure not to append data to the string variable in this PLUGIN_WEBFORM_LOAD call.
          // This has changed, so now use the appropriate functions to write directly to the Streaming
          // web_server. This takes much less memory and is faster.
          // There will be an error in the web interface if something is added to the "string" variable.

          //Use any of the following (defined at web_server.ino):
          //To add some html, which cannot be done in the existing functions, add it in the following way:
          //addHtml(F("<TR><TD>Analog Pin:<TD>"));


          //For strings, always use the F() macro, which stores the string in flash, not in memory.

    
          //number selection (min-value - max-value)
          addFormNumericBox(F("VBAN Port"), F("p202_VBAN_PORT"), PCONFIG(0), 1, 65535);
          
          IPAddress addr(PCONFIG_LONG(0));
          byte ip[4];
          ip[0] = addr[0]; ip[1]=addr[1]; ip[2]=addr[2]; ip[3]=addr[3];
          addFormIPBox(F("Target IP"), F("p202_IP"),ip );
          //after the form has been loaded, set success and break
          
          success = true;
          break;
        }

        case PLUGIN_WEBFORM_SAVE:
        {
          //this case defines the code to be executed when the form is submitted
          //the plugin settings should be saved to PCONFIG(x)
          //ping configuration should be read from CONFIG_PIN1 and stored
          
          PCONFIG(0) = getFormItemInt(F("p202_VBAN_PORT"));
        
          IPAddress ip;
          ip.fromString(web_server.arg(F("p202_IP")).c_str());
          PCONFIG_LONG(0) = ip;

          //after the form has been saved successfuly, set success and break
          success = true;
          break;

        }
        case PLUGIN_INIT:
        {
          //this case defines code to be executed when the plugin is initialised
          UDP_IP = IPAddress(PCONFIG_LONG(0));
          UDP_PORT = PCONFIG(0);
          
          if (ismulticast(UDP_IP))
          {
            addLog(LOG_LEVEL_INFO, F("IP address is Multicast"));
            p202UDP.beginMulticast(UDP_IP,UDP_PORT);
          }
          

          buffer = (uint8_t*)calloc(p202_BUFFER_SIZE,sizeof(uint8_t));
          if (buffer == NULL) 
          {
            addLog(LOG_LEVEL_ERROR,F("VBAN: Buffer Allocation error"));
          }
          else
          {
            int idx = 0;
            while (idx<BLOCKS){
              P202_fillHeader((T_VBAN_HEADER *)(buffer+idx*VBAN_PACKET_SIZE));
              idx++;
            }
          }

          String log = F("VBAN: sending to: ");
          log+=UDP_IP.toString();
          log+=F(":");
          log+=UDP_PORT;
          log+=F(" stream=");
          log+=Settings.Name ;
          addLog(LOG_LEVEL_INFO,log);
          

           // Configuring the I2S driver and pins.
          // This function must be called before any I2S driver read/write operations.
          int8_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
          if (err != ESP_OK) {
            Serial.printf("I2S: Failed installing driver: %d\n", err);
          }
          err = i2s_set_pin(I2S_PORT, &pin_config);
          if (err != ESP_OK) {
            Serial.printf("I2S: Failed setting pins: %d\n", err);
          }
          //Serial.println("I2S driver installed.");
          

          //TODO: only start the task if enabled.
          xTaskCreate(p202_audioHandler, "Handler Task", 5000, NULL, 1, &audioHandlerTask);

          //after the plugin has been initialised successfuly, set success and break
          log = F("VBAN: Initialized");
          addLog(LOG_LEVEL_INFO,log);
          success = true;
          break;

        }

        case PLUGIN_READ:
        {
          //code to be executed to read data
          //It is executed according to the delay configured on the device configuration page, only once

          //after the plugin has read data successfuly, set success and break
          success = true;
          break;

        }

        case PLUGIN_WRITE:
        {
          //this case defines code to be executed when the plugin executes an action (command).
          //Commands can be accessed via rules or via http.
          //As an example, http://192.168.1.12//control?cmd=dothis
          //implies that there exists the comamnd "dothis"

          //if (plugin_not_initialised)
          // break;

          //implement any commands here

          break;
        }

        case PLUGIN_EXIT:
        {
          //perform cleanup tasks here. For example, free memory
          free(buffer);
          buffer= NULL;
          break;

        }
          
        case PLUGIN_ONCE_A_SECOND:
        {
          //code to be executed once a second. Tasks which do not require fast response can be added here
          UserVar[event->BaseVarIndex] = audio_power;
          success = true;
          
          break;
        }
      
        /*
        case PLUGIN_TEN_PER_SECOND:
        {
          //code to be executed 10 times per second. Tasks which require fast response can be added here
          //be careful on what is added here. Heavy processing will result in slowing the module down!
          
          success = true;
          break;
        
        
        }

        case PLUGIN_FIFTY_PER_SECOND:
        {

          success = true;
          break;
        }
        */

      }   // switch
      return success;

    }     //main plugin function


  #endif //ESP32
#endif 