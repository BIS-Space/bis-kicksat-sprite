/*
    This code is the beta version of the BIS sprite code and may include some 
    bugs, it doesn't include the transmission code. It will cycle through 
    a)transmitting the temperature b)startup info including number of startups 
    and duration of the maximum, minimum and average sessions c)the results of 
    a memory integrity (radiation) test.
    
    When printing to serial monitor it will print as normal characters for easy reading
    with headers. It will also print the character code as it will do in the final code.
    This will look like a bunch of numbers, each number represents a character. The first
    character is the header eg.82(R), 83(S) or 84(T). The next numbers are in sets of two
    with the first increasing upto 255 at which point it increments the second, this is the 
    hex representation of the integer being transmitted.
    
*/


#include "CCFlash.h" //To allow the use of the flash memory
#include "temp.h" //To allow the use of the temperature sensor
int32_t TempM;
int32_t TempG;
#include "radtest.h" //To allow the use of the radiation memory integrity test
#define flash_C SEGMENT_C //To use segment C
#define flash_D SEGMENT_D //To use segment D
unsigned char p = 0; //Set p as a variable which we will use later for segment C
unsigned char q = 0; //Set q as a variable which we will use later for segment D
unsigned int errors = 0; //This will be used for the memory integrity test


void setup() 
{
  Serial.begin(9600); //Start serial connection
  
  premult = 10; //Setup for temperature sensor
  mainTempCalibrate();
  mainTempRead(); // First reading is usually spurious
  
  Flash.read(flash_C, &p, 1); //Read flash memory segment C
  
  /*
  Startup_Initialise(); //As a first step if you have a problem with the code not working, uncomment these three lines
  Flash.erase(flash_D);
  Flash.write(flash_D, (unsigned char*) "A" ,1); 
  */
  
  if (!p=='S') Startup_Initialise(); //If the first character is not 'S' (on first ever startup) do the initialisation code
  else Startup_Update();  //If it is not the first ever startup, update the data for the previous session
  
  Flash.read(flash_D, &q, 1);   //Read 'q' in the flash memory
  if ((!q=='A') && (!q=='B') && (!q=='C')) //If 'q' is not A,B,C then: 
  {
      Flash.erase(flash_D);     //Clear the flash memory
      Flash.write(flash_D, (unsigned char*) "A" ,1);    //Write 'A' to the memory
  }
}

//----------------------------------------------------------------------------------------------------------------------------

