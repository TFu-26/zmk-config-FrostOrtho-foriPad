/* SPDX-License-Identifier: MIT */
#define DT_DRV_COMPAT primeorb_encoder
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include "encoder_decoder.h"

struct encoder_config {
    struct gpio_dt_spec a;
    struct gpio_dt_spec b;
    uint16_t detents;
};

struct encoder_data {
    const struct device *dev;
    struct gpio_callback callback;
    struct k_spinlock lock;
    struct primeorb_encoder_decoder decoder;
    int32_t pending;
    int32_t sample;
    sensor_trigger_handler_t handler;
    const struct sensor_trigger *trigger;
};

static int read_ab(const struct encoder_config *cfg) {
    gpio_port_value_t pins;
    int err = gpio_port_get_raw(cfg->a.port, &pins);
    if (err < 0) {
        return err;
    }
    return (!!(pins & BIT(cfg->a.pin)) << 1) | !!(pins & BIT(cfg->b.pin));
}

static void encoder_irq(const struct device *port, struct gpio_callback *cb, uint32_t pins) {
    ARG_UNUSED(port);
    ARG_UNUSED(pins);
    struct encoder_data *data = CONTAINER_OF(cb, struct encoder_data, callback);
    const struct encoder_config *cfg = data->dev->config;
    /* Capture A/B together in the IRQ, before ZMK's deferred sensor work.
     * Keep interrupts enabled; do not lose intermediate edges while queued.
     */
    k_spinlock_key_t key = k_spin_lock(&data->lock);
    int ab = read_ab(cfg);
    int step = 0;
    if (ab >= 0) {
        step = primeorb_encoder_update(&data->decoder, ab);
        data->pending += step;
    } else {
        data->decoder.synchronized = false;
        data->decoder.quarter_steps = 0;
    }
    sensor_trigger_handler_t handler = data->handler;
    const struct sensor_trigger *trigger = data->trigger;
    k_spin_unlock(&data->lock, key);
    if (step && handler) {
        handler(data->dev, trigger);
    }
}

static int encoder_fetch(const struct device *dev, enum sensor_channel chan) {
    if (chan != SENSOR_CHAN_ALL && chan != SENSOR_CHAN_ROTATION) {
        return -ENOTSUP;
    }
    struct encoder_data *data = dev->data;
    k_spinlock_key_t key = k_spin_lock(&data->lock);
    data->sample = data->pending;
    data->pending = 0;
    k_spin_unlock(&data->lock, key);
    return 0;
}

static int encoder_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *v) {
    if (chan != SENSOR_CHAN_ROTATION) {
        return -ENOTSUP;
    }
    const struct encoder_config *cfg = dev->config;
    struct encoder_data *data = dev->data;
    v->val1 = data->sample * (360 / cfg->detents);
    v->val2 = 0;
    return 0;
}

static int encoder_trigger(const struct device *dev, const struct sensor_trigger *trigger,
                           sensor_trigger_handler_t handler) {
    if (trigger->type != SENSOR_TRIG_DATA_READY || trigger->chan != SENSOR_CHAN_ROTATION) {
        return -ENOTSUP;
    }
    struct encoder_data *data = dev->data;
    k_spinlock_key_t key = k_spin_lock(&data->lock);
    data->trigger = trigger;
    data->handler = handler;
    k_spin_unlock(&data->lock, key);
    return 0;
}

static int encoder_init(const struct device *dev) {
    const struct encoder_config *cfg = dev->config;
    struct encoder_data *data = dev->data;
    data->dev = dev;
    if (!gpio_is_ready_dt(&cfg->a) || !gpio_is_ready_dt(&cfg->b)) {
        return -ENODEV;
    }
    if (cfg->a.port != cfg->b.port || cfg->a.pin == cfg->b.pin ||
        (cfg->a.dt_flags & GPIO_ACTIVE_LOW) || (cfg->b.dt_flags & GPIO_ACTIVE_LOW)) {
        return -EINVAL;
    }
    int err = gpio_pin_configure_dt(&cfg->a, GPIO_INPUT);
    if (err < 0) { return err; }
    err = gpio_pin_configure_dt(&cfg->b, GPIO_INPUT);
    if (err < 0) { return err; }
    int ab = read_ab(cfg);
    if (ab < 0) { return ab; }
    primeorb_encoder_reset(&data->decoder, ab);
    gpio_init_callback(&data->callback, encoder_irq, BIT(cfg->a.pin) | BIT(cfg->b.pin));
    err = gpio_add_callback(cfg->a.port, &data->callback);
    if (err < 0) { return err; }
    err = gpio_pin_interrupt_configure_dt(&cfg->a, GPIO_INT_EDGE_BOTH);
    if (err == 0) {
        err = gpio_pin_interrupt_configure_dt(&cfg->b, GPIO_INT_EDGE_BOTH);
    }
    if (err < 0) {
        gpio_pin_interrupt_configure_dt(&cfg->a, GPIO_INT_DISABLE);
        gpio_remove_callback(cfg->a.port, &data->callback);
    }
    return err;
}

static const struct sensor_driver_api encoder_api = {
    .sample_fetch = encoder_fetch,
    .channel_get = encoder_get,
    .trigger_set = encoder_trigger,
};

#define ENCODER_INST(n)                                                                             \
    static struct encoder_data encoder_data_##n;                                                    \
    static const struct encoder_config encoder_config_##n = {                                      \
        .a = GPIO_DT_SPEC_INST_GET(n, a_gpios),                                                     \
        .b = GPIO_DT_SPEC_INST_GET(n, b_gpios),                                                     \
        .detents = DT_INST_PROP(n, detents_per_rotation),                                           \
    };                                                                                             \
    DEVICE_DT_INST_DEFINE(n, encoder_init, NULL, &encoder_data_##n, &encoder_config_##n,              \
                          POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &encoder_api);
DT_INST_FOREACH_STATUS_OKAY(ENCODER_INST)
