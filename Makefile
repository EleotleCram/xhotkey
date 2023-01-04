SOURCES=src/xhotkey.cpp

bin/xhotkey: $(SOURCES) bin
	g++ -I include $(SOURCES) -o $@ -lX11 -lconfig++

bin:
	mkdir -p bin

clean:
	rm -f bin/xhotkey

