//main
#include "controller_display.h"

int main(void) {
  display_init();
  while (1) {
    test_LCD((ScreenState)currentScreen);
}

  return 0;
}