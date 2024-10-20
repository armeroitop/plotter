
##########################################
#### MACROS
##########################################

# $(1) : Compilador
# $(2) : Objeto file a generar
# $(3) : Codigo fuenteALL_C
# $(4) : Dependencias adicionales
# $(5) : Flags del compilador
define COMPILA
$(2) : $(3) $(4)
	$(1) -c -o $(2) $(3) $(5)
endef

# $(1) : Codigo fuente a cambiar a .o
define CTO
$(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(1)))
endef
#$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst $(SRC)%,$(OBJ)%,$(1))))

# $(1) : Codigo fuente a cambiar a .o
define CTH
$(patsubst %.c,%.hpp,$(patsubst %.cpp,%.hpp,$(1)))
endef

##########################################
#### CONFIG
##########################################

APP		:= game
C_FLAGS	:= -Wall -pedantic -std=c++17
CC_FLAGS:= $(C_FLAGS)
CC		:= g++
C		:= gcc
MKDIR	:= mkdir -p
SRC		:= src
OBJ		:= obj
LIBS_DIR	:= libs
LIBS 	:=  -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt
INC_DIRS := -I$(SRC) -I$(LIBS_DIR)
RM 		:= rm

ifdef DEBUG
	CC_FLAGS += -g
else
	CC_FLAGS += -O3
endif

SUB_DIRS		:= $(shell find $(SRC) -type d)
OBJSUB_DIRS	:= $(patsubst $(SRC)%,$(OBJ)%,$(SUB_DIRS))

ALL_C		:= $(shell find src/ -type f -iname *.c)
#ALL_CSOBJ	:= $(patsubst %.c,%.o,$(ALL_C))
#ALL_CSOBJ	:= $(foreach F,$(ALL_C),$(call CTO,$(F)))

ALL_CPP		:= $(shell find src/ -type f -iname *.cpp)
#ALL_CPPSOBJ	:= $(patsubst %.cpp,%.o,$(ALL_CPP))
#ALL_CPPSOBJ	:= $(foreach F,$(ALL_CPP),$(call CTO,$(F)))

ALL_OBJ		:= $(foreach F,$(ALL_C) $(ALL_CPP),$(call CTO,$(F)))

.PHONY: info clean cleanall libs libs-clean libs-cleanall


#Linkador
$(APP) : $(OBJSUB_DIRS) $(ALL_OBJ) 
	$(CC) -o $(APP) $(ALL_OBJ) $(LIBS)

#Genera las reglas para los objetos
$(foreach F,$(ALL_CPP),$(eval $(call COMPILA,$(CC),$(call CTO,$(F)),$(F),$(call CTH,$(F)),$(CC_FLAGS) $(INC_DIRS)))) 

$(foreach F,$(ALL_C),$(eval $(call COMPILA,$(C),$(call CTO,$(F)),$(F),$(call CTH,$(F)),$(C_FLAGS) $(INC_DIRS)))) 

#$(OBJ)/%.o : $(SRC)/%.cpp
#	$(CC) -o $(patsubst $(SRC)%,$(OBJ)%,$@) -c $^ $(CC_FLAGS)
#$(OBJ)/%.o : $(SRC)/%.c
#	$(C) -o $(patsubst $(SRC)%,$(OBJ)%,$@) -c $^ $(C_FLAGS)

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