#
#
#

CC = g++
CXX = g++

#CFLAGS = -Wall -O2 -fomit-frame-pointer
#CXXFLAGS = -Wall -O2 -fomit-frame-pointer `wx-config --cppflags`
CFLAGS = -Wall -O2 -g
CXXFLAGS = -Wall -O2 -g `wx-config --cppflags`

LIB = `wx-config --libs`

CSRC =  sim65_instructions.c disasm.c tables.c srecord.c
CXXSRC = SimApp.cpp SimFrame.cpp RegisterWindow.cpp StatusWindow.cpp Label.cpp StackWindow.cpp \
	 DisasmWindow.cpp MemWindow.cpp ByteRegister.cpp WordRegister.cpp Memory.cpp AddressDialog.cpp \
	 Core65c02.cpp BreakpointWindow.cpp Breakpoint.cpp BreakpointManager.cpp Prefs.cpp \
	 AddressPeripheral.cpp Uart.cpp Acia6551.cpp Acia6551Frame.cpp AppleTextFrame.cpp

COBJ =  sim65_instructions.o disasm.o tables.o srecord.o
CXXOBJ = SimApp.o SimFrame.o RegisterWindow.o StatusWindow.o Label.o StackWindow.o \
	 DisasmWindow.o MemWindow.o ByteRegister.o WordRegister.o Memory.o AddressDialog.o \
	 Core65c02.o BreakpointWindow.o Breakpoint.o BreakpointManager.o Prefs.o \
	 AddressPeripheral.o Uart.o Acia6551.o Acia6551Frame.o AppleTextFrame.o

default: sim65

objs: $(COBJ) $(CXXOBJ)

sim65: $(COBJ) $(CXXOBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(COBJ) $(CXXOBJ) -o sim65 $(LIB)

clean:
	rm -f *.o
	rm -f sim65

distclean: clean
	rm -f *~
	rm -f sim65.depend

sim65.depend:
	$(CC) -MM -MG $(CSRC) > sim65.depend
	$(CXX) -MM -MG `wx-config --cppflags` $(CXXSRC) >> sim65.depend

include sim65.depend
