CC=gcc
CFLAGS=-IDUtils -IDUtilsCV -IDVision -DNDEBUG
CFLAGS+=$(shell pkg-config --cflags opencv) 
LFLAGS=-LDUtils -LDUtilsCV -LDVision $(shell pkg-config --libs opencv) \
  -lDVision -lDUtilsCV -lDUtils -lstdc++

DEPS=BowVector.h FClass.h FSurf64.h FBrief.h ScoringObject.h TemplatedVocabulary.h \
  TemplatedDatabase.h QueryResults.h  FeatureVector.h DBoW2.h Homography.h
  
OBJS=BowVector.o FSurf64.o FBrief.o ScoringObject.o QueryResults.o FeatureVector.o Homography.o

DEPS_SO=DUtils/libDUtils.so DUtilsCV/libDUtilsCV.so DVision/libDVision.so
TARGET=libDBoW2.so

all: $(TARGET)  demo


demo: $(OBJS) demo.o $(TARGET)
	$(CC) $^ $(LFLAGS) -O3 -o $@

demo.o: demo.cpp $(DEPS)
	$(CC) $(CFLAGS) -O3 -Wall -c $< -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -fPIC -O3 -Wall -c $< -o $@ 

$(TARGET): $(OBJS) $(DEPS_SO)
	$(CC) $(OBJS) $(LFLAGS) -shared -o $@

DUtils/libDUtils.so:
	make -C DUtils

DUtilsCV/libDUtilsCV.so:
	make -C DUtilsCVfeatures
	
DVision/libDVision.so:
	make -C DVision

clean:
	rm -f *.o $(TARGET) demo && \
	make -C DUtils clean && \
	make -C DUtilsCV clean && \
	make -C DVision clean && \
	rm -f small_db.yml.gz small_voc.yml.gz

install: $(TARGET)
	cp $(TARGET)  /usr/local/lib/ && \
	mkdir -p /usr/local/include/DBoW2 && \
	cp $(DEPS) /usr/local/include/DBoW2 && \
	make -C DUtils install && \
	make -C DUtilsCV install && \
	make -C DVision install

