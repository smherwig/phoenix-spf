# Compiler options
#----------------------------------------------------------
CC= gcc -std=gnu99

INCLUDES= -I $(HOME)/include

#CFLAGS= -Wall -Werror -DRHO_TRACE -DHO_DEBUG $(INCLUDES)
CFLAGS= -Wall -Werror $(INCLUDES)


# Utilities
#----------------------------------------------------------
RM= rm -f
MKDIR= mkdir -p
INSTALL= install -p

# Install Location
#----------------------------------------------------------
INSTALL_TOP= /usr/local
INSTALL_BIN= $(INSTALL_TOP)/bin

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

# Binaries to install
#----------------------------------------------------------
TO_BIN= spf

all: spf

install:
	$(MKDIR) $(INSTALL_BIN)
	$(INSTALL) $(TO_BIN) $(INSTALL_BIN)

uninstall:
	cd $(INSTALL_BIN) && $(RM) $(TO_BIN)

local:
	$(MAKE) install INSTALL_TOP=install

clean:
	$(RM) spf

echo:
	@echo "CC= $(CC)"
	@echo "CFLAGS= $(CFLAGS)"
	@echo "RM= $(RM)"
	@echo "MKDIR= $(MKDIR)"
	@echo "INSTALL= $(INSTALL)"
	@echo "TO_BIN= $(TO_BIN)"
	@echo "INSTALL_TOP= $(INSTALL_TOP)"
	@echo "INSTALL_BIN= $(INSTALL_BIN)"


# DO NOT DELETE

spf: spf.c
	$(CC) -o spf $(CFLAGS) spf.c $(HOME)/lib/librho.a


.PHONY: clean echo local install uninstall
