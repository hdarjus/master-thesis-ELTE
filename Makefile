PKG_FOLDER = /home/dhosszejni/Development/vdf
CC = ccache clang 
CXX = ccache clang++ 
CPPFLAGS = -I $(PKG_FOLDER)/include
CFLAGS = 
CXXFLAGS = -g -O0 -fPIC 
TARGET = lib/libvdf.so

LDFLAGS = 
TEST_LDFLAGS = $(LDFLAGS) -Llib -lvdf -Wl,-rpath,$(PKG_FOLDER)/lib 

src = $(wildcard src/*.cpp)
trans_units = $(subst src/,,$(src))
obj = obj/$(trans_units:.cpp=.o)
testsrc = $(wildcard test/*.cpp)
testbin = $(testsrc:.cpp=.out)

.PHONY: all
all: $(TARGET) test

$(TARGET): $(obj)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared $(LDFLAGS) -o $@ $^

obj/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

## %.d: %.cpp
##    @$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: test
test: $(testbin)

test/%.out: test/%.cpp $(TARGET)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_LDFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(obj) $(TARGET)

#.PHONY: cleandep
#cleandep:
#  rm -f $(dep)

