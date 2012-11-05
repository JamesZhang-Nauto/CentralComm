CC=gcc
CFLAGS=-DNDEBUG


DEPS=BinaryFile.h DUtils.h LineFile.h Random.h Timestamp.h DException.h \
  FileModes.h Math.hpp Profiler.h STL.h StringFunctions.h ConfigFile.h \
  TimeManager.h FileFunctions.h
OBJS=BinaryFile.o FileFunctions.o LineFile.o Profiler.o Random.o Timestamp.o \
  StringFunctions.o ConfigFile.o TimeManager.o
TARGET=libDUtils.so

$(TARGET): $(OBJS)
	$(CC) $^ -shared -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -fPIC -O3 -Wall -c $< -o $@ 

clean:
	rm -f *.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/lib/ && \
	mkdir -p /usr/local/include/DUtils && \
	cp $(DEPS) /usr/local/include/DUtils

