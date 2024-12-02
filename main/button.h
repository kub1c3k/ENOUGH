#pragma once

#include "soc/gpio_num.h"
class Button
{
private:
  gpio_num_t pin;

public:
  Button(gpio_num_t pin);
  bool isPressed();
};
