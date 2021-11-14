os_timer_t myTimer;

void timerCallback(void *pArg)
{
	//Serial.print("WiFiConnectionState: ");Serial.println( WiFiConnectionState() );
	//Serial.print("WiFi IP: ");Serial.println( WiFi.localIP().toString() );
	//Serial.print("WiFi SSID: ");Serial.println( String(conf.clientWifiSSID) );
	
	if( !WiFiConnectionState() ){
		WiFi.reconnect();
	}
}

void timer_init(void)
{
	os_timer_setfn(&myTimer, timerCallback, NULL);
	os_timer_arm(&myTimer, 10000, true);
}
