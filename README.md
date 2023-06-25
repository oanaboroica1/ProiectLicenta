# ProiectLicenta

Sistemul este compus dintr-o aplicție web și doua sketch-uri: Arduino și ESP32.

1. ArduinoSketch.ino - se va face upload codului pe placa de dezvoltare Arduino
2. ESP32Sketch.ino - se va face upload codului pe modulul WiFi ESP32

3.Rularea aplicției
	a) Back-end: din „Command Prompt” se execută următoarele instrucțiuni:
			- cd OneDrive\Desktop\AplicatieWeb\blinkAPI
			- .\venv\Scripts\activate
			- python -m uvicorn main:app --reload 

	b) Front-end: din „Command Prompt” se execută următoarele instrucțiuni:
			- cd OneDrive\Desktop\AplicatieWeb\blinkFE
			- npm run dev

	Se accesează link-ul: http://localhost:3000/, unde se pot observa datele primite de la sistemul MOREP. 	
