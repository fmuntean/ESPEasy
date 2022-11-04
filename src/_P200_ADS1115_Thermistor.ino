#ifdef USES_P200
#include "_Plugin_Helper.h"
// #######################################################################################################
// ######################## Plugin 200: ADS1115 I2C 0x48)  ###############################################
// #######################################################################################################

/*
Schematic:

 VCC - R - AIN0 - Th1 - GND
 VCC - R - AIN1 - Th2 - GND
           AIN2 - GND
 VCC - R - AIN3 - R   - GND

 R=10K

 Readings:
  Th1 => AIN0 - AIN3
  Th2 => AIN1 - AIN3



*/


#include <cmath>


#define PLUGIN_200
#define PLUGIN_ID_200 200
#define PLUGIN_NAME_200 "ADS1115 Temperature Sensor"
#define PLUGIN_VALUENAME1_200 "Analog"
#define PLUGIN_VALUENAME2_200 "Temperature"

#define ADC_MAX   32767
#define ADC_MIN  -32768

#define ADC_AIN0 4
#define ADC_AIN1 5
#define ADC_AIN2 6
#define ADC_AIN3 7



//boolean Plugin_200_init = false;



boolean Plugin_200(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  // static byte portValue = 0;
  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
    {
      Device[++deviceCount].Number           = PLUGIN_ID_200;
      Device[deviceCount].Type               = DEVICE_TYPE_I2C;
      Device[deviceCount].VType              = Sensor_VType::SENSOR_TYPE_TRIPLE;
      Device[deviceCount].Ports              = 0;
      Device[deviceCount].PullUpOption       = false;
      Device[deviceCount].InverseLogicOption = false;
      Device[deviceCount].FormulaOption      = true;
      Device[deviceCount].ValueCount         = 2; //the third and forth one are used internally for moving average
      Device[deviceCount].SendDataOption     = true;
      Device[deviceCount].TimerOption        = true;
      Device[deviceCount].GlobalSyncOption   = true;
      break;
    }

    case PLUGIN_GET_DEVICENAME:
    {
      string = F(PLUGIN_NAME_200);
      break;
    }

    case PLUGIN_GET_DEVICEVALUENAMES:
    {
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_200));
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_200));
      break;
    }

    case PLUGIN_WEBFORM_LOAD:
    {
      #define ADS1115_I2C_OPTION 4
      byte addr                            = PCONFIG(0);
      int optionValues[ADS1115_I2C_OPTION] = { 0x48, 0x49, 0x4A, 0x4B };
      addFormSelectorI2C(F("p200_i2c"), ADS1115_I2C_OPTION, optionValues, addr);

      addFormSubHeader(F("Input"));

        #define ADS1115_PGA_OPTION 6
      byte pga                              = PCONFIG(1);
      String pgaOptions[ADS1115_PGA_OPTION] = {
        F("2/3x gain (FS=6.144V)"),
        F("1x gain (FS=4.096V)"),
        F("2x gain (FS=2.048V)"),
        F("4x gain (FS=1.024V)"),
        F("8x gain (FS=0.512V)"),
        F("16x gain (FS=0.256V)")
      };
      addFormSelector(F("Gain"), F("p200_gain"), ADS1115_PGA_OPTION, pgaOptions, NULL, pga);

      #define ADS1115_MUX_OPTION 8
      byte mux = PCONFIG(2);
      String muxOptions[ADS1115_MUX_OPTION] = {
        F("AIN0 - AIN1 (Differential)"),
        F("AIN0 - AIN3 (Differential)"),
        F("AIN1 - AIN3 (Differential)"),
        F("AIN2 - AIN3 (Differential)"),
        F("AIN0 - GND (Single-Ended)"),
        F("AIN1 - GND (Single-Ended)"),
        F("AIN2 - GND (Single-Ended)"),
        F("AIN3 - GND (Single-Ended)"),
      };
      addFormSelector(F("Input Multiplexer"), F("p200_mode"), ADS1115_MUX_OPTION, muxOptions, NULL, mux);

      addFormSubHeader(F("Temperature parameters"));
      addFormNumericBox(F("Beta"), F("p200_beta"), PCONFIG_LONG(0), 0, 32767);

      addFormTextBox(F("A"), F("p200_A"), String(PCONFIG_FLOAT(0), 10), 20);
      addFormTextBox(F("B"), F("p200_B"), String(PCONFIG_FLOAT(1), 10), 20);
      addFormTextBox(F("C"), F("p200_C"), String(PCONFIG_FLOAT(2), 10), 20);

      addFormNumericBox(F("Moving Average Size"), F("p200_filter"), PCONFIG_LONG(1), 1, 32767);
      success = true;
      break;
    }

    case PLUGIN_WEBFORM_SAVE:
    {
      PCONFIG(0) = getFormItemInt(F("p200_i2c"));

      PCONFIG(1) = getFormItemInt(F("p200_gain"));
      PCONFIG(3) = getFormItemInt(F("p200_gain"));

      PCONFIG(2) = getFormItemInt(F("p200_mode"));

      // PCONFIG(3) = isFormItemChecked(F("p200_cal"));

      PCONFIG_LONG(0) = getFormItemInt(F("p200_beta"));
      PCONFIG_LONG(1) = getFormItemInt(F("p200_filter"));

      PCONFIG_FLOAT(0) = getFormItemFloat(F("p200_A"));
      PCONFIG_FLOAT(1) = getFormItemFloat(F("p200_B"));
      PCONFIG_FLOAT(2) = getFormItemFloat(F("p200_C"));
      
      //Plugin_200_init = false; // Force device setup next time
      success         = true;
      break;
    }

    case PLUGIN_INIT:
    { 
      uint8_t address = PCONFIG(0);
      uint16_t pga   = PCONFIG(3);
      uint16_t mux = PCONFIG(2);

      //we do the first reading to prime the numbers so we do not get the NaN at the begining.
      p200_prepareRead(address, pga,mux);
      delay(8);
      uint16_t filter_power = (uint16_t)PCONFIG_LONG(1);
      UserVar[event->BaseVarIndex+2] = (float)p200_readRegister((address), (0x00)); // read conversion register
      UserVar[event->BaseVarIndex+3] = UserVar[event->BaseVarIndex+2]* filter_power;
      uint32_t Vref = 6600 << pga;        // calculate the 3v3 /2  adc value
     UserVar[event->BaseVarIndex+2] = UserVar[event->BaseVarIndex+2] / (float)Vref; // compensate for the 2^pga gain
      //Plugin_200_init = true;
      success         = true;
      break;
    }

    case PLUGIN_ONCE_A_SECOND:
    {
        //we keep the raw average value in UserVar[event->BaseVarIndex+2] and the MA[i] in UserVar[event->BaseVarIndex+3]

      //code to be executed once a second. Tasks which do not require fast response can be added here
      uint8_t address = PCONFIG(0);

      uint16_t pga   = PCONFIG(3);
      int16_t  raw = ADC_MAX;

      while (raw >= ADC_MAX || raw <= ADC_MIN)
      {
        uint16_t mux = PCONFIG(2);
        p200_prepareRead(address, pga,mux);
        delay(8);
        
        raw = p200_readRegister((address), (0x00)); // read conversion register
        

        if ((raw >= ADC_MAX) || (raw <= ADC_MIN)) {
          pga--;
        }

        
      }

      //#define  filter_power (64) 
      uint16_t filter_power = (uint16_t)PCONFIG_LONG(1);
      float avg;
      int16_t lastPGA= PCONFIG(3);
      if (lastPGA == pga)
      {
        //Calculate Moving average where 2^filter_power is the moving window of points: https://www.daycounter.com/LabBook/Moving-Average.phtml
        //MA[i]= MA[i-1] +X[i] - MA[i-1]/N
        // where MA is the moving average*N.
        //avg[i]= MA[i]/N
        avg = UserVar[event->BaseVarIndex+3] / (float)filter_power;
        UserVar[event->BaseVarIndex+3] = UserVar[event->BaseVarIndex+3] + (float)raw -  avg;
      } else
      {
        PCONFIG(3) = pga;
        //uint8_t filter_power=(1<<4); //2^4
        avg = (float)raw;
        UserVar[event->BaseVarIndex+3] = (float)raw * filter_power;
        
      }
      

      //String _log = F("ADS1115 : adc:");
      //_log += raw;_log += F(" pga:"); _log += pga; 
      //_log += F(" avg:"); _log+=avg;
      //addLog(LOG_LEVEL_DEBUG, _log);

      // wheastone bridge using R=R1=R2=R3=10K R25=10K
      // Vp-Vn where the thermistor is on the Vn side towards the ground Vp=Vdd/2=3v3/2=1v65
      uint32_t Vref = 6600 << pga;        // calculate the 3v3 /2  adc value


      /*
      p200_prepareRead(address,pga, ADC_AIN2); //AIN2 is wired to ground in order to read the offset
      delay(8);
      int16_t offset = p200_readRegister(address,0x00 );
      */
      UserVar[event->BaseVarIndex+2] = avg / (float)Vref; // compensate for the 2^pga gain
  

      success = true;

      break;
    }
    case PLUGIN_READ:
    {
      
      String _log = F("ADS1115 : ");

      //_log+=F(" offset:"); _log+=offset;
      //_log += F(" Vref:"); _log += Vref;
      float dv = UserVar[event->BaseVarIndex+2] ; //contains the raw moving average value
      _log += F(" dv:"); _log += dv;


      // float Rt = (1-dv/Vref)/(1+dv/Vref); 
      //removed the R (R=R0) from the equation Rt = R * (Vdd+Vt)/(Vdd-Vt) 
      //where Vt =Vp - Vn (differential voltage read)
      double ln = log((1 + dv) / (1 - dv));

      _log += F(" ln:"); _log += ln;

      float beta = PCONFIG_LONG(0);

      // float temperature = 1/( log(Rt) / beta + 1/298.15) - 273.15; //in celsius 
      //removed the R25 inside the log(..) as was simplified above
      
      //float temperature = 298.15 / (1 + (ln * 298.15) / beta) - 273.15;
      #define T0 298.15
      double temperature = (beta * T0)/ (beta + T0*ln)- 273.15;

      UserVar[event->BaseVarIndex] = temperature; //temperature based on beta value

      // Rt =  R*(1 + 2*voltage / vdd )/(1- 2*voltage / vdd)
      // ln = log(Rt/R) = log ( (1+2*voltage/vdd)/(1-2*voltage/vdd) )
      // Tc = 1 / ( (ln / beta) + (1 / 298.15) ) - 273.15;
      addLog(LOG_LEVEL_DEBUG, _log);


      //double Tc = 1 / (ln / beta + 1 / 298.15) - 273.15;

      //UserVar[event->BaseVarIndex + 2] = Tc;

      double lnR = ln+9.21034037198; //adding the ln(R0)

     //https://thecavepearlproject.org/2017/04/26/calibrating-oversampled-thermistors-with-an-arduino/
     //parameters obtained from: https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=2ahUKEwj70M625fnlAhUxqlkKHbTnCEwQFjAAegQIARAC&url=https%3A%2F%2Fedwardmallon.files.wordpress.com%2F2017%2F04%2Fntc-steinhart_and_hart_calculator.xls&usg=AOvVaw3qmQIDzgNcWww0a9uqrbwE
     double Tc1 = 1/ (PCONFIG_FLOAT(0)+ lnR*(PCONFIG_FLOAT(1) + PCONFIG_FLOAT(2)*lnR*lnR) ) - 273.15;

      UserVar[event->BaseVarIndex + 1] = Tc1; //temperature based on steinhart formula



      success = true;
      break;
    }
  }
  return success;
}



void p200_prepareRead(uint8_t address, int16_t pga, uint16_t mux)
{
  uint16_t config = (0x0003)    | // Disable the comparator (default val)
                    (0x0000)    | // Non-latching (default val)
                    (0x0000)    | // Alert/Rdy active low   (default val)
                    (0x0000)    | // Traditional comparator (default val)
                    (0x0080)    | // 128 samples per second (default)
                    (0x0100);     // Single-shot mode (default)

  config |= pga << 9;

  config |= mux << 12;

  config |= (0x8000);          // Start a single conversion

  Wire.beginTransmission(address);
  Wire.write((uint8_t)(0x01)); // address pointer register to select config register
  Wire.write((uint8_t)(config >> 8));
  Wire.write((uint8_t)(config & 0xFF));
  Wire.endTransmission();
}

uint16_t p200_readRegister(uint8_t i2cAddress, uint8_t reg) {
  Wire.beginTransmission(i2cAddress);
  Wire.write((0x00));
  Wire.endTransmission();

  if (Wire.requestFrom(i2cAddress, (uint8_t)2) != 2) {
    return 0x8000;
  }
  return (Wire.read() << 8) | Wire.read();
}

#endif // USES_P025
