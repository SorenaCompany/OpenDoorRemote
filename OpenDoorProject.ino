
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MobinNet-4ACD";
const char* password = "6Qvhmcse";

const char* PARAM_INPUT_1 = "input1";
unsigned long resetInterval = 24*60*60*1000; // Set the reset interval to 24 hours (in milliseconds)
unsigned long lastResetTime = 0;


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>


<head>
	<title>Button in the middle of the page</title>
  <meta charset="UTF-8">
	<style>
	body {
			background-color: #f5f5f5;
			font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
		}

		.center {
			display: flex;
			flex-direction: column;
			justify-content: center;
			align-items: center;
			height: 100vh;
		}

		input[type='submit'] {
			background-color: #4CAF50;
			color: white;
			padding: 60px 120px;
			border: none;
			border-radius: 25px;
			font-size: 64px;
			cursor: pointer;
			transition: background-color 0.3s ease;
		}

		input[type='submit']:hover {
			background-color: #3e8e41;
		}
    label {
			font-size: 32px;
			font-weight: bold;
			margin-bottom: 40px;
			text-align: center;
			text-transform: uppercase;
			color: #333;
			text-shadow: 1px 1px #fff;
		}
	</style>
</head>

<body>
	<form action="/get">
		<div class="center">
    	<label for="input1">باز کردن درب خانه هوشمند</label>
			<input type="submit" name="input1" value="Open Door" onclick="this.value='1';"></input>
		</div>
	</form>
</body>


  </html>)rawliteral";

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}
String inputMessage;
String inputParam;
const uint32_t WATCHDOG_TIMEOUT = 86400000;
void setup() {
  Serial.begin(115200);
  lastResetTime = millis();
  pinMode(5, OUTPUT); 
  digitalWrite(5, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest* request) {
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      Serial.print("DATA==>");
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      Serial.println(inputMessage);

      inputParam = PARAM_INPUT_1;
    }

    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" + inputParam + ") with value: " + inputMessage + "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  if (inputMessage == "1") {
    digitalWrite(5, LOW);
    delay(500);
    digitalWrite(5, HIGH);
    inputMessage="2";
  }
  resetESP();
}

void resetESP() {
  if (millis() - lastResetTime >= resetInterval) {
    Serial.println("Resetting...");
    delay(2000);
    ESP.reset(); // Reset the ESP8266
  }
}