#include <Arduino.h>
#include "SPIFFS.h"
#include <Preferences.h>
Preferences preferences;


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- frite failed");
    }
}
void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
}
void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.print("- read from file: ");
    while(file.available()){
        Serial.write(file.read());
        Serial.println(file.readString());
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void setup() {
  Serial.begin(115200);
  // Open Preferences with my-app namespace. Each application module, library, etc
  // has to use a namespace name to prevent key name collisions. We will open storage in
  // RW-mode (second parameter has to be false).
  // Note: Namespace name is limited to 15 chars.
  preferences.begin("BlueGate", false);

  // Remove all preferences under the opened namespace
  //preferences.clear();

  // Or remove the counter key only
  //preferences.remove("counter");

  preferences.putString("password", "123456");
  preferences.putString("mode", "tap");
  preferences.putBool("addMode", false);

  Serial.println(preferences.getString("mode"));
  Serial.println(preferences.getString("password"));
  Serial.println(preferences.getBool("addMode"));

  // Close the Preferences
  preferences.end();

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  //writeFile(SPIFFS,"/mac.txt","88:c9:d0:9a:22:78,1e:e9:fb:79:07:77,80:88:34:48:43:3c,85:94:b9:6c:ba:56,a1:24:ce:9e:40:41");
  //appendFile(SPIFFS,"/mac.txt",",88:88:88:88:88:88" );
  writeFile(SPIFFS, "/mac.txt", "88:88:88:88:88:88");
  readFile(SPIFFS, "/mac.txt");
  listDir(SPIFFS, "/", 0);

   Serial.println("Restarting in 5 seconds...");
  delay(5000);

  // Restart ESP
  ESP.restart();
}
 
void loop() {}