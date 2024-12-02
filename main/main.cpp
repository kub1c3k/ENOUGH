#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "button.h"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "driver/i2c.h"
#include "esp_log.h"

Led led0(GPIO_NUM_13);
Led led1(GPIO_NUM_25);
Led led2(GPIO_NUM_27);
Led led3(GPIO_NUM_26);

Button button0(GPIO_NUM_15);
Button button1(GPIO_NUM_18);
Button button2(GPIO_NUM_19);
Button button3(GPIO_NUM_23);

std::vector<Led *> leds = {&led0, &led1, &led2, &led3};
std::vector<Button *> buttons = {&button0, &button1, &button2, &button3};

const int difficultyTimes[] = {5000, 4000, 2000, 1000};

extern "C" void app_main(void)
{
  srand(time(NULL));

  while (1)
  {
    printf("Select Difficulty Level:\n");
    printf("Button 0: Easiest (5000 ms)\n");
    printf("Button 1: Easy (4000 ms)\n");
    printf("Button 2: Hard (2000 ms)\n");
    printf("Button 3: Hardest (1000 ms)\n");

    int selectedDifficulty = -1;
    while (selectedDifficulty == -1)
    {
      for (int i = 0; i < buttons.size(); i++)
      {
        if (buttons[i]->isPressed())
        {
          selectedDifficulty = i;
          printf("Difficulty Level %d selected!\n", selectedDifficulty + 1);
          while (buttons[i]->isPressed())
          {
            vTaskDelay(pdMS_TO_TICKS(10));
          }
          break;
        }
      }
      vTaskDelay(pdMS_TO_TICKS(50));
    }

    int reactionTime = difficultyTimes[selectedDifficulty];

    printf("Press any button to start the game!\n");

    bool startPressed = false;
    while (!startPressed)
    {
      for (int i = 0; i < buttons.size(); i++)
      {
        if (buttons[i]->isPressed())
        {
          printf("Button %d pressed to start!\n", i);
          startPressed = true;

          while (buttons[i]->isPressed())
          {
            vTaskDelay(pdMS_TO_TICKS(10));
          }
          break;
        }
      }
      vTaskDelay(pdMS_TO_TICKS(50));
    }

    bool gameRunning = true;
    printf("Game Start!\n");

    while (gameRunning)
    {
      int rd_num = rand() % leds.size();
      leds[rd_num]->on();
      printf("LED %d is ON\n", rd_num);

      TickType_t startTime = xTaskGetTickCount();
      bool correctButtonPressed = false;

      while (xTaskGetTickCount() - startTime < pdMS_TO_TICKS(reactionTime))
      {
        for (int i = 0; i < buttons.size(); i++)
        {
          if (buttons[i]->isPressed())
          {
            if (i == rd_num)
            {
              correctButtonPressed = true;
              printf("Correct button %d pressed!\n", i);
              leds[rd_num]->off();
              reactionTime = (reactionTime > 200) ? reactionTime - 50 : reactionTime;
              break;
            }
            else
            {
              printf("Wrong button %d pressed! You lost!\n", i);
              leds[rd_num]->off();
              printf("Blinking LEDs...\n");
              for (int j = 0; j < 5; j++)
              {
                for (auto led : leds)
                {
                  led->on();
                }
                vTaskDelay(pdMS_TO_TICKS(200));
                for (auto led : leds)
                {
                  led->off();
                }
                vTaskDelay(pdMS_TO_TICKS(200));
              }
              gameRunning = false;
              break;
            }
          }
        }
        if (!gameRunning || correctButtonPressed)
          break;
      }

      if (!correctButtonPressed && gameRunning)
      {
        printf("You lost! Time ran out!\n");
        printf("Blinking LEDs...\n");
        for (int i = 0; i < 5; i++)
        {
          for (auto led : leds)
          {
            led->on();
          }
          vTaskDelay(pdMS_TO_TICKS(200));
          for (auto led : leds)
          {
            led->off();
          }
          vTaskDelay(pdMS_TO_TICKS(200));
        }
        gameRunning = false;
      }

      if (gameRunning)
      {
        printf("Press any button to continue to the next round.\n");

        bool continuePressed = false;
        while (!continuePressed)
        {
          for (int i = 0; i < buttons.size(); i++)
          {
            if (buttons[i]->isPressed())
            {
              printf("Button %d pressed to continue!\n", i);
              continuePressed = true;

              while (buttons[i]->isPressed())
              {
                vTaskDelay(pdMS_TO_TICKS(10));
              }
              break;
            }
          }
          vTaskDelay(pdMS_TO_TICKS(50));
        }

        vTaskDelay(pdMS_TO_TICKS(500));
      }
    }

    printf("Game Over! Press any button to play again!\n");

    bool restartPressed = false;
    while (!restartPressed)
    {
      for (int i = 0; i < buttons.size(); i++)
      {
        if (buttons[i]->isPressed())
        {
          printf("Button %d pressed to restart!\n", i);
          restartPressed = true;

          while (buttons[i]->isPressed())
          {
            vTaskDelay(pdMS_TO_TICKS(10));
          }
          break;
        }
      }
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}
