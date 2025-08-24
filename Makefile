# Compiler & flags
CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude
LDFLAGS  := 
LDLIBS   := 

# Dirs
SRCDIR := src
INCDIR := include
BINDIR := bin

# Common sources used by all programs
COMMON_SRCS := $(SRCDIR)/DiskManager.cpp $(SRCDIR)/Page.cpp
COMMON_OBJS := $(COMMON_SRCS:.cpp=.o)

# Drivers
TABLE_SRC    := table.cpp
JOIN_SRC     := $(SRCDIR)/join.cpp
SELECT_SRC   := $(SRCDIR)/select.cpp
UNTABLEE_SRC := untablee.cpp

TABLE_BIN    := $(BINDIR)/table
JOIN_BIN     := $(BINDIR)/join
SELECT_BIN   := $(BINDIR)/select
UNTABLEE_BIN := $(BINDIR)/untablee

# Default target
all: dirs $(TABLE_BIN) $(JOIN_BIN) $(SELECT_BIN) $(UNTABLEE_BIN)

# Build rules
$(TABLE_BIN): $(TABLE_SRC) $(COMMON_OBJS) $(INCDIR)/Tables.hpp
	$(CXX) $(CXXFLAGS) $< $(COMMON_OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(JOIN_BIN): $(JOIN_SRC) $(COMMON_OBJS) $(INCDIR)/Tables.hpp
	$(CXX) $(CXXFLAGS) $< $(COMMON_OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(SELECT_BIN): $(SELECT_SRC) $(COMMON_OBJS) $(INCDIR)/Tables.hpp
	$(CXX) $(CXXFLAGS) $< $(COMMON_OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(UNTABLEE_BIN): $(UNTABLEE_SRC) $(COMMON_OBJS) $(INCDIR)/Tables.hpp
	$(CXX) $(CXXFLAGS) $< $(COMMON_OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

# Compile common sources
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure bin dir exists
dirs:
	@mkdir -p $(BINDIR)

# Convenience run targets
run-table: $(TABLE_BIN)
	./$(TABLE_BIN)

run-join: $(JOIN_BIN)
	./$(JOIN_BIN)

run-select: $(SELECT_BIN)
	./$(SELECT_BIN)

run-untablee: $(UNTABLEE_BIN)
	./$(UNTABLEE_BIN)

# Clean
clean:
	@rm -f $(SRCDIR)/*.o
	@rm -f $(BINDIR)/*

.PHONY: all dirs clean run-table run-join run-select run-untablee
