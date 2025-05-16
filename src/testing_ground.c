//main
#include "controller_display.h"

int main(void) {
  serial0_init();
  milliseconds_init();
  display_init();
  while (1) {
    test_LCD((ScreenState)currentScreen);
}

  return 0;
}