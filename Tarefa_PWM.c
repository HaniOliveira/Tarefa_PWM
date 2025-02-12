#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#define SERVOMOTOR_PIN 22
#define PWM_DIVISER 100.0
#define PWM_WRAP 25000

const uint dc_max = PWM_WRAP * 0.12;  // duty cycle de 12%
const uint dc_min = PWM_WRAP * 0.025; // duty cycle de 2.5%
const uint step = PWM_WRAP * 0.00025; // step de incremento de 5us no ciclo ativo

void init_gpio()
{
    /** initialize pwm pin */
    gpio_set_function(SERVOMOTOR_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(SERVOMOTOR_PIN);

    pwm_set_clkdiv(slice, PWM_DIVISER);

    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_gpio_level(SERVOMOTOR_PIN, 0);

    pwm_set_enabled(slice, true);
}

int main()
{
    bool ok, initial_routine = true, is_up = true;
    uint dc = 0;

    // set clock freq to 125MHz
    ok = set_sys_clock_khz(125000, false);
    if (ok)
        printf("clock set to %ld\n", clock_get_hz(clk_sys));

    // init gpios and stdio functions
    // stdio_init_all();
    init_gpio();
    while (1)
    {
        if (initial_routine)
        {
            initial_routine = false;
            pwm_set_gpio_level(SERVOMOTOR_PIN, dc_max); // duty cycle no limite superior
            sleep_ms(5000);
            pwm_set_gpio_level(SERVOMOTOR_PIN, PWM_WRAP * 0.0735); // duty cycle em valor intermediário
            sleep_ms(5000);
            pwm_set_gpio_level(SERVOMOTOR_PIN, dc_min); // duty cycle no limite inferior
            sleep_ms(5000);
        }
        if (is_up)
        {
            dc += step;
            if (dc >= dc_max)
                is_up = false;
        }
        else
        {
            dc -= step;
            if (dc <= dc_min)
                is_up = true;
        }
        pwm_set_gpio_level(SERVOMOTOR_PIN, dc); // muda duty cycle do pwm
        sleep_ms(10);
    }

    return 0;
}