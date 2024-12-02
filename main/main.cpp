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

// Initialize LEDs and Buttons
Led led0(GPIO_NUM_13);
Led led1(GPIO_NUM_25);
Led led2(GPIO_NUM_27);
Led led3(GPIO_NUM_26);

Button button0(GPIO_NUM_15); // Button 0 for Difficulty 1
Button button1(GPIO_NUM_18); // Button 1 for Difficulty 2
Button button2(GPIO_NUM_19); // Button 2 for Difficulty 3
Button button3(GPIO_NUM_23); // Button 3 for Difficulty 4

std::vector<Led *> leds = {&led0, &led1, &led2, &led3};
std::vector<Button *> buttons = {&button0, &button1, &button2, &button3};

// Difficulty levels mapped to reaction times
const int difficultyTimes[] = {5000, 4000, 2000, 1000}; // From easiest to hardest

extern "C" void app_main(void)
{
  srand(time(NULL)); // Initialize random seed

  while (1) // Infinite loop for repeated gameplay
  {
    printf("Select Difficulty Level:\n");
    printf("Button 0: Easiest (5000 ms)\n");
    printf("Button 1: Easy (4000 ms)\n");
    printf("Button 2: Hard (2000 ms)\n");
    printf("Button 3: Hardest (1000 ms)\n");

    // Wait for a button press to select difficulty
    int selectedDifficulty = -1;
    while (selectedDifficulty == -1)
    {
      for (int i = 0; i < buttons.size(); i++)
      {
        if (buttons[i]->isPressed()) // If button is pressed, select the difficulty
        {
          selectedDifficulty = i;
          printf("Difficulty Level %d selected!\n", selectedDifficulty + 1);
          while (buttons[i]->isPressed()) // Wait until button is released
          {
            vTaskDelay(pdMS_TO_TICKS(10)); // Debounce delay
          }
          break;
        }
      }
      vTaskDelay(pdMS_TO_TICKS(50)); // Small delay to prevent CPU overload
    }

    int reactionTime = difficultyTimes[selectedDifficulty]; // Set the reaction time based on difficulty

    printf("Press any button to start the game!\n");

    // Wait for any button press to start
    bool startPressed = false;
    while (!startPressed)
    {
      for (int i = 0; i < buttons.size(); i++)
      {
        if (buttons[i]->isPressed())
        {
          printf("Button %d pressed to start!\n", i);
          startPressed = true;

          // Wait until all buttons are released before starting
          while (buttons[i]->isPressed())
          {
            vTaskDelay(pdMS_TO_TICKS(10)); // Debounce delay
          }
          break;
        }
      }
      vTaskDelay(pdMS_TO_TICKS(50)); // Small delay to prevent CPU overload
    }

    bool gameRunning = true;
    printf("Game Start!\n");

    while (gameRunning)
    {
      int rd_num = rand() % leds.size(); // Pick a random LED
      leds[rd_num]->on();                // Light up the selected LED
      printf("LED %d is ON\n", rd_num);

      TickType_t startTime = xTaskGetTickCount(); // Start timer
      bool correctButtonPressed = false;

      while (xTaskGetTickCount() - startTime < pdMS_TO_TICKS(reactionTime))
      {
        for (int i = 0; i < buttons.size(); i++)
        {
          if (buttons[i]->isPressed()) // Check if any button is pressed
          {
            if (i == rd_num) // Correct button pressed
            {
              correctButtonPressed = true;
              printf("Correct button %d pressed!\n", i);
              leds[rd_num]->off();                                                    // Turn off the LED
              reactionTime = (reactionTime > 200) ? reactionTime - 50 : reactionTime; // Decrease time
              break;
            }
            else // Wrong button pressed
            {
              printf("Wrong button %d pressed! You lost!\n", i);
              leds[rd_num]->off(); // Turn off the LED
              printf("Blinking LEDs...\n");
              for (int j = 0; j < 5; j++) // Blink LEDs 5 times
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
              gameRunning = false; // End game
              break;
            }
          }
        }
        if (!gameRunning || correctButtonPressed) // Break out of the loop if game ends
          break;
      }

      if (!correctButtonPressed && gameRunning) // Player failed to press the correct button in time
      {
        printf("You lost! Time ran out!\n");
        printf("Blinking LEDs...\n");
        for (int i = 0; i < 5; i++) // Blink LEDs 5 times
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
        gameRunning = false; // End game
      }

      if (gameRunning)
      {
        printf("Press any button to continue to the next round.\n");

        // Wait for any button press to continue
        bool continuePressed = false;
        while (!continuePressed)
        {
          for (int i = 0; i < buttons.size(); i++)
          {
            if (buttons[i]->isPressed())
            {
              printf("Button %d pressed to continue!\n", i);
              continuePressed = true;

              // Wait until all buttons are released before proceeding
              while (buttons[i]->isPressed())
              {
                vTaskDelay(pdMS_TO_TICKS(10)); // Debounce delay
              }
              break;
            }
          }
          vTaskDelay(pdMS_TO_TICKS(50)); // Small delay to prevent CPU overload
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Brief delay before next round
      }
    }

    printf("Game Over! Press any button to play again!\n");

    // Wait for any button press to restart
    bool restartPressed = false;
    while (!restartPressed)
    {
      for (int i = 0; i < buttons.size(); i++)
      {
        if (buttons[i]->isPressed())
        {
          printf("Button %d pressed to restart!\n", i);
          restartPressed = true;

          // Wait until all buttons are released before restarting
          while (buttons[i]->isPressed())
          {
            vTaskDelay(pdMS_TO_TICKS(10)); // Debounce delay
          }
          break;
        }
      }
      vTaskDelay(pdMS_TO_TICKS(50)); // Small delay to prevent CPU overload
    }
  }
}
