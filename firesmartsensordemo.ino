#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "DC3A94"; // your wireless network name (SSID)
const char* password = "09828667";

bool active_alarm = true;
bool fire = true;
WiFiClient client;
HTTPClient http;
short timecounted;
  char c ;
  char dataStr[16];
  int x = 0;
  int smokepin = 4;
  int counter = 0;
  bool calledfireservice = false;
  int temppin = 13;
  int buzzerpin = 5;

void setup() {
  // put your setup code here, to run once:

  // get the wifi password 

  // activate the variables 

  // connect to the wifi , if the alarm s
  
     pinMode(buzzerpin, OUTPUT);
    Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("connecting....");
  }

  Serial.println("connected");

 

}
void ringbuzzer()
{
  digitalWrite(buzzerpin,HIGH);
}
void unringbuzzer()
{
  digitalWrite(buzzerpin,HIGH);
}
void shutdownalarm()
{
    //this sends the current temperature to the server 
    String url = "http://firesmart.herokuapp.com/?type=action&userkey=44ff&requesttype=cancelalarm&uservalue=nn";
    http.begin(url); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code

        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
}

void loop(){
    int analogSensor = analogRead(temppin) ; //* (3300/1024);
   float voltage = (analogSensor - 500)/ 10;
   sendtemperature(voltage);
  if(active_alarm==true)
  {
    ringbuzzer();
    if(counter==1800)
    {
      if(callfireservice)
      {
        //this block checks if 911 has already been called 
      }
      else
      {
      call911();
      callfireservice=true;
      }
      counter = 0;
      sendx("There is a fire in home !!! , 911 has been called go to the app to cancel alarm if its false","pGtLg8tBiseFYh_qKDzgQ3k-u77aYE8xCJIAQZvjtn-");
      ringbuzzer();
    }
    else
    {
        int tempalarmstatus = checkalarmstatus();
        if(tempalarmstatus==2)
        {
          sendx("There is a fire in home !!! , send stop to deactivate if not after 20 min this will call 911","pGtLg8tBiseFYh_qKDzgQ3k-u77aYE8xCJIAQZvjtn-");
          ringbuzzer();
          counter++;
        }
        else
        {
          active_alarm=false;
          shutdownalarm();
        }        
        
    }

    
  }
  else
  {
  
    int analogSensor = analogRead(smokeA0);

    Serial.print("Pin A0: ");
    Serial.println(analogSensor);
    // Checks if it senses any type of smoke 
    if (analogSensor > 0)
    {
      sendx("There is a fire in home !!! , send stop to deactivate if not after 20 min this will call 911","pGtLg8tBiseFYh_qKDzgQ3k-u77aYE8xCJIAQZvjtn-");
      updatealarmstatus("on");
      active_alarm = true;
      ringbuzzer();
      counter++;  
    }
    else
    {
      
    }

    
  }


  delay(3000);
}

void call911()
{
      //this sends the current temperature to the server 
    String url = "http://firesmart.herokuapp.com/?type=action&userkey=44ff&requesttype=call911&uservalue=nn";
    http.begin(url); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code

        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
}

void sendx(String message,String key)
{
    const char* server = "maker.ifttt.com";
    if (client.connect(server,80)) {
    
    // Construct the message to send to the api

      String data = "{\"value1\":\"";
      data.concat(message);
      data.concat("\"}");
      
  client.print("POST /trigger/");
  client.print("ESP");
  client.print("/with/key/");
  client.print("pGtLg8tBiseFYh_qKDzgQ3k-u77aYE8xCJIAQZvjtn-");
  client.println(" HTTP/1.1");

  client.println("Host: maker.ifttt.com");
  client.println("User-Agent: fire smart");
  client.println("Connection: close");

  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(data.length());
  client.println();
  client.println(data);
  Serial.println("lifeisgoooo");
  }
  else
  {
    reconnectwifi();
  }

      
}

void reconnectwifi()
{
  if (WiFi.status() == WL_CONNECTED)  // We're connected
  {
    Serial.println("\r\nWiFi connected");
  }
  else
  {
    Serial.println("not connected");
  }
}

int checkalarmstatus()
{
  
 // HTTPClient http;
 // checked

    String payload;
    int result;

    http.begin("http://firesmart.herokuapp.com/?type=access&userkey=44ff&requesttype=alarmstatus"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code

        payload = http.getString();
      }

    else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
    
    if(httpCode > 0)
    {
    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(payload);

    String wordresult = root["alarmstatus"];
      if(wordresult == "off")
      {
        result = 1;
      }
      else
      {
        result = 2;
      }
    }
    else
    {
      result = 0;
    }

    return result;

    
}

void sendtemperature(float tempsend)
{
  //this sends the current temperature to the server 
  //checked
    String url = "http://firesmart.herokuapp.com/?type=insert&userkey=44ff&requesttype=temperature&uservalue=";
    url.concat(tempsend);
    http.begin(url); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code

        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
}

void updatealarmstatus(String kaf)
{
  //this sends the current temperature to the server 
      //checked
    String url = "http://firesmart.herokuapp.com/?type=insert&userkey=44ff&requesttype=alarmstatus&uservalue=";

    url.concat(kaf);
    http.begin(url); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code

        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }

    else {
      Serial.println("Error on HTTP request");
    }

    

    http.end(); //Free the resources
}




