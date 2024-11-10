//
#include <Preferences.h>
Preferences preferences;

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// KEYPAD
#include <Keypad.h>
const byte ROWS = 4; 
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {19, 18, 5, 17}; // pin 1 to 4
byte colPins[COLS] = {16, 4, 2, 15}; // pin 5 to 8 may maliit na # sa keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// INPUT 
int inPinDoor1 = 32;
int inPinDoor2 = 33;
int inPinDoor3 = 25;
int inPinDoor4 = 26;
int inPinMasterReset = 23;

// RELAY
int outPinDoor1 = 27;
int outPinDoor2 = 14;
int outPinDoor3 = 12;
int outPinDoor4 = 13;

// Vars
int displayMode = 0;            // 0 - idle / home
                                // 1 - enter user password
                                // 2 - enter admin password
                                // 3 - enter admin set door password
                                // 4 - enter admin password 2
                                // 5 - enter admin set new password
                                // 6 - enter admin password reset
        
                                // 10 - user error try again
                                // 11 - admin error try again
                                // 12 - admin set door password error try again
                                // 13 - admin set new password error try again

                                // 20 - user access ok
                                // 21 - admin access ok
                                // 22 - admin set door password access ok
                                // 23 - admin new password access ok
                                // 24 - admin reset password access ok
int selectedDoor = 0;
String passDoor1 = "";
String passDoor2 = "";
String passDoor3 = "";
String passDoor4 = "";
String passAdmin = "";
String currentPassword = "";

// START
// ====================================
void setup() 
{
  //
  Serial.begin(9600);

  //
  preferences.begin("my-app", false);
  passDoor1 = preferences.getString("passDoor1", "");
  passDoor2 = preferences.getString("passDoor2", "");
  passDoor3 = preferences.getString("passDoor3", "");
  passDoor4 = preferences.getString("passDoor4", "");
  passAdmin = preferences.getString("passAdmin", "1234");

  // LCD
  lcd.init();
  lcd.clear();         
  lcd.backlight();      

  //
  pinMode(inPinDoor1, INPUT);
  pinMode(inPinDoor2, INPUT);
  pinMode(inPinDoor3, INPUT);
  pinMode(inPinDoor4, INPUT); 
  pinMode(inPinMasterReset, INPUT_PULLUP); 

  //
  pinMode(outPinDoor1, OUTPUT); digitalWrite(outPinDoor1, LOW);
  pinMode(outPinDoor2, OUTPUT); digitalWrite(outPinDoor2, LOW);
  pinMode(outPinDoor3, OUTPUT); digitalWrite(outPinDoor3, LOW);
  pinMode(outPinDoor4, OUTPUT); digitalWrite(outPinDoor4, LOW);

  // 
  lcd.setCursor(0,0);   
  lcd.print("Starting...");
  lcd.setCursor(0,1);
  lcd.print("Please Wait");

  //
  delay(10000);
  displayMode = 0;
  displayAll();
  Serial.println("START");
}


