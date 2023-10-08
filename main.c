#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"

const uint start_pin = 2;
const uint stop_pin = 3;
uint64_t start_time, stop_time;

// Function to run on Core 0
void core0_entry() {
    gpio_init(start_pin);
    gpio_set_dir(start_pin, GPIO_IN);
    gpio_init(stop_pin);
    gpio_set_dir(stop_pin, GPIO_IN);

    while (true) {
        if (gpio_get(start_pin)) {
            start_time = time_us_64();
        }
        if (gpio_get(stop_pin)) {
            stop_time = time_us_64();
        }
        sleep_ms(1);
    }
}

// Function to run on Core 1
void core1_entry() {
    stdio_init_all();
    uart_init(uart0, 115200);

    while (true) {
        if (start_time && stop_time) {
            uint64_t time_diff = stop_time - start_time;
            char buffer[50];
            snprintf(buffer, sizeof(buffer), "Time Difference: %llu microseconds\n", time_diff);
            uart_puts(uart0, buffer);

            // Reset times
            start_time = 0;
            stop_time = 0;
        }
        sleep_ms(10);
    }
}

int main() {
    // Launch core0_entry() on core 0
    multicore_launch_core1(core0_entry);
    // Run core1_entry() on core 1
    core1_entry();

    return 0;
}
