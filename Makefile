# Nome do executável final
TARGET = sobrevivendo_pif

# Compilador
CC = gcc

# Flags do compilador:
# -Wall -Wextra = Mostra todos os warnings 
# -std=c99 = Usa o padrão C99
# -I. = Inclui o diretório atual
# -Icli-lib/include = Diz ao GCC onde achar os headers da cli-lib
CFLAGS = -Wall -Wextra -std=c99 -I. -Icli-lib/include

# Bibliotecas necessárias para a cli-lib
# -lrt = real-time library
# -pthread = threading library
LIBS = -lrt -pthread

# Lista de todos os arquivos .c do jogo
SRC = main.c \
      jogador.c \
      tela.c \
      logica.c \
      fase1_forca.c \
      fase2_bomba.c \
      fase3_galaga.c

# Adiciona os arquivos .c da cli-lib que precisa ser compilado
SRC += cli-lib/src/keyboard.c \
       cli-lib/src/screen.c \
       cli-lib/src/timer.c

# Transforma a lista de .c em .o
OBJ = $(SRC:.c=.o)

# Regra principal: como criar o executável final
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

# Regra para compilar um .c em um .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos compilados
clean:
	rm -f $(OBJ) $(TARGET)