// LOOP
// ====================================
void loop() 
{
  //
  char key = keypad.getKey();// Read the key
  
  // Print if key pressed
  if (key)
  {
    Serial.print("Key Pressed : ");
    Serial.println(key);

    // KEY
    { 
      // keys
      {
        //
        if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9')
        {
          if (displayMode == 1 || displayMode == 2 || displayMode == 3 || displayMode == 4 || displayMode == 5 || displayMode == 6)
          {
              int pCount = currentPassword.length();
              if (pCount < 10)
              {
                currentPassword = currentPassword + key;
                displayAll();
                delay(100);
              }
          }
        }

        // cancel
        if (key == '#')
        {
          if (displayMode == 1 || displayMode == 2 || displayMode == 3 || displayMode == 4 || displayMode == 5 || displayMode == 6)
          {
            // remove password 
            if (currentPassword.length() > 0) 
            {
                currentPassword.remove(currentPassword.length() - 1);
                displayAll();
                delay(100);
            }
            // back
            else
            {
              displayMode = 0;
              displayAll();
            }
          }
        }

        // ok
        if (key == '*')
        {
          if (currentPassword.length() > 0) 
          {
            // user pass?
            if (displayMode == 1)
            {
              if (selectedDoor == 1 && currentPassword == passDoor1)
              {
                digitalWrite(outPinDoor1, HIGH);
                displayMode = 20;
                displayAll();
                delay(5000);
                digitalWrite(outPinDoor1, LOW);
                displayMode = 0;
                displayAll();
                return;
              }
              else if (selectedDoor == 2 && currentPassword == passDoor2)
              {
                digitalWrite(outPinDoor2, HIGH);
                displayMode = 20;
                displayAll();
                delay(5000);
                digitalWrite(outPinDoor2, LOW);
                displayMode = 0;
                displayAll();
                return;
              }
              else if (selectedDoor == 3 && currentPassword == passDoor3)
              {
                digitalWrite(outPinDoor3, HIGH);
                displayMode = 20;
                displayAll();
                delay(5000);
                digitalWrite(outPinDoor3, LOW);
                displayMode = 0;
                displayAll();
                return;
              }
              else if (selectedDoor == 4 && currentPassword == passDoor4)
              {
                digitalWrite(outPinDoor4, HIGH);
                displayMode = 20;
                displayAll();
                delay(5000);
                digitalWrite(outPinDoor4, LOW);
                displayMode = 0;
                displayAll();
                return;
              }
              // error
              else
              {
                displayMode = 10;
                displayAll();
                delay(2000);
                displayMode = 1;
                displayAll();
                return;
              }
            }

            // admin pass?
            if (displayMode == 2)
            {
              if (currentPassword == passAdmin)
              {
                displayMode = 3;
                currentPassword = "";
                displayAll();
                delay(100);
              }
              // error
              else
              {
                displayMode = 11;
                displayAll();
                delay(2000);
                displayMode = 2;
                displayAll();
              }
            }

            // admin set door pass?
            if (displayMode == 3)
            {
              if (currentPassword.length() > 0)
              {
                //
                if (selectedDoor == 1)
                {
                  preferences.putString("passDoor1", currentPassword);
                  passDoor1 = currentPassword;
                }
                if (selectedDoor == 2)
                {
                  preferences.putString("passDoor2", currentPassword);
                  passDoor2 = currentPassword;
                }
                if (selectedDoor == 3)
                {
                  preferences.putString("passDoor3", currentPassword);
                  passDoor3 = currentPassword;
                }
                if (selectedDoor == 4)
                {
                  preferences.putString("passDoor4", currentPassword);
                  passDoor4 = currentPassword;
                }
                
                // return
                displayMode = 22;
                displayAll();
                delay(2000);
                displayMode = 0;
                displayAll();
                return;
              }
              // error
              else
              {
                
              }
            }

            // admin pass 2?
            if (displayMode == 4)
            {
              if (currentPassword == passAdmin)
              {
                displayMode = 5;
                currentPassword = "";
                displayAll();
                delay(100);
              }
              // error
              else
              {
                displayMode = 11;
                displayAll();
                delay(2000);
                displayMode = 4;
                displayAll();
              }
            }

             // admin set new pass?
            if (displayMode == 5)
            {
              if (currentPassword.length() > 0)
              {
                //
                preferences.putString("passAdmin", currentPassword);
                passAdmin = currentPassword;
                
                // return
                displayMode = 23;
                displayAll();
                delay(2000);
                displayMode = 0;
                displayAll();
                return;
              }
              // error
              else
              {
                
              }
            }

            // admin pass reset?
            if (displayMode == 6)
            {
              if (currentPassword == passAdmin)
              {
                //
                preferences.putString("passDoor1", "");
                passDoor1 = "";
                preferences.putString("passDoor2", "");
                passDoor2 = "";
                preferences.putString("passDoor3", "");
                passDoor3 = "";
                preferences.putString("passDoor4", "");
                passDoor4 = "";
                
                //
                preferences.putString("passAdmin", "1234");
                passAdmin = "1234";
                
                // return
                displayMode = 24;
                displayAll();
                delay(2000);
                displayMode = 0;
                displayAll();
                return;
              }
              // error
              else
              {
                displayMode = 11;
                displayAll();
                delay(2000);
                displayMode = 6;
                displayAll();
              }
            }
          }
          else
          {
            
          }

          //
          if (displayMode == 0)
          {
            displayMode = 4;
            currentPassword = "";
            displayAll();
            delay(100);
          }
        }
      
      }

      // display idle?
      if (displayMode == 0)
      {
        if (key == 'A')
        {
          displayMode = 2;
          selectedDoor = 1;
          currentPassword = "";
          displayAll();
        }
        if (key == 'B')
        {
          displayMode = 2;
          selectedDoor = 2;
          currentPassword = "";
          displayAll();
        }
        if (key == 'C')
        {
          displayMode = 2;
          selectedDoor = 3;
          currentPassword = "";
          displayAll();
        }
        if (key == 'D')
        {
          displayMode = 2;
          selectedDoor = 4;
          currentPassword = "";
          displayAll();
        }
      }
    }
  }

  //
  int inPinDoor1Var = digitalRead(inPinDoor1);
  int inPinDoor2Var = digitalRead(inPinDoor2);
  int inPinDoor3Var = digitalRead(inPinDoor3);
  int inPinDoor4Var = digitalRead(inPinDoor4);

  //
  {
    if (inPinDoor1Var)
    {
      if (displayMode == 0)
      {
        Serial.println("hayop1");
        displayMode = 1;
        selectedDoor = 1;
        currentPassword = "";
        displayAll();
      }
    }
    if (inPinDoor2Var)
    {
      if (displayMode == 0)
      {
        Serial.println("hayop2");
        displayMode = 1;
        selectedDoor = 2;
        currentPassword = "";
        displayAll();
      }
    }
    if (inPinDoor3Var)
    {
      if (displayMode == 0)
      {
        Serial.println("hayop3");
        displayMode = 1;
        selectedDoor = 3;
        currentPassword = "";
        displayAll();
      }
    }
    if (inPinDoor4Var)
    {
      if (displayMode == 0)
      {
        Serial.println("hayop4");
        displayMode = 1;
        selectedDoor = 4;
        currentPassword = "";
        displayAll();
      }
    }
  }

  //
  int inPinMasterResetVar = digitalRead(inPinMasterReset);
  if (!inPinMasterResetVar)
  {
    if (displayMode == 0)
    {
      displayMode = 6;
      currentPassword = "";
      displayAll();
    }
  }
}


