PKG_FOLDER = /home/dhosszejni/Development/vdf
CC = ccache clang 
CXX = ccache clang++ 
CPPFLAGS = 
CFLAGS = 
CXXFLAGS = -g -O0 -fPIC -fexception 
TARGET = lib/libvdf.so

OPENSSLLIB = $(shell pkg-config --libs openssl) 
LDFLAGS = $(OPENSSLLIB)
TEST_LDFLAGS = -Llib -lvdf -Wl,-rpath,$(PKG_FOLDER)/lib 

src = $(wildcard src/*.cpp)
objtmp = $(subst src/,obj/,$(src))
obj = $(objtmp:.cpp=.o)
testsrc = $(wildcard test/*.cpp)
testbin = $(testsrc:.cpp=.out)

.PHONY: all
all: $(TARGET) test

$(TARGET): $(obj)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS) 

obj/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

## %.d: %.cpp
##    @$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: test
test: $(testbin)

test/test_openssl.out: test/test_openssl.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(OPENSSLLIB) 

test/%.out: test/%.cpp $(TARGET)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(TEST_LDFLAGS)

.PHONY: run
run: $(testbin)
	for testfile in $(testbin); do \
		echo $$testfile; \
	  ./$$testfile; \
	done

.PHONY: clean
clean:
	rm -f $(obj) $(TARGET) $(testbin)

#.PHONY: cleandep
#cleandep:
#  rm -f $(dep)

