#include "functions.h"
#include "main.h"

//--------------------------------------------------
void fan_on(void)
{
    // Keyboard.press( 0xAC ); delay(10); Keyboard.release( 0xAC );
    // свернуть все окна, сочетание Win+D
    Keyboard.press( KEY_LEFT_GUI );
    delay( 10 );
    Keyboard.write( 'D' );
    delay( 10 );
    Keyboard.release( KEY_LEFT_GUI );
}

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------