void loop() 
{
  Flash.read(flash_D, &q, 1);    //Read 'q' in the memory
  switch(q)
  {
    case 'A':                 //If 'q' is 'A' then:
      Flash.erase(flash_D);     //Clear the memory...
      Flash.write(flash_D, (unsigned char*) "B" ,1);  //Write 'B' to it and...
      Serial.print("Temperature:  ");
      Temperature(); //Execute the tempterature sensor code
      break ;
    case 'B':                  //If 'q' is 'B' then:
      Flash.erase(flash_D);      //Clear the memory...
      Flash.write(flash_D, (unsigned char*) "C" ,1);  //Write 'C' to it and...
      Serial.println("Session info: ");
      Startup_Transmit(); //Execute the session info code
      break ;
    case 'C':                  //If 'q' is 'C' then:
      Flash.erase(flash_D);      //Clear the memory...
      Flash.write(flash_D, (unsigned char*) "A" ,1);  //Write 'A' to it and...
      MemoryTest(); //Execute the memory integrity test code
      break ;
  }
  Serial.println("----------------");
  delay(3000);
  Serial.println("++++++++++++++++");
}


  //-------------------------------------------------------------------------------------------------------------------------------

  void Startup_Initialise()
  {
    erase_memory();
    unsigned char message[19]; //Make 'message' 19 bytes long
    message[0] = 'S'; //Set first character to 'S'
    union //Use union to translate floating points and integers to characters
    {
      float union_f;
      unsigned int union_i;
      char union_s[4];
    };
    union_i = 1; //Set character 2 and 3 as number of startups to 1
    message[1] = union_s[0];
    message[2] = union_s[1];

    union_f = millis(); //Set characters 4-7 to time since startup
    message[3] = union_s[0];
    message[4] = union_s[1];
    message[5] = union_s[2];
    message[6] = union_s[3];

    union_f = 0; //Set characters 8-11 to 0 (they will be used as minimum time awake)
    message[7] = union_s[0];
    message[8] = union_s[1];
    message[9] = union_s[2];
    message[10] = union_s[3];

    message[11] = union_s[0]; //Set characters 12-15 to 0 (they will be used as maximum time awake)
    message[12] = union_s[1];
    message[13] = union_s[2];
    message[14] = union_s[3];

    message[15] = union_s[0]; //Set characters 16-19 to 0 (they will be used as average time awake)
    message[16] = union_s[1];
    message[17] = union_s[2];
    message[18] = union_s[3];

    Flash.erase(flash_C);
    Flash.write(flash_C, (unsigned char*) message ,19); //Write this starting data to segment_C
  }

  //-------------------------------------------------------------------------------------------------------------------------------

  void Startup_Update()
  {
    unsigned char message[19]; //Make 'message' 19 bytes long
    for (int i = 0; i < 19; i ++) //Read message from memory
    {
      Flash.read(flash_C + i, &p, 1);
      message[i] = p;    
    }
    union //Use union to translate floating points and integers to characters
    {
      float union_f;
      unsigned int union_i;
      char union_s[4];
    };
    union_s[0] = message[1]; //Set number of startups to message
    union_s[1] = message[2];
    unsigned int NoStartups = union_i;
    union_s[0] = message[3]; //Set duration of last session to message
    union_s[1] = message[4];
    union_s[2] = message[5];
    union_s[3] = message[6];
    float Duration = union_f;
    union_s[0] = message[7]; //Set minimum duration to message
    union_s[1] = message[8];
    union_s[2] = message[9];
    union_s[3] = message[10];
    float MinDuration = union_f;
    union_s[0] = message[11]; //Set maximum duration to message
    union_s[1] = message[12];
    union_s[2] = message[13];
    union_s[3] = message[14];
    float MaxDuration = union_f;
    union_s[0] = message[15]; //Set average duration to message
    union_s[1] = message[16];
    union_s[2] = message[17];
    union_s[3] = message[18];
    float AveDuration = union_f;
    
    //Now update data for latest session
    Serial.println("Duration and MaxDuration");
    Serial.println(Duration);
    Serial.println(MaxDuration);
    if(MinDuration) MinDuration = min(Duration, MinDuration);
    else MinDuration = Duration;
    //If(MaxDuration) 
    Serial.println("Resetting MaxDuration");
    MaxDuration = max(Duration, MaxDuration);
    //Else MaxDuration = Duration;
    //MaxDuration = max(Duration, MaxDuration);
    Serial.println("Duration and MaxDuration");
    Serial.println(Duration);
    Serial.println(MaxDuration);
    AveDuration = (Duration + ((NoStartups - 1) * AveDuration))/(NoStartups);
    Duration = millis();
    NoStartups ++;

    //Now write back to message
    union_i = NoStartups; //Set character 2 and 3 as number of startups to 1
    message[1] = union_s[0];
    message[2] = union_s[1];

    union_f = Duration; //Set characters 4-7 to time since startup
    message[3] = union_s[0];
    message[4] = union_s[1];
    message[5] = union_s[2];
    message[6] = union_s[3];

    union_f = MinDuration; //Set characters 8-11 to 0 (they will be used as minimum time awake)
    message[7] = union_s[0];
    message[8] = union_s[1];
    message[9] = union_s[2];
    message[10] = union_s[3];

    union_f = MaxDuration; //Set characters 12-15 to 0 (they will be used as maximum time awake)
    message[11] = union_s[0]; 
    message[12] = union_s[1];
    message[13] = union_s[2];
    message[14] = union_s[3];

    union_f = AveDuration;  //Set characters 16-19 to 0 (they will be used as average time awake)
    message[15] = union_s[0];
    message[16] = union_s[1];
    message[17] = union_s[2];
    message[18] = union_s[3];

    Flash.erase(flash_C);
    Flash.write(flash_C, (unsigned char*) message ,19); //Write this starting data to segment_C
    Serial.println("Updated");
  }

  //-------------------------------------------------------------------------------------------------------------------------------

  void Startup_Transmit()
  {
    unsigned char message[19]; //make 'message' 19 bytes long
    for (int i = 0; i < 19; i ++) //read message from memory
    {
      Flash.read(flash_C+i, &p, 1);
      message[i] = p;
    }
    union //Use union to translate floating points and integers to characters
    {
      float union_f;
      unsigned int union_i;
      char union_s[4];
    };
    union_s[0] = message[1]; //Set number of startups to message
    union_s[1] = message[2];
    unsigned int NoStartups = union_i;
        Serial.print("Number of startups: ");
        Serial.println(NoStartups);
    union_s[0] = message[3]; //Set duration of last session to message
    union_s[1] = message[4];
    union_s[2] = message[5];
    union_s[3] = message[6];
    float Duration = union_f;
        Serial.print("Current time awake: ");
        Serial.println(Duration);
    union_s[0] = message[7]; //Set minimum duration to message
    union_s[1] = message[8];
    union_s[2] = message[9];
    union_s[3] = message[10];
    float MinDuration = union_f;
        Serial.print("Minimum duration: ");
        Serial.println(MinDuration);
    union_s[0] = message[11]; //Set maximum duration to message
    union_s[1] = message[12];
    union_s[2] = message[13];
    union_s[3] = message[14];
    float MaxDuration = union_f;
        Serial.print("Maximum duration: ");
        Serial.println(MaxDuration);
    union_s[0] = message[15]; //Set average duration to message
    union_s[1] = message[16];
    union_s[2] = message[17];
    union_s[3] = message[18];
    float AveDuration = union_f;
        Serial.print("Average duration: ");
        Serial.println(AveDuration);

    //Now update data for latest session
    Duration = millis();

    //Now write back to message

    union_f = Duration; //Set characters 4-7 to time since startup
    message[3] = union_s[0];
    message[4] = union_s[1];
    message[5] = union_s[2];
    message[6] = union_s[3];


    Flash.erase(flash_C);
    Flash.write(flash_C, (unsigned char*) message ,19); //Write this starting data to segment_C

    unsigned int DurationSecs = Duration/1000;
    unsigned int MinDurationSecs = MinDuration/1000;
    unsigned int MaxDurationSecs = MaxDuration/1000;
    unsigned int AveDurationSecs = AveDuration/1000;
    char InfoTrans[11];
    InfoTrans[0] = message[0];
    union_i = NoStartups;
    InfoTrans[1] = union_s[0];
    InfoTrans[2] = union_s[1];
    union_i = DurationSecs;
    InfoTrans[3] = union_s[0];
    InfoTrans[4] = union_s[1];
    union_i = MinDurationSecs;
    InfoTrans[5] = union_s[0];
    InfoTrans[6] = union_s[1];
    union_i = MaxDurationSecs;
    InfoTrans[7] = union_s[0];
    InfoTrans[8] = union_s[1];
    union_i = AveDurationSecs;
    InfoTrans[9] = union_s[0];
    InfoTrans[10] = union_s[1];
    Serial.println(InfoTrans);
    unsigned char c;
    for (int i = 0; i<11; i++)
    {
      c = InfoTrans[i];
      Serial.print(c); 
      Serial.print("  ");
    }
    Serial.println(" ");
  }

