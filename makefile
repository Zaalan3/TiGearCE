# ----------------------------
# Makefile Options
# ----------------------------

NAME = DEMO
ICON = icon.png
DESCRIPTION = "CE Toolchain Demo"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

BSSHEAP_LOW = D031F6
BSSHEAP_HIGH = D03FFF 

# ----------------------------

include $(shell cedev-config --makefile)
