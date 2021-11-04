# debug or release
ver=debug

# decide compiling parameters by target platform
ifeq ($(ver),debug)
CXXFLAGS = -c -g -Ddebug
else
CXXFLAGS = -c -O3
endif

# building path settings
EXGC_SRC_PATH = src/
TEST_SRC_PATH = testbench/

OBJ_PATH=.object\$(ver)
EXGC_OBJ_PATH=$(OBJ_PATH)\ExGC
TEST_OBJ_PATH=$(OBJ_PATH)\Test
TARGET_PATH = .build\$(ver)
LIB_PATH = $(TARGET_PATH)\libs

# target .exe .o .lib files
EXE_NAME = cpplab
EXE_FILE = $(TARGET_PATH)/$(EXE_NAME).exe

EXGC_OBJ_FILES:=$(patsubst $(EXGC_SRC_PATH)/%.cpp, $(EXGC_OBJ_PATH)/%.o, $(wildcard $(EXGC_SRC_PATH)/*.cpp))
EXGC_HEADER_FILES:=$(wildcard $(EXGC_SRC_PATH)/*.h)
TEST_OBJ_FILES:=$(patsubst $(TEST_SRC_PATH)/%.cpp, $(TEST_OBJ_PATH)/%.o, $(wildcard $(TEST_SRC_PATH)/*.cpp))
TEST_HEADER_FILES:=$(wildcard $(TEST_SRC_PATH)/*.h)

OBJ_FILES:=$(EXGC_OBJ_FILES) $(TEST_OBJ_FILES)

LIB_FILES:=$(LIB_PATH)/exgc.lib

# default task, check directory and build .exe and .lib files
all:$(EXE_FILE) $(LIB_FILES)

$(EXE_FILE) : $(OBJ_FILES)
	@echo Linking Executable : $@
	@mkdir $(@D)||:
	@g++ $^ -o $@

$(EXGC_OBJ_PATH)/%.o : $(EXGC_SRC_PATH)/%.cpp $(EXGC_HEADER_FILES)
	@echo Building ExGC Object : $@
	@mkdir $(@D)||:
	@g++ $< $(CXXFLAGS) -o $@ 

$(TEST_OBJ_PATH)/%.o : $(TEST_SRC_PATH)/%.cpp $(TEST_HEADER_FILES)
	@echo Building Test Object : $@
	@mkdir $(@D)||:
	@g++ $< $(CXXFLAGS) -o $@ 

$(LIB_PATH)/exgc.lib : $(EXGC_OBJ_FILES)
	@echo Building LIB : $@ from $^
	@mkdir $(@D)||:
	@ar crv $@ $^ 


.PHONY : clean chkdir cleanexe cleanobj
clean : cleanlib cleanexe cleanobj
cleanexe:
	@del $(TARGET_PATH)\*.exe
cleanlib:
	@del $(LIB_PATH)\*.lib
cleanobj:
	@for /r $(OBJ_PATH) %%i in (*.o) do del /q %%i
	

