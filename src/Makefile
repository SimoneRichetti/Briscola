OBJ = Briscola.o IA.o gestione_gioco.o grafica_debug.o
CXXFLAGS=-Wall
DEB = -D DEBUG_MODE
LEVEL1 = -D MASK=1 
LEVEL2 = -D MASK=3
LDFLAGS = `pkg-config --cflags --libs allegro-5.0 \
	allegro_dialog-5.0 allegro_primitives-5.0 allegro_font-5.0 \
	allegro_ttf-5.0 allegro_image-5.0` 

Briscola: $(OBJ)
	g++ $(OBJ) $(LDFLAGS) -o Briscola

debug1: CXXFLAGS +=$(DEB)
debug1: CXXFLAGS +=$(LEVEL1)
debug1: CXXFLAGS +=-g
debug1: $(OBJ)
	g++ $(OBJ) $(LDFLAGS) -o Briscola
	
debug2: CXXFLAGS +=$(DEB)
debug2: CXXFLAGS +=$(LEVEL2)
debug1: CXXFLAGS +=-g
debug2: $(OBJ)
	g++ $(OBJ) $(LDFLAGS) -o Briscola

-include dependencies.txt
	
.PHONY: depend clean cleanall 

depend:
	g++ -MM *.cc > dependencies.txt
clean:
	rm -f *.o
cleanall:
	rm -f Briscola *.o

