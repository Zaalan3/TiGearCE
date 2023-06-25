# ----------------------------
# Makefile Options
# ----------------------------

NAME = DEMO
ICON = icon.png
DESCRIPTION = "CE Toolchain Demo"
COMPRESSED = NO
ARCHIVED = NO
LTO = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

BSSHEAP_LOW = D031F6
BSSHEAP_HIGH = D03FFF 

# ----------------------------

include $(shell cedev-config --makefile)
