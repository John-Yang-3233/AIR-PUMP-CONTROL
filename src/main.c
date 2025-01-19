#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#define AIN2_PIN 10
#define AIN1_PIN 12
#define STBY_PIN 14
#define BIN1_PIN 16
#define BIN2_PIN 18

#define SLEEP_TIME_MS   100
#define PWM_frequency   100000
#define PWM_PERIOD_NS   10000000
#define PWM_DUTY_CYCLE   90
#define PWM_PULSE    9000000

#define PWM_MOTOR_A    DT_ALIAS(motor0)
static const struct pwm_dt_spec motorA = PWM_DT_SPEC_GET(PWM_MOTOR_A); //pin 11 pwmA
#define PWM_MOTOR_B    DT_ALIAS(motor1)
static const struct pwm_dt_spec motorB = PWM_DT_SPEC_GET(PWM_MOTOR_B); //pin 13 pwmB

static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));


#define SW3_NODE	DT_ALIAS(sw3)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW3_NODE, gpios);
static struct gpio_callback button_cb_data;

int phase = 0;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    phase += 1 ;
    if(phase>2){
        phase = 0 ;
    }
    if(phase = 1){
        pwm_set_dt(&motorA, PWM_PERIOD_NS, PWM_PLUSE);
		pwm_set_dt(&motorB, PWM_PERIOD_NS, 0);
        printk("motorA on\n");

    } else if(phase = 2){
        pwm_set_dt(&motorB, PWM_PERIOD_NS, PWM_PLUSE);
        pwm_set_dt(&motorA, PWM_PERIOD_NS, 0);
        printk("motorB on\n");
    } else if (phase = 0){
        pwm_set_dt(&motorA, PWM_PERIOD_NS, 0);
		pwm_set_dt(&motorB, PWM_PERIOD_NS, 0);
        printk("all motor off\n");
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
	int ret = gpio_pin_configure(gpio_dev, AIN2_PIN, GPIO_OUTPUT );	//AIN2
	ret = gpio_pin_configure(gpio_dev, AIN1_PIN, GPIO_OUTPUT );//AIN1
	ret = gpio_pin_configure(gpio_dev, STBY_PIN, GPIO_OUTPUT );//STBY
	ret = gpio_pin_configure(gpio_dev, BIN1_PIN, GPIO_OUTPUT );//BIN1
	ret = gpio_pin_configure(gpio_dev, BIN2_PIN, GPIO_OUTPUT );//BIN2
	ret = gpio_pin_set(gpio_dev,AIN2_PIN,1);
	ret = gpio_pin_set(gpio_dev,AIN1_PIN,0);
	ret = gpio_pin_set(gpio_dev,STBY_PIN,1);
	ret = gpio_pin_set(gpio_dev,BIN1_PIN,1);
	ret = gpio_pin_set(gpio_dev,BIN2_PIN,0);

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
