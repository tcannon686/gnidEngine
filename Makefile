# Makefile wrapper

BUILD_DIR = build
CMAKE 	  = cmake
CD        = cd
RMRF      = rm -rf
MKDIR     = mkdir -p

all: ./build/Makefile
	$(CD)    $(BUILD_DIR) && $(CMAKE) --build .

install: all
	$(CD)    $(BUILD_DIR) && $(CMAKE) --install .

./build/Makefile: ./CMakeLists.txt
	$(MKDIR) $(BUILD_DIR)
	$(CD)    $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug ..

doc:
	doxygen Doxyfile

distclean:
	$(RMRF) $(BUILD_DIR)

