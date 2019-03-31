PKG_FOLDER = $(shell pwd -P)
CC = ccache clang 
CXX = ccache clang++ 
CPPFLAGS = -D_DEBUG 
CFLAGS = 
CXXFLAGS = -g -O0 -fPIC -fexceptions 
TARGET = lib/libvdf.so

OPENSSLLIB = $(shell pkg-config --libs openssl) 
LDFLAGS = $(OPENSSLLIB)
TEST_LDFLAGS = -Llib -lvdf $(OPENSSLLIB) -Wl,-rpath,$(PKG_FOLDER)/lib 

src = $(wildcard src/*.cpp)
objtmp = $(subst src/,obj/,$(src))
obj = $(objtmp:.cpp=.o)
testsrc = $(wildcard test/*.cpp)
testbin = $(testsrc:.cpp=.out)
testdep = $(testsrc:.cpp=.d)

.PHONY: all
all: $(TARGET) test

$(TARGET): $(obj)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS) 

obj/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

.PHONY: test
test: $(testbin) $(testdep)

test/%.d: test/%.cpp
	$(CXX) $(CXXLAGS) $< -MM -MT $(@:.d=.o) >$@

-include $(testdep)

test/%.out: test/%.cpp $(TARGET) test/%.d
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

.PHONY: cleandep
cleandep:
	rm -f $(testdep)

