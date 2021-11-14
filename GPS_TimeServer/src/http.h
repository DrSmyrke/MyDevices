//------------------------------------------------------
void httpHandleIndex(void)
{
	String page = 
	"<!DOCTYPE html><html lang=\"en\"><head lang=\"en\"><title>-= GPS =-</title></head><body>"
	"<b>latitude:</b> " + String( gps.latitude ) + "°<br>"
	"<b>longitude:</b> " + String( gps.longitude ) + "°<br>"
	"<b>altitude:</b> " + String( gps.altitude ) + "m<br>"
	"<b>time:</b> " + String( gps.Hours ) + ":" + String( gps.minutes ) + ":" + String( gps.seconds ) + " " +
	String( gps.day ) + "." + String( gps.month ) + "." + String( gps.year ) + "<br>"

	"<b>Unix:</b> " + String( gps.Unix ) + "<br>"
	"<b>speed:</b> " + String( gps.speed ) + "km/h<br>"
	"<b>satellites:</b> " + String( gps.satellites[ GPS_ACTIVE ] ) + "/" + String( gps.satellites[ GPS_VISIBLE ] ) + "<br>"

	//http://maps.yandex.ru/?ll=gps.longitude,gps.latitude&pt=gps.longitude,gps.latitude&l=map&z=18
	"</body></html>"
	;

	webServer.send(200, "text/html", page);
}

//------------------------------------------------------
void httpHandleData(void)
{
	String page = 
	"{"
	"\"latitude\":  \"" + String( gps.latitude ) + "\","
	"\"longitude\":  \"" + String( gps.longitude ) + "\","
	"\"altitude\":  \"" + String( gps.altitude ) + "\","
	"\"time\":  \"" + String( gps.Hours ) + ":" + String( gps.minutes ) + ":" + String( gps.seconds ) + "\","
	"\"date\":  \"" + String( gps.day ) + "." + String( gps.month ) + "." + String( gps.year ) + "\","
	"\"Unix\":  \"" + String( gps.Unix ) + "\","
	"\"speed\":  \"" + String( gps.speed ) + "\","
	"\"satellites\":  \"" + String( gps.satellites[ GPS_ACTIVE ] ) + "/" + String( gps.satellites[ GPS_VISIBLE ] ) + "\""

	//http://maps.yandex.ru/?ll=gps.longitude,gps.latitude&pt=gps.longitude,gps.latitude&l=map&z=18
	"}"
	;

	webServer.send(200, "application/json", page);
}

//------------------------------------------------------
void httpHandleNotFound(void)
{
	webServer.send( 404, "text/html", "" );
}
