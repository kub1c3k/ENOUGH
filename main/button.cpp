#include "button.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"

Button::Button(gpio_num_t pin) : pin(pin)
{
  gpio_reset_pin(this->pin);
  gpio_set_direction(this->pin, GPIO_MODE_INPUT);
  gpio_set_pull_mode(this->pin, GPIO_PULLUP_ONLY);
}

bool Button::isPressed()
{
  return !gpio_get_level(this->pin);
}
