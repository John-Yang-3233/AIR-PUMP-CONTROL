#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#define SLEEP_TIME_MS   100
#define PWM_frequency   100000
#define PWM_PERIOD_NS   20000000
#define POWER_PERCENTAGY   50
#define PWM_DUTY_CYCLE    18000000

#define PWM_MOTOR_A    DT_ALIAS(motor0)
static const struct pwm_dt_spec motorA = PWM_DT_SPEC_GET(PWM_MOTOR_A); //pin 11 pwmA
#define PWM_MOTOR_B    DT_ALIAS(motor1)
static const struct pwm_dt_spec motorB = PWM_DT_SPEC_GET(PWM_MOTOR_B); //pin 13 pwmB

static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));


#define SW3_NODE	DT_ALIAS(sw3)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW3_NODE, gpios);
static struct gpio_callback button_cb_data;

static volatile bool phase = 0;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    phase = !phase; // Toggle phase

    if(phase){
        pwm_set_dt(&motorA, PWM_PERIOD_NS, PWM_DUTY_CYCLE);
		pwm_set_dt(&motorB, PWM_PERIOD_NS, 0);
        printk("motorA on\n");

    } else {
        pwm_set_dt(&motorB, PWM_PERIOD_NS, PWM_DUTY_CYCLE);
        pwm_set_dt(&motorA, PWM_PERIOD_NS, 0);
        printk("motorB on\n");
    }

}

int main(void)
{
    if (!pwm_is_ready_dt(&motorA)) {
        printk("Error: PWM device %s is not ready\n", motorA.dev->name);
        return -1;
    }

    if (!device_is_ready(button.port)) {
        printk("Error: Button device %s is not ready\n", button.port->name);
        return -1;
    }
	int ret = gpio_pin_configure(gpio_dev, 10, GPIO_OUTPUT );	//AIN2
	ret = gpio_pin_configure(gpio_dev, 12, GPIO_OUTPUT );//AIN1
	ret = gpio_pin_configure(gpio_dev, 14, GPIO_OUTPUT );//STBY
	ret = gpio_pin_configure(gpio_dev, 16, GPIO_OUTPUT );//BIN1
	ret = gpio_pin_configure(gpio_dev, 18, GPIO_OUTPUT );//BIN2
	ret = gpio_pin_set(gpio_dev,10,1);
	ret = gpio_pin_set(gpio_dev,12,0);
	ret = gpio_pin_set(gpio_dev,14,1);
	ret = gpio_pin_set(gpio_dev,16,1);
	ret = gpio_pin_set(gpio_dev,18,0);

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error %d: failed to configure interrupt\n", ret);
        return ret;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    while (1) {
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
