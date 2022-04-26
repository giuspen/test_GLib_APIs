.PHONY: all
FLAGS=$(shell pkg-config --libs --cflags gio-2.0 gio-unix-2.0 glib-2.0)

all: server client_add client_sub client_pingme

gen:
	gdbus-codegen --generate-c-code mydbus --c-namespace MyDBus --interface-prefix com.TestXmlInterface. com.TestXmlInterface.xml

%.o: %.c
	gcc -o $@ $^ -c $(FLAGS)

server: server.o mydbus.o 
	gcc -o $@ $^ $(FLAGS)

client_add: client_add.o mydbus.o 
	gcc -o $@ $^ $(FLAGS)

client_sub: client_sub.o mydbus.o 
	gcc -o $@ $^ $(FLAGS)

client_pingme: client_pingme.o mydbus.o 
	gcc -o $@ $^ $(FLAGS)

clean:
	rm -f *.o server client_add client_sub client_pingme
