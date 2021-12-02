
# FLAGS      = -g -I /usr/local/Cellar/gmp/6.2.1/include -L /usr/local/Cellar/gmp/6.2.1/lib
#GMPSRC    = -g -ggdb

# use your own flags. 
GPP        = g++-9 -O2 -std=c++17 $(FLAGS) -isysroot "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"   
# GMPLIB     =
# GMPLIB    = -lgmp -lgmpxx

OBJECTS = InplaceQuickhullTest.o \
          ConvexHullQuickHull.o \
     	    ConvexHullInplaceQuickHull.o \
          Number.o \
          PointHandler.o \
          TimeMeasurement.o
        

all: InplaceQuickhullTest 

InplaceQuickhullTest: $(OBJECTS)
	$(GPP) -o $@ $^ $(GMPLIB)

InplaceQuickhullTest.o: InplaceQuickhullTest.cpp \
                  ConvexHullQuickHull.h \
                  PointHandler.h \
                  TimeMeasurement.h
	$(GPP) -o $@ -c $<

ConvexHullQuickHull.o: ConvexHullQuickHull.cpp \
                       ConvexHullQuickHull.h \
                       PointHandler.h
	$(GPP) -o $@ -c $<


ConvexHullInplaceQuickHull.o: ConvexHullInplaceQuickHull.cpp \
                       ConvexHullQuickHull.h \
                       PointHandler.h
	$(GPP) -o $@ -c $<

Number.o: Number.cpp \
          Number.h
	$(GPP) -o $@ -c $<

PointHandler.o: PointHandler.cpp \
                PointHandler.h \
                Number.h
	$(GPP) -o $@ -c $<

TimeMeasurement.o: TimeMeasurement.cpp \
                   TimeMeasurement.h
	$(GPP) -o $@ -c $<

clean:
	rm -f $(wildcard *.o) $(wildcard *.csv) InplaceQuickhullTest 

