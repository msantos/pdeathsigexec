PROG=   pdeathsigexec
SRCS=   pdeathsigexec.c

UNAME_SYS := $(shell uname -s)
ifeq ($(UNAME_SYS), Linux)
    CFLAGS ?= -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong \
              -Wformat -Werror=format-security \
              -fno-strict-aliasing
    LDFLAGS += -Wl,-z,relro,-z,now
	  PDEATHSIGEXEC ?= prctl
else ifeq ($(UNAME_SYS), OpenBSD)
    CFLAGS ?= -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong \
              -Wformat -Werror=format-security \
              -fno-strict-aliasing
    LDFLAGS += -Wno-missing-braces -Wl,-z,relro,-z,now
    PDEATHSIGEXEC ?= pledge
else ifeq ($(UNAME_SYS), FreeBSD)
    CFLAGS ?= -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong \
              -Wformat -Werror=format-security \
              -fno-strict-aliasing
    LDFLAGS += -Wno-missing-braces -Wl,-z,relro,-z,now
    PDEATHSIGEXEC ?= procctl
else ifeq ($(UNAME_SYS), Darwin)
    CFLAGS ?= -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong \
              -Wformat -Werror=format-security \
              -fno-strict-aliasing
    LDFLAGS += -Wno-missing-braces
endif

RM ?= rm

PDEATHSIGEXEC_CFLAGS ?= -g -Wall -Wextra -fwrapv -pedantic -pie -fPIE

CFLAGS += $(PDEATHSIGEXEC_CFLAGS) -DPDEATHSIGEXEC=\"$(PDEATHSIGEXEC)\" -DPDEATHSIGEXEC_$(PDEATHSIGEXEC)

LDFLAGS += $(PDEATHSIGEXEC_LDFLAGS)

.PHONY: all
all: $(PROG) libpdeathsigexec.so

$(PROG):
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LDFLAGS)

.PHONY: clean
clean:
	-@$(RM) $(PROG) *.so

.PHONY: test
test: $(PROG) libpdeathsigexec.so
	@env LD_LIBRARY_PATH=. PATH=.:$(PATH) bats test

libpdeathsigexec.so:
	$(CC) -Wall -Wextra -pedantic -D_GNU_SOURCE -nostartfiles -shared -fpic -fPIC \
		-DPDEATHSIGEXEC=\"$(PDEATHSIGEXEC)\" -DPDEATHSIGEXEC_$(PDEATHSIGEXEC) \
		-Wconversion -Wshadow \
		-Wpointer-arith -Wcast-qual \
		-Wstrict-prototypes -Wmissing-prototypes \
		-o libpdeathsigexec.so libpdeathsigexec.c -ldl \
		-Wl,-z,relro,-z,now -Wl,-z,noexecstack
