CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iheaders
LDFLAGS = -ldl

TARGET = build/cryptum

OBJECTS = build/obj/main.o \
          build/obj/menu.o \
          build/obj/cryptoWorker.o \
          build/obj/keyGenerator.o \
          build/obj/pluginLoader.o \
          build/obj/utilities.o

PLUGINS = plugins/libcaesarCipher.so \
          plugins/libxorCipher.so \
          plugins/libatbashCipher.so \
          plugins/librot128Cipher.so \
          plugins/libplayfairCipher.so \
          plugins/libchacha20Cipher.so

all: dirs $(TARGET) $(PLUGINS)

dirs:
	mkdir -p build/obj

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

build/obj/main.o: source/main.cpp
	$(CXX) $(CXXFLAGS) -c source/main.cpp -o build/obj/main.o

build/obj/menu.o: source/menu.cpp
	$(CXX) $(CXXFLAGS) -c source/menu.cpp -o build/obj/menu.o

build/obj/cryptoWorker.o: source/cryptoWorker.cpp
	$(CXX) $(CXXFLAGS) -c source/cryptoWorker.cpp -o build/obj/cryptoWorker.o

build/obj/keyGenerator.o: source/keyGenerator.cpp
	$(CXX) $(CXXFLAGS) -c source/keyGenerator.cpp -o build/obj/keyGenerator.o

build/obj/pluginLoader.o: source/pluginLoader.cpp
	$(CXX) $(CXXFLAGS) -c source/pluginLoader.cpp -o build/obj/pluginLoader.o

build/obj/utilities.o: source/utilities.cpp
	$(CXX) $(CXXFLAGS) -c source/utilities.cpp -o build/obj/utilities.o

plugins/libcaesarCipher.so: plugins/caesarCipher.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o plugins/libcaesarCipher.so plugins/caesarCipher.cpp

plugins/libxorCipher.so: plugins/xorCipher.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o plugins/libxorCipher.so plugins/xorCipher.cpp

plugins/libatbashCipher.so: plugins/atbashCipher.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o plugins/libatbashCipher.so plugins/atbashCipher.cpp

plugins/librot128Cipher.so: plugins/rot128Cipher.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o plugins/librot128Cipher.so plugins/rot128Cipher.cpp

plugins/libplayfairCipher.so: plugins/Playfair.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o plugins/libplayfairCipher.so plugins/Playfair.cpp

plugins/libchacha20Cipher.so: plugins/chacha20.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o plugins/libchacha20Cipher.so plugins/chacha20.cpp

run: all
	./build/cryptum

clean:
	rm -rf build
	rm -f plugins/*.so
