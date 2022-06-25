
BUILD_DIR := build-linux
BUILD_CONFIG := Debug
#BUILD_CONFIG := Release

CMAKE_BUILD_CMD := cmake --build $(BUILD_DIR) --config $(BUILD_CONFIG) --target

app: cmake_setup
	$(CMAKE_BUILD_CMD) App

app/run: app
	./$(BUILD_DIR)/BL-Shaper-app/BL-Shaper

vst2: cmake_setup
	$(CMAKE_BUILD_CMD) VST2

vst3: cmake_setup
	$(CMAKE_BUILD_CMD) VST3

cmake_setup:
#cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_CONFIG) -DCMAKE_C_FLAGS="-g -fno-omit-frame-pointer" -D CMAKE_CXX_FLAGS="-g -fno-omit-frame-pointer"	

