//const int motorPins[] = {2, 3, 4, 5, 6, 7, 11, 12, 13};
const int motorPins[] = {54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83};

const int numPumps = sizeof(motorPins) / sizeof(motorPins[0]);

unsigned long pumpStartTimes[numPumps] = {0};

void setup() {
  for (int i = 0; i < numPumps; i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }

  Serial.begin(115200);
}

/*
void runPumps(int pumpNumber, int runTime) {
  if (pumpNumber >= 1 && pumpNumber <= numPumps) {
    // Start the pump
    digitalWrite(motorPins[pumpNumber - 1], HIGH);
    pumpStartTimes[pumpNumber - 1] = millis(); // Record the start time

    Serial.print("Running pump: ");
    Serial.println(pumpNumber);

    // Wait for the specified run time
    while (millis() - pumpStartTimes[pumpNumber - 1] < runTime) {
      // Check if it's time to stop the pump
      if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        processCommand(data);
      }
    }

    // Stop the pump
    digitalWrite(motorPins[pumpNumber - 1], LOW);
    Serial.print("Stopping pump: ");
    Serial.println(pumpNumber);
  } else {
    Serial.println("Invalid pump number");
  }
}
*/
/*
const int ledStripPin = 9; // Replace with the actual pin connected to your LED strip

void runPumps(int pumpNumber, int runTime) {
  if (pumpNumber >= 1 && pumpNumber <= numPumps) {
    // Start the pump
    digitalWrite(motorPins[pumpNumber - 1], HIGH);
    pumpStartTimes[pumpNumber - 1] = millis(); // Record the start time

    // Turn on the LED strip pattern when the first pump starts
    if (pumpNumber == 1) {
      digitalWrite(ledStripPin, HIGH);
      Serial.println("LED strip pattern ON");
    }

    Serial.print("Running pump: ");
    Serial.println(pumpNumber);

    // Wait for the specified run time
    while (millis() - pumpStartTimes[pumpNumber - 1] < runTime) {
      // Check if it's time to stop the pump
      if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        processCommand(data);
      }
    }

    // Stop the pump
    digitalWrite(motorPins[pumpNumber - 1], LOW);
    Serial.print("Stopping pump: ");
    Serial.println(pumpNumber);

    // Turn off the LED strip pattern when the last pump stops
    if (pumpNumber == numPumps) {
      digitalWrite(ledStripPin, LOW);
      Serial.println("LED strip pattern OFF");
    }
  } else {
    Serial.println("Invalid pump number");
  }
}




void runPumps(int pumpNumber, int runTime) {
  int pumpIndex = pumpNumber - 54; // Adjust for the range of pump numbers

  if (pumpIndex >= 0 && pumpIndex < numPumps) {
    // Start the pump
    digitalWrite(motorPins[pumpIndex], HIGH);
    pumpStartTimes[pumpIndex] = millis(); // Record the start time

    Serial.print("Running pump: ");
    Serial.println(pumpNumber);

    // Wait for the specified run time
    while (millis() - pumpStartTimes[pumpIndex] < runTime) {
      // Check if it's time to stop the pump
      if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        processCommand(data);
      }
    }

    // Stop the pump
    digitalWrite(motorPins[pumpIndex], LOW);
    Serial.print("Stopping pump: ");
    Serial.println(pumpNumber);
  } else {
    Serial.println("Invalid pump number");
  }
}


///////
void runPumps(int pumpNumber, int runTime) {
  int pumpIndex = pumpNumber - 54; // Adjust for the range of pump numbers

  if (pumpIndex >= 0 && pumpIndex < numPumps) {
    unsigned long earliestStartTime = millis(); // Initialize earliest start time

    // Find the earliest start time among the requested pumps
    for (int i = 0; i < numPumps; ++i) {
      unsigned long pumpStartTime = pumpStartTimes[i];
      if (pumpStartTime > 0 && pumpStartTime < earliestStartTime) {
        earliestStartTime = pumpStartTime;
      }
    }

    // Start all pumps at the earliest start time
    for (int i = 0; i < numPumps; ++i) {
      digitalWrite(motorPins[i], HIGH);
      pumpStartTimes[i] = earliestStartTime; // Record the common start time
    }

    Serial.print("Running pumps: ");
    Serial.println(pumpNumber);

    // Wait for the specified run time
    while (millis() - earliestStartTime < runTime) {
      // Check if it's time to stop the pumps
      if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        processCommand(data);
      }
    }

    // Stop the pump
    for (int i = 0; i < numPumps; ++i) {
      digitalWrite(motorPins[i], LOW);
    }
    
    Serial.print("Stopping pumps: ");
    Serial.println(pumpNumber);
  } else {
    Serial.println("Invalid pump number");
  }
}
/////
*/

void runPumps(int pumpNumber, int runTime) {
  int pumpIndex = pumpNumber - 54; // Adjust for the range of pump numbers

  if (pumpIndex >= 0 && pumpIndex < numPumps) {
    unsigned long pumpStartTime = millis(); // Record the start time for the requested pump
    pumpStartTimes[pumpIndex] = pumpStartTime;

    Serial.print("Running pump: ");
    Serial.println(pumpNumber);

    // Start the requested pump
    digitalWrite(motorPins[pumpIndex], HIGH);

    // Wait for the specified run time
    while (millis() - pumpStartTime < runTime) {
      // Check if it's time to stop the pump
      if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        processCommand(data);
      }
    }

    // Stop the requested pump
    digitalWrite(motorPins[pumpIndex], LOW);
    Serial.print("Stopping pump: ");
    Serial.println(pumpNumber);

    // Reset the start time for the stopped pump
    pumpStartTimes[pumpIndex] = 0;
  } else {
    Serial.println("Invalid pump number");
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

      //Serial.print("Motor Type: ");
      //Serial.println(motorType);
      Serial.print("SID: ");
      Serial.println(SID);

      
      // Relay control logic based on motorType
      if (motorType == "PUMPMOTOR_OPERATION") {
        //digitalWrite(driverOut1, HIGH);
        //iswashing = true;
      } else if (motorType == "WASHING_OPERATION") {
        //digitalWrite(driverOut2, HIGH);
        //iswashing = true;
      }


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

            if (motorType == "PUMPMOTOR_OPERATION") runPumps(param1, param2);
            else if (motorType == "SERVOMOTOR_OPERATION");// runServo(param1, param2);
            else if (motorType == "STEPPERMOTOR_OPERATION");// runStepper(param1, param2);
            else Serial.println("Unknown motor type");
          } else {
            Serial.println("Invalid pump data format");
          }

          index = endIndex + 1;
        } else {
          break;
        }
      }
    } else {
      Serial.println("Invalid motor type and SID format");
    }
  } else {
    Serial.println("Invalid command format");
  }
}


void loop() {
  // Main loop remains unchanged
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    processCommand(data);
  }
}

 //"PUMPMOTOR_OPERATION" from the string "(PUMPMOTOR_OPERATION 1647eba3-a6b0-42a7-8a08-ffef8ab07065),(1,3000),(2,4250),(3,3000),(4,4000)(5,4250),(6,3000),(7,4000)"
