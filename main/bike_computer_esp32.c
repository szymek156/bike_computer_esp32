#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hello.h"

void app_main(void) {
    for (int i = 10; i >= 0; i--) {
        printf("cpp bike Restarting in %d seconds...\n", i);
        SayHello();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
