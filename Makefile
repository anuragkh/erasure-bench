CC := g++
SRCDIR := src
BUILDDIR := build
BINDIR := bin
LIBDIR := lib
INCDIR := include

TARGET_EN := $(BINDIR)/encoder
OBJ_EN := $(BUILDDIR)/Encoder.o
SRC_EN := $(SRCDIR)/Encoder.cpp

TARGET_DE := $(BINDIR)/decoder
OBJ_DE := $(BUILDDIR)/Decoder.o
SRC_DE := $(SRCDIR)/Decoder.cpp

CFLAGS := -O3 -g -Wall


LIB := -L $(LIBDIR) -llonghair
INC := -I $(INCDIR)
all: longhair encoder decoder

longhair:
	@echo "Building longhair library..."
	git submodule update --init
	mkdir -p $(LIBDIR)
	@echo "cd external/longhair; make; cp bin/* ../../lib/";\
		cd external/longhair; make; cp bin/* ../../lib/

encoder: $(TARGET_EN)

decoder: $(TARGET_DE)

$(TARGET_EN): $(OBJ_EN)
	@echo "Linking..."
	mkdir -p $(BINDIR)
	@echo "$(CC) $^ -o $(TARGET_EN) $(LIB)"; $(CC) $^ -o $(TARGET_EN) $(LIB)

$(TARGET_DE): $(OBJ_DE)
	@echo "Linking..."
	mkdir -p $(BINDIR)
	@echo "$(CC) $^ -o $(TARGET_DE) $(LIB)"; $(CC) $^ -o $(TARGET_DE) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning..."; 
	@echo "$(RM) -r $(BUILDDIR) $(TARGET_EN) $(TARGET_DE)"; $(RM) -r $(BUILDDIR) $(TARGET_EN) $(TARGET_DE)
	@echo "cd external/longhair; make clean"; cd external/longhair; make clean

.PHONY: clean
