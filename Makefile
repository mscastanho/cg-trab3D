# Makefile

# make all: Compila os arquivos, gerando o executável trabalhocg
# make clean: Apaga o executável e todos os arquivos objetos

# Arquivos objetos que serão gerados implicitamente
myObjects = main.o objreader.o tinyxml2.o misc.o rect.o circle.o \
						bullet.o car.o input.o imageloader.o geometry.o

# Flags necessárias para a compilação
myFlags = -lglut -lGLU -lGL -lm

all: $(myObjects)
	g++ -o trabalhocg $(myObjects) $(myFlags)
	@echo "Executável 'trabalhocg' criado com sucesso."

.PHONY: clean
clean:
	-rm -f $(myObjects) trabalhocg
	@echo "Arquivos removidos com sucesso."
