
##########################################
#### MACROS
##########################################

# Macro para compilar un código fuente en un archivo objeto.
#
# Esta macro se utiliza para compilar un archivo fuente especificado
# en un archivo objeto, tomando en cuenta las dependencias y los flags 
# del compilador proporcionados.
#
# Parámetros:
#   $(1) : Compilador a utilizar (por ejemplo, gcc).
#   $(2) : Nombre del archivo objeto a generar (por ejemplo, archivo.o).
#   $(3) : Código fuente a compilar (por ejemplo, archivo.c).
#   $(4) : Dependencias adicionales requeridas para la compilación 
#           (pueden ser otros archivos objeto o bibliotecas).
#   $(5) : Flags del compilador (opciones adicionales, como -Wall, -O2, etc.).
#
# Ejemplo de uso:
#   $(call COMPILA, gcc, archivo.o, archivo.c, dependencias.o, -Wall -O2)
# Esto generará el archivo objeto 'archivo.o' a partir del código 
# fuente 'archivo.c', utilizando el compilador 'gcc', y considerando 
# las dependencias y flags especificados.
#
define COMPILA
$(2) : $(3) $(4)
	$(1) -c -o $(2) $(3) $(5)
endef

# Macro para cambiar la extensión de un código fuente a archivo objeto (.o).
#
# Esta macro toma un nombre de archivo de código fuente (tanto .c como .cpp)
# y lo transforma en su correspondiente archivo objeto (.o) en el directorio
# de objetos especificado.
#
# Parámetros:
#   $(1) : Nombre del archivo de código fuente a transformar. Puede ser 
#          un archivo con extensión .c o .cpp.
#
# Detalles de la transformación:
#   - Si el archivo de entrada es de tipo .c, la macro lo transformará 
#     a .o en el directorio definido por la variable OBJ.
#   - Si el archivo de entrada es de tipo .cpp, la macro también lo 
#     transformará a .o en el directorio definido por la variable OBJ.
#
# Ejemplo de uso:
#   $(call CTO, src/mi_archivo.c)
#   Esto generará 'obj/mi_archivo.o' si las variables SRC y OBJ 
#   están definidas como 'src' y 'obj', respectivamente.
#
define CTO
$(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(1)))
endef

# Macro para cambiar la extensión de archivos de código fuente a archivos de encabezado (.hpp).
#
# Esta macro toma un nombre de archivo de código fuente (tanto .c como .cpp)
# y lo transforma en su correspondiente archivo de encabezado (.hpp).
#
# Parámetros:
#   $(1) : Nombre del archivo de código fuente a transformar. Puede ser 
#          un archivo con extensión .c o .cpp.
#
# Detalles de la transformación:
#   - Si el archivo de entrada es de tipo .c, la macro lo transformará 
#     a .hpp.
#   - Si el archivo de entrada es de tipo .cpp, la macro también lo 
#     transformará a .hpp.
#
# Ejemplo de uso:
#   $(call CTH, mi_archivo.c)
#   Esto generará 'mi_archivo.hpp'.
#   
#   $(call CTH, mi_archivo.cpp)
#   Esto también generará 'mi_archivo.hpp'.
#
define CTH
$(patsubst %.c,%.hpp,$(patsubst %.cpp,%.hpp,$(1)))
endef

##########################################
#### CONFIG
##########################################

APP			:= game
C_FLAGS		:= -Wall -pedantic -std=c++17
CC_FLAGS	:= $(C_FLAGS)
CC			:= g++
C			:= gcc
MKDIR		:= mkdir -p
SRC			:= src
OBJ			:= obj
LIBS_DIR	:= libs
LIBS 		:= -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt 
INC_DIRS 	:= -I$(SRC) -I$(LIBS_DIR)
RM 			:= rm

#Usa lasiguiente expresión para compilar con DEBUG y poder hacer depuración
#make DEBUG=1
ifdef DEBUG
	CC_FLAGS += -g
else
	CC_FLAGS += -O3
endif

# Definición de la variable SUB_DIRS.
#
# Esta variable captura todos los subdirectorios dentro del directorio
# especificado por la variable $(SRC) utilizando el comando 'find'.
#
# Comando utilizado:
#   - $(shell find $(SRC) -type d) : Este comando busca y lista todos
#     los subdirectorios (indicado por -type d) dentro del directorio
#     especificado en $(SRC).
#
# Ejemplo de uso:
#   Si $(SRC) es 'src', la variable $(SUB_DIRS) contendrá todos los
#   subdirectorios dentro de 'src', como por ejemplo:
#   'src/dir1', 'src/dir2', 'src/dir1/dir3', etc.
#
# Notas:
#   - Asegúrate de que la variable $(SRC) esté definida y sea un
#     directorio existente para evitar errores al ejecutar el Makefile.
#
SUB_DIRS		:= $(shell find $(SRC) -type d)

