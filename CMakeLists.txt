# SPDX-License-Identifier: MIT
if(CONFIG_SHIELD_FROSTORTHO_R OR CONFIG_SHIELD_FROSTORTHO_L)
  target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/behaviors/behavior_wheel_step.c)
endif()
if(CONFIG_SHIELD_FROSTORTHO_L)
  target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/encoder_sensor.c)
endif()
