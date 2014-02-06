CC=g++
CFLAGS=--std=c++11 -Wall -Wextra -O2
CDEBUGFLAGS=-g -ggdb
SOURCES=CELUtils.cpp ColumnMetadata.cpp DataHeaderParameter.cpp DataSet.cpp DataGroup.cpp Command_Console_Generic_Data_CEL_converter.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DEBUGOBJECTS=$(SOURCES:.cpp=.do)
TARGET=ccgdCELc.exe
DEBUGTARGET=ccgdCELc_d.exe

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(SOURCES) $(DEBUGTARGET)

$(DEBUGTARGET): $(DEBUGOBJECTS)
	$(CC) $(DEBUGOBJECTS) -o $@

.cpp.do:
	$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.do $(TARGET) $(DEBUGTARGET)

