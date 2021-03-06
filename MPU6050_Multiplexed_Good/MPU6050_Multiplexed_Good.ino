/*
 Name:		MPU6050_Multiplexed_Good.ino
 Created:	2/15/2016 12:55:57 AM
 Author:	Administrator



        Complete
    ======================
    Get the multiplexer to read identify the ports that the gyros are on
    Read the gyro data
*/




#include "Wire.h"

extern "C" { 
    #include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#include "MPU6050.h"
//#include "MPU6050\MPU6050.h"



#define TCAADDR 0x70
#define MPUADDR 0x68


MPU6050 mpu;

//=========================
// FOUND IMU SENSORS
//=========================
bool foundIMU[8] = {false};
unsigned long timer = 0;

void tcaselect(uint8_t i) {
    if (i > 7) 
        return;

    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << i);
    Wire.endTransmission();  
}

// standard Arduino setup()
void setup()
{
    while (!Serial);
    delay(1000);

    Wire.begin();
    Serial.println("\nTCAScanner ready!");
    Serial.begin(115200);
    Serial.println("\nTCAScanner ready!");
    //IdentifyIMU_Addresses();
   

    for (uint8_t t=0; t<8; t++) {
        tcaselect(t);
        Serial.print("TCA Port #"); Serial.println(t);

        for (uint8_t addr = 0; addr<=127; addr++) {
            if (addr == TCAADDR) continue;

            uint8_t data;
            if (! twi_writeTo(addr, &data, 0, 1, 1)) {
                Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
                
                //Initialize MPU
                InitMPU(t);
                foundIMU[t] = true;
            }
           // else
           // {
              //Set the IMU Address as disabled
           //   foundIMU[t]=false;
            
          //  }
        }
    }
    Serial.println("\ndone");



    
 

    // Check settings
   // checkSettings();
}



//Initialize The gyroscope on the multiplexer
void InitMPU(uint8_t gyroID)
{
    tcaselect(gyroID);


    Serial.print("Initialize MPU6050: ");
    Serial.println(gyroID);

    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
    {
        Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
        delay(500);
    }


    mpu.calibrateGyro();

    // Set threshold sensivty. Default 3.
    // If you don't want use threshold, comment this line or set 0.
    mpu.setThreshold(3);
}


void IdentifyIMU_Addresses()
{
    for (uint8_t t=0; t<8; t++) {
        tcaselect(t);
        Serial.print("TCA Port #"); Serial.println(t);

        for (uint8_t addr = 0; addr<=127; addr++) {
            if (addr == TCAADDR) continue;

            uint8_t data;
            if (! twi_writeTo(addr, &data, 0, 1, 1)) {
                Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
            }
        }
    }
    Serial.println("\ndone");
}



void checkSettings()
{
    Serial.println();

    Serial.print(" * Sleep Mode:        ");
    Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

    Serial.print(" * Clock Source:      ");
    switch(mpu.getClockSource())
    {
        case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
        case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
        case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
        case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
        case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
        case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
        case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
    }

    Serial.print(" * Gyroscope:         ");
    switch(mpu.getScale())
    {
        case MPU6050_SCALE_2000DPS:        Serial.println("2000 dps"); break;
        case MPU6050_SCALE_1000DPS:        Serial.println("1000 dps"); break;
        case MPU6050_SCALE_500DPS:         Serial.println("500 dps"); break;
        case MPU6050_SCALE_250DPS:         Serial.println("250 dps"); break;
    } 

    Serial.print(" * Gyroscope offsets: ");
    Serial.print(mpu.getGyroOffsetX());
    Serial.print(" / ");
    Serial.print(mpu.getGyroOffsetY());
    Serial.print(" / ");
    Serial.println(mpu.getGyroOffsetZ());

    Serial.println();
}

float timeStep = 0.01;
float pitch = 0;
float roll = 0;
float yaw = 0;



//Displays the gyro data per gyro
void DisplayGyroData(uint8_t gyroID)
{
    tcaselect(gyroID);

  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();

  // Calculate Pitch, Roll and Yaw
  pitch = pitch + norm.YAxis * timeStep;
  roll = roll + norm.XAxis * timeStep;
  yaw = yaw + norm.ZAxis * timeStep;

  // Output raw

  Serial.print(pitch);
  Serial.print(" ,");
  Serial.print(roll);  
  Serial.print(" ,");
  Serial.print(yaw);
  
  Serial.println(" ; ");

    /*
    Vector rawGyro = mpu.readRawGyro();
    Vector normGyro = mpu.readNormalizeGyro();

    
    Serial.print("Gyro");
    Serial.print(gyroID);
    Serial.print(":");
    
    Serial.print(rawGyro.XAxis);
    Serial.print(",");
    Serial.print(rawGyro.YAxis);
    Serial.print(",");
    Serial.println(rawGyro.ZAxis);
    */
    /*
    Serial.print(" Xnorm = ");
    Serial.print(normGyro.XAxis);
    Serial.print(" Ynorm = ");
    Serial.print(normGyro.YAxis);
    Serial.print(" Znorm = ");
    Serial.println(normGyro.ZAxis);*/
    // Wait to full timeStep period
  //delay((timeStep*1000) - (millis() - timer));
}

void loop() 
{

   // DisplayGyroData(2);
  //  DisplayGyroData(3);
    for (int i = 0; i < 8;  i++)
    {
     //   Serial.print(foundIMU[i]);
        if (foundIMU[i] == true)
        {
            DisplayGyroData(i);
    //        Serial.println("Update :" + i );
        }
    }

 // Serial.println("Update");

   // delay(100);
}
