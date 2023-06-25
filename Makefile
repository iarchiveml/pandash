CC=clang
CFLAGS=-Wall -lreadline -lpthread
VPATH=utils
BUILDDIR=build
SRC=$(wildcard $(VPATH)/*.c)
MAIN=$(wildcard ./*.c)
OBJ=$(subst utils/,$(BUILDDIR)/, $(subst .c,.o, $(SRC))) $(subst ./,$(BUILDDIR)/, $(subst .c,.o, $(MAIN)))
BIN=bin
PROGRAM=pandash
TARGET=$(BIN)/$(PROGRAM)

all: pandash

$(BUILDDIR)/%.o: $(VPATH)/%.c
	mkdir -p $(BUILDDIR)
	$(CC) -c $< -o $@

$(BUILDDIR)/%.o: %.c
	mkdir -p $(BUILDDIR)
	$(CC) -c $< -o $@

pandash: $(OBJ)
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
	ldid -Kcert.p12 -Sent.xml bin/pandash

run:
	./$(TARGET)

clean:
	rm -fr $(BUILDDIR) $(BIN)
