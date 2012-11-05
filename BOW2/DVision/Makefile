CC=gcc
CFLAGS=-I../DUtils -I../DUtilsCV -DNDEBUG
LFLAGS=-L/usr/local/lib 

DEPS=DVision.h SurfSet.h Matches.h PatchFile.h PLYFile.h PMVSCamera.h \
  PixelPointFile.h BundleCamera.h FSolver.h BRIEF.h
OBJS=SurfSet.o Matches.o PatchFile.o PLYFile.o PMVSCamera.o \
  PixelPointFile.o BundleCamera.o FSolver.o BRIEF.o
TARGET=libDVision.so


$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $^ -shared -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -fPIC -O3 -Wall -c $< -o $@ 

clean:
	rm -f *.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/lib/ && \
	mkdir -p /usr/local/include/DVision && \
	cp $(DEPS) /usr/local/include/DVision

