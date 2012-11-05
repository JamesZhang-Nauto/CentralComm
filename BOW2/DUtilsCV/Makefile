CC=gcc
CFLAGS=-I../DUtils -DNDEBUG
LFLAGS=-L/usr/local/lib 

DEPS=DUtilsCV.h Drawing.h Geometry.h GUI.h IO.h Types.h Transformations.h
OBJS=Drawing.o Geometry.o GUI.o IO.o Types.o Transformations.o 
TARGET=libDUtilsCV.so




$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $^ -shared -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -fPIC -O3 -Wall -c $< -o $@ 

clean:
	rm -f *.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/lib/ && \
	mkdir -p /usr/local/include/DUtilsCV && \
	cp $(DEPS) /usr/local/include/DUtilsCV

