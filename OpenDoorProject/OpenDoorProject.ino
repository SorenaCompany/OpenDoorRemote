#include <ESP8266WebServer.h>
#include <Arduino.h>


ESP8266WebServer server;
const char* ssid = "MobinNet-4ACD";
const char* password = "6Qvhmcse";
IPAddress localIP(192, 168, 2, 3);
IPAddress gateway(192, 168, 2, 3);
IPAddress subnet(255, 255, 255, 0);

const char* PARAM_INPUT_1 = "input1";
unsigned long resetInterval = 24 * 60 * 60 * 1000;  // Set the reset interval to 24 hours (in milliseconds)
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

		button {
			background-color: #4CAF50;
			color: white;
			padding: 60px 120px;
			border: none;
			border-radius: 25px;
			font-size: 64px;
			cursor: pointer;
			transition: background-color 0.3s ease;
		}

	button:hover {
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
		<div class="center">
    	<label for="input1">باز کردن درب خانه هوشمند</label>
			<button id="door1" onclick="sendData('door1')">Click Me!!</button>
		</div>
	

  <script>
  function sendData(butn){
    var URL,variable;
    if(butn=='door1')
    {
      URL='OpenDoor';
      variable='door1';
    }
    var xhr=new XMLHttpRequest();
     xhr.onreadystatechange = function(butn)
  {
    if (this.readyState == 4 && this.status == 200)
    {
    document.getElementById(variable).innerHTML = this.responseText;
    }
  };
  xhr.open('GET', URL, true);
  xhr.send();
  }
  </script>
</body>


  </html>)rawliteral";



const uint32_t WATCHDOG_TIMEOUT = 86400000;
void setup() {
  Serial.begin(115200);
  lastResetTime = millis();
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  //WiFi.softAPConfig(localIP, gateway, subnet);  //predefined IP address
  //WiFi.softAP(ssidAp, password);                // initialise Wi-Fi
  server.on("/", handleRoot);        // initialise server
  server.on("/OpenDoor", openDoor);  // load default webpage
  server.begin();
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
void handleRoot() {
  Serial.println("inside handleRoot");
  server.send_P(200, "text/html", index_html);
}
void openDoor()  // function to load default webpage and send HTML code to client
{
  Serial.println("inside openDoor");
  digitalWrite(5, LOW);
  delay(700);
  digitalWrite(5, HIGH);
  server.send_P(200, "text/plain", "Button Text Changed");
   // document.getElementById("door1").innerHTML = "Changed....";

}
void loop() {
  server.handleClient();
  resetESP();
   // server.send_P(200, "text/plain", "Click meee");

}

void resetESP() {
  if (millis() - lastResetTime >= resetInterval) {
    Serial.println("Resetting...");
    delay(2000);
    ESP.reset();  // Reset the ESP8266
  }
}