// FUNCTION
// ====================================
void displayAll()
{
  displayIdle();
  displayUserPassword();
  displayAdminPassword();
  displayAdminSetDoorPassword();
  displayAdminPassword2();
  displayAdminPasswordNew();
  displayAdminPasswordReset();
  displayErrorUserPassword();
  displayErrorAdminPassword();
  displayOkUser();
  displayOkAdminSetDoor();
  displayOkAdminSetNew();
  displayOkAdminReset();
}

void displayIdle()
{
  //
  if (displayMode != 0)
  {
    return;  
  }
  
  //
  lcd.setCursor(0,0);   
  lcd.print("    SCAN  QR    ");
  lcd.setCursor(0,1);  
  lcd.print("   TO  ACCESS   ");
}

void displayUserPassword()
{
  //
  if (displayMode != 1)
  {
    return;  
  }

  //
  int pCount = currentPassword.length();
  String pCountDisplay = "";
  for (int x = 1; x <= 16; x++)
  {
    if (x <= pCount)
    {
      pCountDisplay = pCountDisplay + "*";
    }
    else
    {
      pCountDisplay = pCountDisplay + " ";
    }
  }

  //
  lcd.setCursor(0,0);  
  lcd.print("Enter Passkey[" + String(selectedDoor) + "]");
  lcd.setCursor(0,1);   
  lcd.print(pCountDisplay);
}

void displayAdminPassword()
{
  //
  if (displayMode != 2)
  {
    return;  
  }

  //
  int pCount = currentPassword.length();
  String pCountDisplay = "";
  for (int x = 1; x <= 16; x++)
  {
    if (x <= pCount)
    {
      pCountDisplay = pCountDisplay + "*";
    }
    else
    {
      pCountDisplay = pCountDisplay + " ";
    }
  }

  //
  lcd.setCursor(0,0);  
  lcd.print("Admin Passkey   ");
  lcd.setCursor(0,1);   
  lcd.print(pCountDisplay);
}

