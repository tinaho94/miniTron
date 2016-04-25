#include <SparkFunESP8266Server.h>
#include <SparkFunESP8266WiFi.h>
#include <math.h>

/*
 * Configuration Information
 */
const char mySSID[] = "miniTron";
const char myPSK[] = "password";
//const char macAddr[] = "5e:cf:7f:10:cd:97";
const char myIP[] = "192.168.4.1";
char localMAC[24];
char softMAC[24];
char apConfig[24];

/*
 * User Configuration Information
 */
//const char yourSSID[];
//const char yourPSK[];


ESP8266Server server = ESP8266Server(80);

void setup() 
{
  delay(1000);
  Serial.begin(115200);
  delay(2000);
  
  initializeESP8266();
  delay(1000);

  while(setupAccessPoint() < 0) {
    delay(3000);
  }

  setupServer();
}

void initializeESP8266()
{
  //esp8266.setAutoConnect(false);
  int test = esp8266.begin();
  delay(6000);
  if (test != true) {
    Serial.println(F("Error talking to ESP8266."));
    errorLoop(test);
  }
  Serial.println(F("ESP8266 Device Present"));

  esp8266.reset();
  Serial.println(F("reset"));
  delay(1000);

  esp8266.echo(0);
  Serial.println(F("echo off"));
}

int setupAccessPoint() {

  int retVal = esp8266.getMode();
  if(retVal != ESP8266_MODE_STAAP) {
    //esp8266.disconnect();
    retVal = esp8266.setMode(ESP8266_MODE_STAAP);
  }

  if (retVal < 0) {
    Serial.println(F("Error setting mode."));
    errorLoop(retVal);
  }

  esp8266.reset();
  Serial.println(F("reset"));
  delay(1000);
  
  Serial.println(F("Mode set to station and access point"));

  retVal = esp8266.status();
  
  if(retVal <= 0) {
    Serial.println(F("Setting Up Access Point"));
    retVal = esp8266.setSoftAP(mySSID, myPSK, 4, ESP8266_ECN_OPEN);
    delay(300);

    if(retVal < 0) {
      Serial.println(F("Error setting up access point."));
    }
  }

  Serial.println("Access Point miniTron has been set up");
  esp8266.getSoftAP(apConfig);
  Serial.println(apConfig);

  IPAddress ipaddr = esp8266.localIP();
  Serial.println("this is the soft ip:");
  Serial.println(ipaddr);

  esp8266.localMAC(localMAC);
  Serial.println("My local MAC");
  Serial.println(localMAC);

  esp8266.getSoftAPMAC(softMAC);
  Serial.println("My soft AP MAC");
  Serial.println(softMAC);

  // allow multiple TCP connections
  esp8266.setMux(1);  
  Serial.println(F("Multiple TCP connections set"));
  server.begin();
  
  return retVal;
}

void setupServer()
{
  uint8_t serverStatus = server.status();
  if(serverStatus < 0) {
    if(serverStatus == -1) {
      Serial.println(F("Server is connected, but have not gotten an IP."));

    } else if(serverStatus == -2) {
      Serial.println(F("No wifi configured."));
    }
  }
  Serial.println(F("Successful server setup!"));
  Serial.println(F("Go to "));
  Serial.println(esp8266.localIP());
}

// errorLoop prints an error code, then loops forever.
void errorLoop(int error)
{
  Serial.print(F("Error: ")); 
  Serial.println(error);
  Serial.println(F("Looping forever."));
  for (;;);
}


void loop() 
{
  configPage();
  //delay(1000);
}

