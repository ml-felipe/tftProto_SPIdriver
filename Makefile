CC ?= cc
CPPFLAGS += -Iinclude
CFLAGS ?= -O2 -g
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic
LDLIBS += -lbcm2835
SRC = main.c config_and_spi.c print.c bmp.c
OBJ = $(SRC:.c=.o)
TARGET = tftDriver.exe
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@
-include $(OBJ:.o=.d)
run: $(TARGET)
	sudo ./$(TARGET)
TESTFLAGS = -std=c11 -g -Wall -Wextra -Wpedantic -Werror $(SANITIZE)
tests/test_bmp: tests/test_bmp.c bmp.c include/bmp.h
	$(CC) $(TESTFLAGS) -Iinclude tests/test_bmp.c bmp.c -o $@
tests/test_driver: tests/test_driver.c tests/mock_bcm2835.c config_and_spi.c print.c bmp.c $(wildcard include/*.h)
	$(CC) $(TESTFLAGS) -Itests -Iinclude $(filter %.c,$^) -o $@
test: tests/test_bmp tests/test_driver
	./tests/test_bmp
	./tests/test_driver
host-build:
	$(CC) $(TESTFLAGS) -Itests -Iinclude $(SRC) tests/mock_bcm2835.c -o tests/demo_host
analyze:
	@set -e; for source in $(SRC); do $(CC) $(TESTFLAGS) -fanalyzer -Itests -Iinclude -c $$source -o /dev/null; done
clean:
	rm -f $(OBJ) $(OBJ:.o=.d) $(TARGET) tests/test_bmp tests/test_driver tests/demo_host
.PHONY: all run test host-build analyze clean
