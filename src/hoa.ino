#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define I2C_SDA 2
#define I2C_SCL 0
#define OE_PIN 6
#define TOUCH_PIN 4

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// PCA channels
#define LED0_CH 0
#define LED1_CH 1
#define SERVO2_CH 2
#define SERVO3_CH 3

// Servo pulse range
#define SERVOMIN 150
#define SERVOMAX 600

bool isOpen = false;

// ========= MAP SERVO =========
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

// ========= MOVE 2 SERVO + 2 LED =========
void moveAllParallel(int s2_from, int s2_to,
                     int s3_from, int s3_to,
                     int led0_from, int led0_to,
                     int led1_from, int led1_to,
                     int delayMs)
{
  int maxSteps = max(abs(s2_to - s2_from), abs(s3_to - s3_from));

  float s2_step = (float)(s2_to - s2_from) / maxSteps;
  float s3_step = (float)(s3_to - s3_from) / maxSteps;

  float l0_step = (float)(led0_to - led0_from) / maxSteps;
  float l1_step = (float)(led1_to - led1_from) / maxSteps;

  float s2 = s2_from;
  float s3 = s3_from;
  float l0 = led0_from;
  float l1 = led1_from;

  for (int i = 0; i <= maxSteps; i++) {

    // Servo 2
    pca.setPWM(SERVO2_CH, 0, angleToPulse((int)s2));

    // Servo 3
    pca.setPWM(SERVO3_CH, 0, angleToPulse((int)s3));

    // LED 0 & LED 1
    pca.setPWM(LED0_CH, 0, (int)l0);
    pca.setPWM(LED1_CH, 0, (int)l1);

    s2 += s2_step;
    s3 += s3_step;
    l0 += l0_step;
    l1 += l1_step;

    delay(delayMs);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);

  pinMode(TOUCH_PIN, INPUT);

  Wire.begin(I2C_SDA, I2C_SCL);
  pca.begin();
  pca.setPWMFreq(50);

  Serial.println("READY");
}

void loop() {

  if (digitalRead(TOUCH_PIN) == HIGH) {
    delay(60);
    if (digitalRead(TOUCH_PIN) == HIGH) {

      isOpen = !isOpen;
      delay(200);

      if (isOpen) {
        digitalWrite(OE_PIN, LOW);

        // ====== Open ======
        moveAllParallel(
          40, 80,        // Servo2
          155, 80,      // Servo3
          0, 3095,      // LED0
          0, 3095,      // LED1
          12            // speed
        );

      } else {

        // ====== Close ======
        moveAllParallel(
          80, 40,
          80, 155,
          3095, 0,
          3095, 0,
          12
        );

        digitalWrite(OE_PIN, HIGH);
      }
    }
  }
}
