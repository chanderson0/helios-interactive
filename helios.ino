// Helios helios helios

// State machines
enum InteractionState {
  InteractionNone,
  InteractionStarted,
  InteractionFinished
};
InteractionState interactionState;
bool switchActive;

// Timing
unsigned long touchStartTime;
unsigned long touchEndTime;
unsigned long interactionStartTime;

// Touch constants
static const int kTouchThreshold = 950;
static const int kTouchOnTimeThreshold = 250;
static const int kTouchOffTimeThreshold = 250;

// Interaction times
static const int kInteractionHoldTime = 10000;

// Pin IDs
static const int kPinInteractionStarted = 11;
static const int kPinInteractionFinished = 12;

void setup() {
  Serial.begin(9600);

  interactionState = InteractionNone;
  switchActive = false;

  touchStartTime = 0;
  touchEndTime = 0;
  interactionStartTime = 0;

  pinMode(kPinInteractionStarted, OUTPUT);
  pinMode(kPinInteractionFinished, OUTPUT);

  Serial.println("Started!");
}

void loop() {
  const int reading = analogRead(A0);
  Serial.println(reading);
  
  const bool switchTouched = reading < kTouchThreshold;
  const unsigned long now = millis();
  
  if (switchTouched && touchStartTime == 0) {
    touchStartTime = now;
    touchEndTime = 0;
  } else if (!switchTouched && touchEndTime == 0) {
    touchStartTime = 0;
    touchEndTime = now;
  }

  // Debounce the switch
  if (!switchActive && touchStartTime > 0 && now - touchStartTime > kTouchOnTimeThreshold) {
    Serial.println("Switch Active");
    switchActive = true;
  } else if (switchActive && touchEndTime > 0 && now - touchEndTime > kTouchOffTimeThreshold) {
    Serial.println("Switch Inactive");
    switchActive = false; 
  }
  
  // State machine time!
  if (interactionState == InteractionNone) {
    // Transition to Started once you hold on
    if (switchActive) {
      Serial.println("None -> Started");
      interactionState = InteractionStarted;
      interactionStartTime = now;
      digitalWrite(kPinInteractionStarted, HIGH);
      digitalWrite(kPinInteractionFinished, LOW);
    }
  } else if (interactionState == InteractionStarted) {
    // Transition to None if you let go
    if (!switchActive) {
      Serial.println("Started -> None");
      interactionState = InteractionNone;
      interactionStartTime = 0;
      
      digitalWrite(kPinInteractionStarted, LOW);
      digitalWrite(kPinInteractionFinished, LOW);
    }
    
    // Transition to Finished if you hold on long enough
    if (interactionStartTime > 0 && now - interactionStartTime > kInteractionHoldTime) {
      Serial.println("Started -> Finished");
      interactionState = InteractionFinished;
      
      digitalWrite(kPinInteractionStarted, HIGH);
      digitalWrite(kPinInteractionFinished, HIGH);
    }
  } else if (interactionState == InteractionFinished) {
    // Transition to None if you let go
    if (!switchActive) {
      Serial.println("Finished -> None");
      interactionState = InteractionNone;
      interactionStartTime = 0;
      
      digitalWrite(kPinInteractionStarted, LOW);
      digitalWrite(kPinInteractionFinished, LOW);
    }
  }
  
  delay(5);
}