# Definición de la variable OBJSUB_DIRS.
#
# Esta variable transforma la lista de subdirectorios obtenida en
# $(SUB_DIRS) para que apunten a la ubicación correspondiente en
# el directorio de objetos, reemplazando la parte del directorio 
# fuente (especificado por $(SRC)) por el directorio de objetos 
# (especificado por $(OBJ)).
#
# Comando utilizado:
#   - $(patsubst $(SRC)%, $(OBJ)%, $(SUB_DIRS)) : Esta función
#     realiza la sustitución de patrones, donde:
#       - $(SRC)% : Coincide con cualquier subdirectorio dentro de 
#         $(SRC).
#       - $(OBJ)% : Reemplaza la parte de $(SRC) con $(OBJ),
#         creando así la ruta correspondiente en el directorio
#         de objetos.
#
# Ejemplo de uso:
#   Si $(SRC) es 'src/' y $(SUB_DIRS) contiene 'src/dir1', 
#   'src/dir2', la variable $(OBJSUB_DIRS) resultará en:
#   'obj/dir1', 'obj/dir2'.
#
# Notas:
#   - Asegúrate de que las variables $(SRC) y $(OBJ) estén
#     definidas y que correspondan a los directorios existentes
#     para evitar errores.
#
OBJSUB_DIRS	:= $(patsubst $(SRC)%,$(OBJ)%,$(SUB_DIRS))

# Definición de la variable ALL_C.
#
# Esta variable captura todos los archivos de código fuente con
# extensión .c en el directorio 'src/' y sus subdirectorios.
#
# Comando utilizado:
#   - $(shell find src/ -type f -iname *.c) : Este comando utiliza
#     'find' para buscar archivos:
#       - 'src/' : Directorio en el que se realiza la búsqueda.
#       - '-type f' : Filtra los resultados para que solo incluya
#         archivos regulares.
#       - '-iname *.c' : Busca archivos que tengan una extensión
#         .c, sin tener en cuenta mayúsculas y minúsculas (por
#         lo que también coincidirá con archivos como '.C').
#
# Ejemplo de uso:
#   Si hay archivos como 'src/archivo1.c', 'src/dir/archivo2.C',
#   la variable $(ALL_C) contendrá:
#   'src/archivo1.c src/dir/archivo2.C'.
#
# Notas:
#   - Asegúrate de que el directorio 'src/' exista y contenga
#     archivos .c para que la variable se defina correctamente.
#
ALL_C		:= $(shell find src/ -type f -iname *.c)

ALL_CPP		:= $(shell find src/ -type f -iname *.cpp)

ALL_OBJ		:= $(foreach F,$(ALL_C) $(ALL_CPP),$(call CTO,$(F)))

.PHONY: info clean cleanall libs libs-clean libs-cleanall


#Linkador
$(APP) : $(OBJSUB_DIRS) $(ALL_OBJ) 
	$(CC) -o $(APP) $(ALL_OBJ) $(LIBS)

#Genera las reglas para los objetos
$(foreach F,$(ALL_CPP),$(eval $(call COMPILA,$(CC),$(call CTO,$(F)),$(F),$(call CTH,$(F)),$(CC_FLAGS) $(INC_DIRS)))) 

$(foreach F,$(ALL_C),$(eval $(call COMPILA,$(C),$(call CTO,$(F)),$(F),$(call CTH,$(F)),$(C_FLAGS) $(INC_DIRS)))) 



# Regla para crear los subdirectorios donde se almacenarán los archivos objeto.
#
# Esta regla verifica la existencia de los subdirectorios especificados
# en la variable $(OBJSUB_DIRS) y los crea si no existen.
#
# Dependencias:
#   - $(OBJSUB_DIRS) : Lista de subdirectorios a crear. 
#
# Comando:
#   - $(MKDIR) : Comando para crear directorios (normalmente 'mkdir' en sistemas Unix).
#
# Ejemplo de uso:
#   Si $(OBJSUB_DIRS) contiene 'obj/dir1 obj/dir2', esta regla ejecutará:
#   mkdir obj/dir1 obj/dir2
#
$(OBJSUB_DIRS) :
	$(MKDIR) $(OBJSUB_DIRS)

info :
	$(info $(SUB_DIRS))
	$(info $(OBJSUB_DIRS))
	$(info $(ALL_CPP))
	$(info $(ALL_C))
	$(info $(ALL_OBJ))

clean:
	$(RM) -r ./$(OBJ)

cleanall: clean
	$(RM) "./$(APP)"


# LIBS Rules
libs:
	$(MAKE) -C $(LIBS_DIR)

libs-clean:
	$(MAKE) -C $(LIBS_DIR) clean

libs-cleanall:
	$(MAKE) -C $(LIBS_DIR) cleanall