/* SPDX-License-Identifier: MIT */
#define DT_DRV_COMPAT primeorb_behavior_wheel_step
#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>

static int pressed(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    ARG_UNUSED(event);
    if (binding->param1 > 1) {
        return -EINVAL;
    }
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    return input_report_rel(dev, INPUT_REL_WHEEL, binding->param1 ? 2 : -2, true, K_NO_WAIT);
}

static int released(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return 0;
}

static const struct behavior_driver_api wheel_api = {
    .binding_pressed = pressed,
    .binding_released = released,
};

#define WHEEL_INST(n)                                                                               \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                  \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &wheel_api);
DT_INST_FOREACH_STATUS_OKAY(WHEEL_INST)
