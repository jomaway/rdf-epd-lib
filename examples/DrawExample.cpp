#include <Arduino.h>
#include <ws_4in2b_V2.h>

constexpr uint8_t Busy = D0;
constexpr uint8_t Reset = D1;
constexpr uint8_t DataCommand = D2;
constexpr uint8_t ChipSelect = D3;

WS_4in2b_V2 epd(ChipSelect, DataCommand, Reset, Busy);

void setup()
{
  Serial.begin(115200);
  epd.begin();
  // you can use all functions from AdafruitGFx to draw something.
  epd.drawCircle(50, 150, 30, EPD::Color::Black);
  epd.drawLine(0, 0, 400, 300, EPD::Color::Black);
  epd.drawCircle(300, 100, 50, EPD::Color::Black);
  epd.display();   // make the change visible to the screen.
  epd.end(); // make sure to send epaper back to sleep.
}

void loop()
{
  // put your main code here, to run repeatedly:
}
