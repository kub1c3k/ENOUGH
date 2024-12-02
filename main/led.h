#pragma once
#include "driver/gpio.h"

class Led
{
private:
  gpio_num_t pin;

public:
  Led(gpio_num_t pin);
  void on();
  void off();
  bool getState();
};