// get a full header terminated with an empty line from the client
int getHeaderFromClientWithTimeout(ESP8266Client *client, char *pBuffer, int nBuffer, int timeout, int nAvailable)
{
    unsigned long starttime = millis();
    int n = 0;
    int emptyline = 0;

    // keep reading header until I find a crlf
    while (client->connected() && (starttime + timeout > millis())) 
    {       
      // if data is available
      if (nAvailable > 0) 
      {       
          // read the data
          int i;   
          for(i=0;i<nAvailable;i++)
          {            
            char c = client->read();
            //if(c<0)
            //{
            //  nAvailable = 0;
            //  break;
            //}
            pBuffer[n++] = c;

            // other remember that we just found a linefeed and are beginning a new line
            if(c=='\n' ) 
              emptyline++; // = true;

            // found non-cr or lf character, not an empty line
            else if(c!='\n' && c!='\r')
              emptyline = 0;

            // return we encounter the end of an empty line
            if(emptyline >= 2) //&& c=='\n')
            { 
              // terminate our buffer with a null character
              pBuffer[n] = '\0';
              
              // and return the number of characters we read
              Serial.println("what i read:");
              Serial.println(pBuffer);
              return n;
            }

          }
       }
       nAvailable = client->available();      
       
       delay(1);  // <-- this is absolutely necessary or the esp8266 will not send the entire header!

       // check for more data to read
    }  

    // terminate our buffer with a null character
    pBuffer[n] = '\0';

    // and return the number of characters we read
    return n;
}

