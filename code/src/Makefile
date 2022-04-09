CXX=g++ -m64
CXXFLAGS=-I ../include -O3 -Wall -std=c++11

APP_NAME=cacheSim
OUTDIR=output

default: $(APP_NAME)

.PHONY: dirs clean

dirs:
		/bin/mkdir -p $(OUTDIR)/

clean:
		/bin/rm -rf $(OUTDIR) $(APP_NAME)

OBJS=$(OUTDIR)/main.o $(OUTDIR)/cache.o 

$(APP_NAME): dirs $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -lm -lpthread

$(OUTDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@
