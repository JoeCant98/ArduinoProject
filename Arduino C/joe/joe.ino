#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <TimeLib.h>

int snoozeActive = false;
int secondsNextAlertSnooze = 0;

int alarmHours = 0;
int alarmMinutes = 0;
int alarmSeconds = 0;

int currentSelection = 0;
bool timeIsSet = false;
bool alarmSet = false;
bool alarmActive = false;

#define LCD_OFF 0x0
#define LCD_WHITE 0x7

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
uint8_t buttons;

void setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(LCD_WHITE);
}

void loop() {
  if (!timeIsSet) {
    setTime();
  }

  showTimes();
  checkAlert();

  buttons = lcd.readButtons();

  if (buttons) {
    if (buttons & BUTTON_SELECT) {
      delay(100);
      displayMenu();
    }
  }

}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Menu");

  currentSelection = 0;

  while(true) {

    lcd.setCursor(0,1);
    buttons = lcd.readButtons();

    switch(currentSelection) {

      case 0:
        lcd.print("[Set Time]      ");

        if (buttons && (buttons & BUTTON_SELECT)) {
          delay(100);
          setTime();
          delay(100);
          displayMenu();
          return;
        }
        break;

      case 1:
        lcd.print("[Set Alarm]     ");

        if (buttons && (buttons & BUTTON_SELECT)) {
          delay(100);
          setAlarm();
          delay(100);
          displayMenu();
          return;
        }
        break;

      case 2:
        lcd.print("[Toggle Alarm]  ");

        if (buttons && (buttons & BUTTON_SELECT)) {
          delay(100);
          toggleAlarm();
          delay(100);
          displayMenu();
          return;
        }
        break;

      case 3:
        lcd.print("[Leave Menu]    ");

        if (buttons && (buttons & BUTTON_SELECT)) {
          delay(100);
          lcd.clear();
          return;
        }
        break;
    }

    if (buttons) {
      if (buttons & BUTTON_LEFT) {
        currentSelection--;
        if (currentSelection < 0) {
          currentSelection = 3;
        }
        delay(100);
      }
      else if (buttons & BUTTON_RIGHT) {
        currentSelection++;
        if (currentSelection > 3) {
          currentSelection = 0;
        }
        delay(100);
      }
    }
  }
}

void showTimes() {
  lcd.setCursor(0,0);

  lcd.print(twoDigits(hour()) + ":" + twoDigits(minute()) + ":" + twoDigits(second()));

  if (snoozeActive) {
    lcd.print(" SNOOZED");
  }

  lcd.setCursor(0,1);

  if (alarmSet) {

    lcd.print(twoDigits(alarmHours) + ":" + twoDigits(alarmMinutes) + ":" + twoDigits(alarmSeconds));

    if (alarmActive) {
      lcd.print(" ACTIVE");
    }

  } else {
    lcd.print("Alarm not set");
  }

}

void checkAlert() {

  if (alarmActive) {
    if (snoozeActive) {

      int alertSeconds = 3600 * alarmHours + 60 * alarmMinutes + alarmSeconds + secondsNextAlertSnooze;
      int currentSeconds = 3600 * hour() + 60 * minute() + second();

      if (alertSeconds == currentSeconds) {
        showAlert();
        return;
      }
    } else {
      if (alarmHours == hour() && alarmMinutes == minute() && alarmSeconds == second()) {
        showAlert();
        return;
      }
    }
  }
}

void setTime() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set Time");

  currentSelection = 0;

  while(true) {
    lcd.setCursor(0,1);

    if (currentSelection == 0) {
      lcd.print("[" + twoDigits(hour()) + "]");
    } else {
      lcd.print(twoDigits(hour()));
    }

    lcd.print(":");

    if (currentSelection == 1) {
      lcd.print("[" + twoDigits(minute()) + "]");
    } else {
      lcd.print(twoDigits(minute()));
    }

    lcd.print(":");

    if (currentSelection == 2) {
      lcd.print("[" + twoDigits(second()) + "]");
    } else {
      lcd.print(twoDigits(second()));
    }

    buttons = lcd.readButtons();

    if (buttons) {
      delay(100);
      if (buttons & BUTTON_UP) {
        switch(currentSelection) {
          case 0:
            adjustTime(3600);
            break;
          case 1:
            adjustTime(60);
            break;
          case 2:
            adjustTime(1);
            break;
        }
      }
      else if (buttons & BUTTON_DOWN) {
        switch(currentSelection) {
          case 0:
            adjustTime(-3600);
            break;
          case 1:
            adjustTime(-60);
            break;
          case 2:
            adjustTime(-1);
            break;
        }
      }
      else if (buttons & BUTTON_LEFT) {
        currentSelection--;
        if (currentSelection < 0) {
          currentSelection = 2;
        }
      }
      else if (buttons & BUTTON_RIGHT) {
        currentSelection++;
        if (currentSelection > 2) {
          currentSelection = 0;
        }
      }
      else if (buttons & BUTTON_SELECT) {
        timeIsSet = true;
        lcd.clear();
        return;
      }
    }

  }
}

