#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

Led::Led(gpio_num_t pin) : pin(pin)
{
  gpio_reset_pin(pin);
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  off();
}

void Led::on()
{
  gpio_set_level(pin, 1);
}

void Led::off()
{
  gpio_set_level(pin, 0);
}

bool Led::getState()
{
  return gpio_get_level(pin);
}