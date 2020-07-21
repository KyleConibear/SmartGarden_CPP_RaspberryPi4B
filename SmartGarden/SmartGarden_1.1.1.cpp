#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <ADCDevice.hpp>

ADCDevice *adc; // Define an Analog to Digital Converter (ADC)

#define soilMoisturePin 0 // Input: Soil Monisture Sensor
#define soilDry 226 // The ADC reading of the Soil Moisture Sensor when dry
#define soilWet 95 // The ADC reading of the Soil Moisture Sensor when wet

#define waterPumpPin 1 // Output: Control water pump
#define enablePumpThreshold 180 // The ADC reading threshold to turn on the pump

#define wetSoilThreshold 120 // Below this amount the soil is too wet

int main()
{   
    adc = new ADCDevice();
    
    printf("Smart Garden starting...");
    
    if(adc->detectI2C(0x4b))
    {
    delete adc;
    adc = new ADS7830();
    }
    else
    {
        printf("I2C address fail.\nPlease use command 'i2cdetect -y 1' to check the I2C address.\nProgram Exit.\n");
        return -1;
    }
    
    wiringPiSetup();
    
    pinMode(waterPumpPin, OUTPUT);
    
     while(1)
    {
        int soilMoistureValue = adc->analogRead(soilMoisturePin);   
        
        int soilMoisturePercent = (float)((soilMoistureValue - soilDry) / (float)(soilDry-soilWet)) * 100.0f;
        // Convert to positive number
        soilMoisturePercent -= soilMoisturePercent * 2;
        
        printf("Soil Moisture ADC: %i.\n", soilMoistureValue);
        printf("Soil Moisture %%: %i.\n", soilMoisturePercent);
        
        if(soilMoistureValue >= enablePumpThreshold)
        {
            printf("Soil is dry, pump is running.\n\n");
            digitalWrite(waterPumpPin, HIGH); // Turn on pump
        } 
        else if(soilMoistureValue < enablePumpThreshold && soilMoistureValue > wetSoilThreshold)
        {
            printf("Soil is damp, pump is off.\n\n");
            digitalWrite(waterPumpPin, LOW); // Turn on pump
        }
        else
        {
            printf("Warning: Soil is too wet.\nMold growth may occur\n\n"); 
            digitalWrite(waterPumpPin, LOW); // Turn on pump        
        }   
        
        delay(100);
    }
    
    return 0;
}
