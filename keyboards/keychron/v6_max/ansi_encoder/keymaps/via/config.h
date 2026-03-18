#pragma once

#ifdef CUSTOM_CODE_ENABLE
#    ifdef RGB_MATRIX_ENABLE
         /* CUSTOM: Default to solid black — all LEDs off.
          * Indicator LEDs are painted by os_state_indicate() in custom_code.c. */
#        define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
#        define RGB_MATRIX_DEFAULT_HUE 0
#        define RGB_MATRIX_DEFAULT_SAT 0
#        define RGB_MATRIX_DEFAULT_VAL 0
#    endif
#endif
