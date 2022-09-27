all: RayCasting MapEditor
	
RayCasting:
	gcc source.c -o RayCasting.exe -Wall -W -Werror -lSDL2main -lSDL2 -lm
	./RayCasting.exe

MapEditor:
	gcc editorSource.c -o MapEditor.exe -Wall -W -Werror -lSDL2main -lSDL2
	./MapEditor.exe