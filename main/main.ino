#include <Servo.h>
#include<Wire.h>

//User settings
const int down = 180;
const int up = 90;
const int MPU_addr = 0x68;  // I2C address of the MPU-6050
const int16_t AcZdif_threshold = 1000;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int16_t AcZrest;
long timeout;

Servo myservo;

void setup() {
  //Setup servo
  myservo.attach(9);
  pinMode(LED_BUILTIN, OUTPUT);

  //Setup accelerometer
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  //Init initial value
  digitalWrite(LED_BUILTIN, HIGH);
  float AcZsum = 0.0;
  for (int i = 0; i < 100; i++) {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    AcZsum += float(AcZ);
    delay(50);
  }
  AcZrest = int16_t(AcZsum / 100.0);

  //Finish indicator
  for(int i = 0; i<3; i++){
    digitalWrite(LED_BUILTIN, LOW); 
    delay(333);
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(333);
  }
  digitalWrite(LED_BUILTIN, LOW); 
  timeout = 0;
}

void loop() {
  //Get IMU values
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.println(AcZ); Serial.print(" ");
  timeout += 50;
  delay(50);

  //If got PS4 controller vibrates
  if (abs(AcZ - AcZrest) > AcZdif_threshold || timeout > 60000)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    myservo.write(down);
    delay(1000);
    myservo.write(up);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    timeout = 0;
  }
}