void displayAdminSetDoorPassword()
{
  //
  if (displayMode != 3)
  {
    return;  
  }

  //
  int pCount = currentPassword.length();
  String pCountDisplay = "";
  for (int x = 1; x <= 16; x++)
  {
    if (x <= pCount)
    {
      pCountDisplay = pCountDisplay + "*";
    }
    else
    {
      pCountDisplay = pCountDisplay + " ";
    }
  }

  //
  lcd.setCursor(0,0);  
  lcd.print("New   Passkey[" + String(selectedDoor) + "]");
  lcd.setCursor(0,1);   
  lcd.print(pCountDisplay);
}

void displayAdminPassword2()
{
  //
  if (displayMode != 4)
  {
    return;  
  }

  //
  int pCount = currentPassword.length();
  String pCountDisplay = "";
  for (int x = 1; x <= 16; x++)
  {
    if (x <= pCount)
    {
      pCountDisplay = pCountDisplay + "*";
    }
    else
    {
      pCountDisplay = pCountDisplay + " ";
    }
  }

  //
  lcd.setCursor(0,0);  
  lcd.print("Admin Passkey   ");
  lcd.setCursor(0,1);   
  lcd.print(pCountDisplay);
}

void displayAdminPasswordNew()
{
  //
  if (displayMode != 5)
  {
    return;  
  }

  //
  int pCount = currentPassword.length();
  String pCountDisplay = "";
  for (int x = 1; x <= 16; x++)
  {
    if (x <= pCount)
    {
      pCountDisplay = pCountDisplay + "*";
    }
    else
    {
      pCountDisplay = pCountDisplay + " ";
    }
  }

  //
  lcd.setCursor(0,0);  
  lcd.print("New Admin Pass  ");
  lcd.setCursor(0,1);   
  lcd.print(pCountDisplay);
}

void displayAdminPasswordReset()
{
  //
  if (displayMode != 6)
  {
    return;  
  }

  //
  int pCount = currentPassword.length();
  String pCountDisplay = "";
  for (int x = 1; x <= 16; x++)
  {
    if (x <= pCount)
    {
      pCountDisplay = pCountDisplay + "*";
    }
    else
    {
      pCountDisplay = pCountDisplay + " ";
    }
  }

  //
  lcd.setCursor(0,0);  
  lcd.print("Admin Passkey   ");
  lcd.setCursor(0,1);   
  lcd.print(pCountDisplay);
}

void displayErrorUserPassword()
{
  //
  if (displayMode != 10)
  {
    return;  
  }

  //
//lcd.print("                ");
  lcd.setCursor(0,0);  
  lcd.print("    Invalid     ");
  lcd.setCursor(0,1);   
  lcd.print("    Passkey     ");
}

void displayErrorAdminPassword()
{
  //
  if (displayMode != 11)
  {
    return;  
  }

  //
//lcd.print("                ");
  lcd.setCursor(0,0);  
  lcd.print("    Invalid     ");
  lcd.setCursor(0,1);   
  lcd.print("    Passkey     ");
}

void displayOkUser()
{
  //
  if (displayMode != 20)
  {
    return;  
  }

  //
//lcd.print("                ");
  lcd.setCursor(0,0);  
  lcd.print("     Locker     ");
  lcd.setCursor(0,1);   
  lcd.print("    Unlocked    ");
}

void displayOkAdminSetDoor()
{
  //
  if (displayMode != 22)
  {
    return;  
  }

  //
//lcd.print("                ");
  lcd.setCursor(0,0);  
  lcd.print("     Locker     ");
  lcd.setCursor(0,1);   
  lcd.print("  New Pass  OK  ");
}

void displayOkAdminSetNew()
{
  //
  if (displayMode != 23)
  {
    return;  
  }

  //
//lcd.print("                ");
  lcd.setCursor(0,0);  
  lcd.print(" Update  Admin ");
  lcd.setCursor(0,1);   
  lcd.print("  New Pass  OK  ");
}

void displayOkAdminReset()
{
  //
  if (displayMode != 24)
  {
    return;  
  }

  //
//lcd.print("                ");
  lcd.setCursor(0,0);  
  lcd.print(" Update  Admin ");
  lcd.setCursor(0,1);   
  lcd.print(" Reset Pass OK ");
}
