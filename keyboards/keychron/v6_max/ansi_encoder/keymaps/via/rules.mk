VIA_ENABLE = yes

# CUSTOM: Enable custom code (key-spam toggles and future features)
# Comment out or set to "no" to produce stock firmware
CUSTOM_CODE_ENABLE = yes
ifeq ($(CUSTOM_CODE_ENABLE), yes)
    OPT_DEFS += -DCUSTOM_CODE_ENABLE
    SRC += custom_code.c
endif
