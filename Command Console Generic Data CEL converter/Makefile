CC=g++
CFLAGS=--std=c++11 -Wall -Wextra
CDEBUGFLAGS=-g -ggdb
SOURCES=ColumnMetadata.cpp DataHeaderParameter.cpp DataSet.cpp DataGroup.cpp Command_Console_Generic_Data_CEL_converter.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DEBUGOBJECTS=$(SOURCES:.cpp=.do)
TARGET=ccgdCELc.exe
DEBUGTARGET=ccgdCELc_d.exe

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

debug:
	$(CC) $(CFLAGS) $(CDEBUGFLAGS) $(SOURCES) -o $(DEBUGTARGET)

clean:
	rm -rf *.o *.do $(TARGET) $(DEBUGTARGET)

