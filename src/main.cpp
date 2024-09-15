#include <Arduino.h>
#include "fabgl.h"

fabgl::ILI9341Controller DisplayController;
fabgl::Terminal          Terminal;

// Cheap Yellow Display PINS
#define TFT_SCK    14
#define TFT_MOSI   13
#define TFT_CS     15
#define TFT_DC     2
#define TFT_RESET  -1
#define TFT_BL     21

#define BAUDRATE  115200
#define SLOWPRINTDELAY 0

void slowPrintf(const char * format, ...);
void slowPrintLine(const char c);

void setup()
{
  unsigned int cols,rows,width,height;
  Serial.begin(BAUDRATE); Serial.write("\n\n\n");

  DisplayController.begin(TFT_SCK, TFT_MOSI, TFT_DC, TFT_RESET, TFT_CS, HSPI_HOST, TFT_BL);
  DisplayController.setResolution(TFT_240x320);
  DisplayController.setOrientation(fabgl::TFTOrientation::Rotate90);

  Terminal.begin(&DisplayController);
  Terminal.loadFont(&fabgl::FONT_6x10);
  Terminal.write("\e[40;33m"); // background: black, foreground: Yellow
  Terminal.write("\e[2J");     // clear screen
  Terminal.write("\e[11;20H"); // move cursor to middle of screen
  slowPrintf("\e[1m<< STARTUP >>");
  delay(2500);                 // REQUIRED to settle FabGL display controller

  Terminal.write("\e[40;32m"); // background: black, foreground: green
  Terminal.write("\e[2J");     // clear screen
  Terminal.write("\e[1;1H");   // move cursor to 1,1

  cols = Terminal.getColumns();
  rows = Terminal.getRows();
  height = DisplayController.getScreenWidth(); // rotated screen
  width = DisplayController.getScreenHeight(); // rotated screen
  slowPrintf("* * * \e[31mA\e[32mN\e[33mS\e[34mI\e[32m-VT100 serial terminal * * *\r\n");
  slowPrintf("Serial speed  : %d 8 N1\r\n", BAUDRATE);
  slowPrintf("Screen size   : %d x %d\r\n", width,height);
  slowPrintf("Terminal size : %d x %d\r\n", cols,rows);
  slowPrintLine('=');
  Terminal.write("\r\n");
  Terminal.enableCursor(true);

  pinMode(0, INPUT);
}

void slowPrintf(const char * format, ...)
{
  va_list ap;
  va_start(ap, format);
  int size = vsnprintf(nullptr, 0, format, ap) + 1;
  if (size > 0) {
    va_end(ap);
    va_start(ap, format);
    char buf[size + 1];
    vsnprintf(buf, size, format, ap);
    for (int i = 0; i < size; ++i) {
      Terminal.write(buf[i]);
      delay(SLOWPRINTDELAY);
    }
  }
  va_end(ap);
}

void slowPrintLine(const char c) {
  unsigned int maxcols = Terminal.getColumns();
  for(int i = 1; i < maxcols; i++) {
    Terminal.write(c);
    delay(SLOWPRINTDELAY);
  }
  Terminal.write("\r\n");
}

const char *colorCycle[] = {
  "\e[32m", // 0 GREEN
  "\e[37m", // 1 WHITE
  "\e[34m", // 2 BLUE
  "\e[33m", // 3 YELLOW
  "\e[31m", // 4 RED
  "\e[35m", // 5 MAGENTA
  "\e[36m", // 6 CYAN
};
unsigned int colorindex; // starts on GREEN

void loop()
{
  if(Serial.available() > 0) {
    Terminal.write(Serial.read());
  }
  if(digitalRead(0) == LOW) {
    if(++colorindex > (sizeof(colorCycle)/sizeof(colorCycle[0]))-1) colorindex = 0;
    Terminal.write(colorCycle[colorindex]);
    while(digitalRead(0) == LOW);
  }
}