//--------------------------------------------------------------------------------------------------------------------------------------

void Temperature() {  //Print the temperature sensor data from both sensors with headings to the serial monitor:

  union //Use union to translate floating points and integers to characters
    {
      float union_f;
      unsigned int union_i;
      char union_s[4];
    };

  TempM = mainTempRead();
  Serial.print("main: ");
  Serial.print(TempM / 10);
  Serial.print('.');
  Serial.print(TempM % 10);
  Serial.print(" degrees, ");

  TempG = gyroTempRead();
  Serial.print("gyro: ");
  Serial.print(TempG / 10);
  Serial.print('.');
  Serial.print(TempG % 10);
  Serial.println(" degrees"); 
  
  char TempTrans[5];
    TempTrans[0] = 'T';
    union_i = TempM / 10;
    TempTrans[1] = union_s[0];
    TempTrans[2] = union_s[1];
    union_i = TempG / 10;
    TempTrans[3] = union_s[0];
    TempTrans[4] = union_s[1];
    Serial.println(TempTrans);
    unsigned char c;
    for (int i = 0; i<5; i++)
    {
      c = TempTrans[i];
      Serial.print(c); 
      Serial.print("  ");
    }
    Serial.println(" ");
  
}


void MemoryTest() {
  union //Use union to translate floating points and integers to characters
    {
      float union_f;
      unsigned int union_i;
      char union_s[4];
    };
    
  errors = check_memory();
  
      Serial.print("Memory Integrity Test: ");
      Serial.print("errors: ");
      Serial.println(errors);
      
  char MemTrans[3];
  MemTrans[0] = 'R';
  union_i = errors;
  MemTrans[1] = union_s[0];
  MemTrans[2] = union_s[1];
  Serial.println(MemTrans);
  unsigned char c;
  for (int i = 0; i<3; i++)
    {
      c = MemTrans[i];
      Serial.print(c); 
      Serial.print("  ");
    }
    Serial.println(" ");
}