void showAlert() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("!! ALARM !!");

  currentSelection = 0;

  while(true) {
    lcd.setCursor(0,1);

    if (currentSelection == 0) {
      lcd.print("[DISMISS] ");
    } else {
      lcd.print("DISMISS ");
    }

    if (currentSelection == 1) {
      lcd.print("[SNOOZE]");
    } else {
      lcd.print("SNOOZE");
    }

    buttons = lcd.readButtons();

    if (buttons) {
      delay(100);

      if (buttons & BUTTON_LEFT) {
        currentSelection--;
        if (currentSelection < 0) {
          currentSelection = 1;
        }
      }
      else if (buttons & BUTTON_RIGHT) {
        currentSelection++;
        if (currentSelection > 1) {
          currentSelection = 0;
        }
      }
      else if (buttons & BUTTON_SELECT) {
        switch(currentSelection) {
          case 0:
            alarmActive = false;
            secondsNextAlertSnooze = 0;
            snoozeActive = false;
            lcd.clear();
            return;

          case 1:
            secondsNextAlertSnooze += 30;
            snoozeActive = true;
            lcd.clear();
            return;
        }
      }
    }
  }
}

void setAlarm() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set Alarm");

  currentSelection = 0;

  while(true) {
    lcd.setCursor(0,1);

    if (currentSelection == 0) {
      lcd.print("[" + twoDigits(alarmHours) + "]");
    } else {
      lcd.print(twoDigits(alarmHours));
    }

    lcd.print(":");

    if (currentSelection == 1) {
      lcd.print("[" + twoDigits(alarmMinutes) + "]");
    } else {
      lcd.print(twoDigits(alarmMinutes));
    }

    lcd.print(":");

    if (currentSelection == 2) {
      lcd.print("[" + twoDigits(alarmSeconds) + "]");
    } else {
      lcd.print(twoDigits(alarmSeconds));
    }

    buttons = lcd.readButtons();

    if (buttons) {
      delay(100);

      if (alarmHours < 0) {
        alarmHours = 23;
      } else if (alarmHours > 23) {
        alarmHours = 0;
      }

      if (alarmMinutes < 0) {
        alarmMinutes = 59;
      } else if (alarmMinutes > 59) {
        alarmMinutes = 0;
      }

      if (alarmSeconds < 0) {
        alarmSeconds = 59;
      } else if (alarmSeconds > 59) {
        alarmSeconds = 0;
      }

      if (buttons & BUTTON_UP) {
        switch(currentSelection) {
          case 0:
            alarmHours++;
            break;
          case 1:
            alarmMinutes++;
            break;
          case 2:
            alarmSeconds++;
            break;
        }
      }
      else if (buttons & BUTTON_DOWN) {
        switch(currentSelection) {
          case 0:
            alarmHours--;
            break;
          case 1:
            alarmMinutes--;
            break;
          case 2:
            alarmSeconds--;
            break;
        }
      }
      else if (buttons & BUTTON_LEFT) {
        currentSelection--;
        if (currentSelection < 0) {
          currentSelection = 2;
        }
      }
      else if (buttons & BUTTON_RIGHT) {
        currentSelection++;
        if (currentSelection > 2) {
          currentSelection = 0;
        }
      }
      else if (buttons & BUTTON_SELECT) {
        alarmSet = true;
        lcd.clear();
        return;
      }

    }
  }
}

void toggleAlarm() {
  lcd.clear();
  lcd.setCursor(0,0);

  if (!alarmSet) {
    lcd.print("You must");
    lcd.setCursor(0,1);
    lcd.print("set the alarm");
    delay(1000);
    return;
  }

  alarmActive = !alarmActive;

  if (alarmActive) {
    lcd.print("Alarm Is");
    lcd.setCursor(0,1);
    lcd.print("Now Active");
  } else {
    lcd.print("Alarm Is");
    lcd.setCursor(0,1);
    lcd.print("Now Inactive");
  }

  delay(1000);
  lcd.clear();

}

String twoDigits(int val) {
  if (val < 10) {
    return "0" + String(val);
  }
  return String(val);
}
