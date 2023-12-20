const int motorPins[] = {2, 3, 4, 5, 6, 7, 11, 12, 13};
const int numPumps = sizeof(motorPins) / sizeof(motorPins[0]);

unsigned long pumpStartTimes[numPumps] = {0};

void runPumps(int pumpNumber, int runTime) {
  if (pumpNumber >= 1 && pumpNumber <= numPumps) {
    digitalWrite(motorPins[pumpNumber - 1], HIGH);
    pumpStartTimes[pumpNumber - 1] = millis() + runTime;
    Serial.print("Running pump: ");
    Serial.println(pumpNumber);
  } else {
    Serial.println("Invalid pump number");
  }
}

void stopPump(int pumpNumber) {
  if (pumpNumber >= 1 && pumpNumber <= numPumps) {
    digitalWrite(motorPins[pumpNumber - 1], LOW);
    Serial.print("Stopping pump: ");
    Serial.println(pumpNumber);
  } else {
    Serial.println("Invalid pump number");
  }
}

void setup() {
  for (int i = 0; i < numPumps; i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }

  Serial.begin(115200);
}

void loop() {
  // Check and deactivate pumps if the run time has elapsed
  for (int i = 0; i < numPumps; i++) {
    if (millis() >= pumpStartTimes[i] && digitalRead(motorPins[i]) == HIGH) {
      stopPump(i + 1);
    }
  }

  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    processCommand(data);
  }
}

void processCommand(String command) {
  char separator = ',';

  int firstBracketIndex = command.indexOf('(');
  int secondBracketIndex = command.indexOf(')', firstBracketIndex + 1);

  if (firstBracketIndex != -1 && secondBracketIndex != -1) {
    // Extract motor type and SID from the first set of brackets
    String motorTypeAndSID = command.substring(firstBracketIndex + 1, secondBracketIndex);
    int spaceIndex = motorTypeAndSID.indexOf(' ');

    if (spaceIndex != -1) {
      String motorType = motorTypeAndSID.substring(0, spaceIndex);
      String SID = motorTypeAndSID.substring(spaceIndex + 1);

      Serial.print("SID: ");
      Serial.println(SID);

      // Relay control logic based on motorType
      if (motorType == "PUMPMOTOR_OPERATION") {
        // Extract pump number/angle and run time from subsequent brackets
        int index = secondBracketIndex + 1;
        while (index < command.length()) {
          int nextBracketIndex = command.indexOf('(', index);
          int endIndex = command.indexOf(')', nextBracketIndex + 1);

          if (nextBracketIndex != -1 && endIndex != -1) {
            String inputData = command.substring(nextBracketIndex + 1, endIndex);

            int commaIndex = inputData.indexOf(separator);
            if (commaIndex != -1) {
              int param1 = inputData.substring(0, commaIndex).toInt();
              int param2 = inputData.substring(commaIndex + 1).toInt();

              Serial.print(motorType);
              Serial.print(": Param1: ");
              Serial.print(param1);
              Serial.print(", Param2: ");
              Serial.println(param2);

              runPumps(param1, param2);
            } else {
              Serial.println("Invalid pump data format");
            }

            index = endIndex + 1;
          } else {
            break;
          }
        }
      } else if (motorType == "WASHING_OPERATION") {
        // Handle other motor types if needed
      } else if (motorType == "SERVOMOTOR_OPERATION") {
        // Handle servo motor if needed
      } else if (motorType == "STEPPERMOTOR_OPERATION") {
        // Handle stepper motor if needed
      } else {
        Serial.println("Unknown motor type");
      }
    } else {
      Serial.println("Invalid motor type and SID format");
    }
  } else {
    Serial.println("Invalid command format");
  }
}

 //"PUMPMOTOR_OPERATION" from the string "(PUMPMOTOR_OPERATION 1647eba3-a6b0-42a7-8a08-ffef8ab07065),(1,3000),(2,4250),(3,3000),(4,4000)(5,4250),(6,3000),(7,4000)"
