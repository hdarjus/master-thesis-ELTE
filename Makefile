PKG_FOLDER = $(shell pwd -P)
LOCAL_FOLDER = ${HOME}/.local
CC = ccache gcc-8 
CXX = ccache g++-8 
CPPFLAGS = -I$(LOCAL_FOLDER)/include -std=c++17 # -D_DEBUG 
CFLAGS = 
CXXFLAGS = -g -O0 -fPIC -fexceptions 
TARGET = lib/libvdf.so

OPENSSL_LDFLAGS = -L$(LOCAL_FOLDER)/lib -lssl -lcrypto -Wl,-rpath=$(LOCAL_FOLDER)/lib 
PKG_LDFLAGS = -Llib -lvdf -Wl,-rpath=$(PKG_FOLDER)/lib 
GMP_LDFLAGS = -L$(LOCAL_FOLDER)/lib -lgmpxx -lgmp -Wl,-rpath=$(LOCAL_FOLDER)/lib 
MPFR_LDFLAGS = -L$(LOCAL_FOLDER)/lib -lmpfr -Wl,-rpath=$(LOCAL_FOLDER)/lib 
LDFLAGS = $(OPENSSL_LDFLAGS) $(GMP_LDFLAGS) $(MPFR_LDFLAGS) 
TEST_LDFLAGS = $(PKG_LDFLAGS) $(LDFLAGS) 

src = $(wildcard src/*.cpp)
objtmp = $(subst src/,obj/,$(src))
obj = $(objtmp:.cpp=.o)
testsrc = $(wildcard test/*.cpp)
testbin = $(testsrc:.cpp=.out)
testdep = $(testsrc:.cpp=.d)
timingsrc = $(wildcard timing_*.cpp)
timingbin = $(timingsrc:.cpp=.out)
timingdep = $(timingsrc:.cpp=.d)

.PHONY: all
all: $(TARGET) test timing

$(TARGET): $(obj)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS) 

obj/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

.PHONY: test
test: $(testbin) $(testdep)

test/%.d: test/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXLAGS) $< -MM -MT $(@:.d=.o) >$@

-include $(testdep)

test/%.out: test/%.cpp $(TARGET) test/%.d
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(TEST_LDFLAGS)

.PHONY: timing
timing: $(timingbin) $(timingdep)

./%.d: ./%.cpp
	$(CXX) $(CPPFLAGS) $(CXXLAGS) $< -MM -MT $(@:.d=.o) >$@

-include $(timingdep)

./%.out: ./%.cpp $(TARGET) ./%.d
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(TEST_LDFLAGS)

.PHONY: run
run: $(testbin)
	for testfile in $(testbin); do \
		echo $$testfile; \
	  ./$$testfile; \
	done

.PHONY: clean
clean:
	rm -f $(obj) $(TARGET) $(testbin) $(testdep) $(timingbin) $(timingdep) 

