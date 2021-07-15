# GardenController
Rework of GardenSensor to include more checks for things like pumps and other devices


## LED blink codes
MQTT connection error - red led will turn on for 1 second and then blink the error code number that is returned from PubSubClient library
DHT22 error - if a sensor read fails the red led will blink twice quickly
Data send - blue led will blink once when data is sent