#define TIMEOUT_DISCONNECT 60
#define READBUFFERSIZE 1024
void configPage() {
  ESP8266Client client = (ESP8266Client) server.available(500);
  Serial.println(client.status());
  if(client) {
    Serial.println("WooHoo! I have a client!!!");
    unsigned long lastActivity = millis();
  
    delay(1);
  
    Serial.println(F("======================="));
//    while (client.connected() && !client.available()) {
//      delay(1);
//    }

    while(client.connected()) {
      int nAvailable = 0;
      char headerBuffer[READBUFFERSIZE];
      int nHeaderBuffer = 0;
      headerBuffer[0] = '\0';

      if((nAvailable = client.available()) > 0) {
        lastActivity = millis();
        int retval =  getHeaderFromClientWithTimeout(&client, headerBuffer, READBUFFERSIZE, 10000, nAvailable);        
        nHeaderBuffer = retval;        
        
        Serial.printf("GetHeader returned %d\n", retval);
        Serial.println(F("HTTP Request Content"));
        Serial.println(F("-----------------------"));
        Serial.write(headerBuffer);
        // found favicon request
        if(strstr(headerBuffer,"favicon.ico"))
        {
          char httpResponse[1024];
          httpResponse[0] = '\0';

          // reply with 404 error
          sprintf(&httpResponse[strlen(httpResponse)], "HTTP/1.0 404 \r\n\r\n");
          Serial.println(F("HTTP Response Content"));
          Serial.println(F("-----------------------"));
          Serial.print(httpResponse);
          Serial.println(F("-----------------------"));
          Serial.println(F("Finished sending favicon response"));
          client.print(httpResponse);          
        } 

        // found GET request
        else if(char *pGET = strstr(headerBuffer,"GET"))
        {
          // parse the URL
          char URL[128];
          int i;          
          for(i=0;(pGET[i]!=' ')&&(i<128);i++);
          char *pURL = &pGET[i+1];          
          for(i=0;(pURL[i]!=' ')&&(i<128);i++);
          strncpy(URL,pURL,i);
          URL[i] = '\0';
          
          //Serial.print(">");
          //Serial.print(URL);
          //Serial.println("<");

          // default http page
          if(!strncmp(URL, "/config.html", 7))
          {
            char htmlBody[2048];
            htmlBody[0] = '\0';

            // first build the html page that will be returned
//            sprintf(&htmlBody[strlen(htmlBody)], 
//              "<!DOCTYPE HTML>\r\n"
//              "<html><head><title>Config</title></head>\r\n"
//              "<body>\r\n"
//              "Hello World!\r\n"
//              "</body></html>\r\n");
//        
//            sprintf(&htmlBody[strlen(htmlBody)],
//              "<!DOCTYPE html>\r\n"
//              "<html>\r\n"
//              "<head>\r\n"
//              "<title> Config </title>\r\n"
//              "</head>\r\n"
//              "<center>\r\n"
//              "<h1 style = \"color:#003366\"> miniTron Configurations </h1>\r\n"
//              "</center>\r\n"
//              "<body>\r\n"
//              "</body>\r\n"
//              "</html>\r\n");

            sprintf(&htmlBody[strlen(htmlBody)], 
            "<!DOCTYPE html>\r\n"
            "<html>\r\n"
              "<head>\r\n"
                "<title> Config </title>\r\n"
              "</head>\r\n"
              "<style>\r\n"
                "div {\r\n"
                    "width: 500px;\r\n"
                    "height: 350px;\r\n"
                    "padding: 0px;\r\n"
                    "border: 5px solid gray;\r\n"
                    "margin: 0px;\r\n"
                    "border-radius: 50px;\r\n"
                "}\r\n"
              "</style>\r\n"
              "<body style=\"background-color: #003366\">\r\n"
                "<center style=\"margin-top: 50px; font-size: 100px; color: #fff\">\r\n"
                  "miniTron Configurations\r\n"
                "</center>\r\n"
                "<center>\r\n"
                  "<div style=\"background-color: #fff; margin-top:70px\">\r\n"
                    "<center>\r\n"
                      "<h2 style=\"margin-left: 20px; margin-top: 30px\">\r\n"
                        "Wifi Configs\r\n"
                      "</h2>\r\n"
            
                      "<form id=\"wifi\" style=\"margin-top: 10px;\">\r\n"
                        "SSID:\r\n"
                        "<input type=\"text\" name=\"ssid\" style=\"margin-bottom:10px\">\r\n"
                        "<br>\r\n"
                        "Password:\r\n"
                        "<input type=\"password\" name=\"password\"> <br>\r\n"
                        "<h2 style=\"margin-left: 20px; margin-top: 30px\">\r\n"
                          "Display Configs\r\n"
                        "</h2> \r\n"
                        "<input type=\"radio\" name=\"gender\" value=\"twitter\" checked> Twitter <br>\r\n"
                        "<input type=\"radio\" name=\"gender\" value=\"temp\"> Temp <br>\r\n"
                      "</form>\r\n"
            
                      "<button type=\"submit\" form=\"wifi\" value=\"Submit\" style=\"margin-top:20px\">\r\n"
                        "Submit\r\n"
                      "</button>\r\n"
                    "</center>\r\n"
                  "</div>\r\n"
                "</center>\r\n"
              "</body>\r\n"
            "</html>\r\n");
            char httpResponse[1024];
            httpResponse[0] = '\0';

            // build the http response header, including the size of the html body (this is necessary)
            sprintf(&httpResponse[strlen(httpResponse)], 
                "HTTP/1.1 200 OK \r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %d\r\n"
                "Connection: keep-alive\r\n"
                "\r\n"
                "%s", 
                strlen(htmlBody), htmlBody );          

            // write the response to the debug console
            Serial.println(F("HTTP Response Content"));
            Serial.println(F("-----------------------"));
            Serial.print(httpResponse);

            // write the response to the esp8266            
            client.print(httpResponse);
            Serial.println(F("-----------------------"));
            Serial.println(F("Finished sending HTML page"));
          }
          // page not found: 404 error
          else
          {
            char httpResponse[1024];
            httpResponse[0] = '\0';          
  
            // http header
            sprintf(&httpResponse[strlen(httpResponse)], "HTTP/1.0 404 \r\n\r\n");
            
            // write the response to the debug console
            Serial.print(httpResponse);
            
            // write the response to the esp8266            
            client.print(httpResponse);          
            
            Serial.println(F("Finished Sending 404 error"));
          }            
        }  
      }
      // no data available so wait 1ms
      //else
      delay(1);
    }
    // give the web browser time to receive the data
    delay(10);
    client.flush();
    
    // close the connection:
    delay(10);
    client.stop();
    Serial.println(F("Client disconnected"));
  }
}
