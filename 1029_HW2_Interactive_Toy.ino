const int buttonPin = 2;
const int RledPin = 9;
const int GledPin = 10;
const int BledPin = 11;
const int ldrPin = A0;

int mood = 0;
const int neutralMood = 10;

int buttonState = 0;
bool ButtonPressed = false;
bool decayStarted = false;

unsigned long touchedTimer = 0;
unsigned long reducedTimer = 0;

const unsigned long unTouchInterval = 5000;
unsigned long reducedInterval = 1000;

void setup() {
  Serial.begin(9600); 

  pinMode(buttonPin, INPUT);
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);

  mood = neutralMood;

  touchedTimer = millis();
  reducedTimer = millis();
}

void loop() {
  // 1. Display the current mood
  showLEDState(mood);

  // 2. Read the photoresistor
  int lightLevel = analogRead(ldrPin);

  
  Serial.print("Light Level: ");
  Serial.print(lightLevel);
  Serial.print("\t Mood Score: ");
  Serial.println(mood);

  // Dark environment: faster mood decay
  if (lightLevel < 400) {
    reducedInterval = 500;
  } else {
    reducedInterval = 1000;
  }

  
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && !ButtonPressed) {
    mood++;

    if (mood > 20) {
      mood = 20;
    }

    touchedTimer = millis();
    reducedTimer = millis();
    decayStarted = false;
    ButtonPressed = true;
  }

  // Detect button release
  if (buttonState == LOW && ButtonPressed) {
    ButtonPressed = false;
  }

  // 4. Handle mood decay
  unsigned long currentTimer = millis();

  if (currentTimer - touchedTimer >= unTouchInterval) {

    // Start the decay interval after 5 seconds
    if (!decayStarted) {
      reducedTimer = currentTimer;
      decayStarted = true;
    }

    
    if (currentTimer - reducedTimer >= reducedInterval) {
      if (mood > 0) {
        mood--;
      }

      reducedTimer = currentTimer;
    }
  }

  delay(200); 
}

void showLEDState(int state) {
  float brightnessInterval = 255.0 / 10.0;

  if (state >= neutralMood) {
    // Mood 10 to 20: Green to Blue
    analogWrite(RledPin, 255);

    analogWrite(
      GledPin,
      (int)(brightnessInterval * (state - neutralMood))
    );

    analogWrite(
      BledPin,
      (int)(255 - brightnessInterval * (state - neutralMood))
    );

  } else {
    // Mood 0 to 10: Red to Green
    analogWrite(
      RledPin,
      (int)(255 - brightnessInterval * (neutralMood - state))
    );

    analogWrite(
      GledPin,
      (int)(brightnessInterval * (neutralMood - state))
    );

    analogWrite(BledPin, 255);
  }
}