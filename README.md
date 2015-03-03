# RoboControl
Robo Control ist eine App, die es ermöglicht einen Roboter drahtlos zu steuern. Die Steuerung erfolgt entweder über einen virtuellen Joystick oder mit Hilfe des Gesture Control Armband MYO von Thalmic Labs.
 
Um Robo Control zur Robotersteuerung zu verwendet, muss das Smartphone mit dem WiFi-Netzwerk, welches z.B. von einem XBee® WiFi Modul von Digi International erzeugt wird, verbunden sein.
Bei korrekter Verbindung sendet Robo Control bis zu zehn Mal in der Sekunde Steuerdaten an das WiFi Modul, welche von einem Mikrocontroller ausgewertet und weiterverarbeitet werden können.

Um MYO für die Steuerung zu verwenden, wird Bluetooth Low Energy (BLE) benötigt. 

Erste Schritte

1. WiFi verbinden
	Verbinden Sie ihr Smartphone mit dem WiFi-Modul.

	Robo Control -> WiFi Einstellungen
	oder
	Einstellungen (Smartphone) -> WLAN
	MYO koppeln

2. Koppeln Sie MYO mit ihrem Smartphone per Bluetooth LE. Rufen Sie dazu die Einstellungen des Smartphones auf.
	Robo Control > BT Einstellungen
	oder
	Einstellungen (Smartphone) -> Bluetooth
	Steuerung

3. Starten Sie nun Robo Control und passen Sie, wenn notwendig die Einstellungen an. Wählen Sie den gewünschten Control Modus aus.
	
Wenn Sie den Gesture Control Mode verwenden, wählen Sie im nächsten Dialog ihr Armband aus.
