CXX=clang++ -std=c++11 -Wno-write-strings -fvisibility-inlines-hidden -Wno-narrowing -Wno-int-to-void-pointer-cast -Wno-unsequenced -Wno-parentheses
WWISESDK=../SDK
CXXOPTFLAGS=-g -fsigned-char
CXXFLAGS=$(CXXOPTFLAGS) -I../enet/include -F ../Frameworks/ -I ../Frameworks/SDL2.framework/Headers -I../jpeg-9e -I$(WWISESDK)/include -I$(WWISESDK)/samples/SoundEngine/POSIX \
-I$(WWISESDK)/samples/SoundEngine/Common -rpath @executable_path/Frameworks
CFLAGS=-I../enet/include -DHAS_SOCKLEN_T

CLIENT_LIBS=-L/usr/local/lib -L$(WWISESDK)/Mac/Profile/lib \
	../jpeg-9e/libjpeg.a -lz -framework SDL2 -framework OpenGL -framework AudioToolbox -framework AudioUnit -framework CoreAudio -framework CoreFoundation \
	-lAkSoundEngine -lAkMusicEngine -lAkMemoryMgr -lAkStreamMgr -lCommunicationCentral \
	-lAkAACDecoder -lAkSoundSeedWindSource -lAkAudioInputSource -lAkMeterFX -lAkSoundSeedWooshSource \
	-lAkCompressorFX -lAkStereoDelayFX -lAkConvolutionReverbFX -lAkParametricEQFX \
	-lAkDelayFX -lAkPeakLimiterFX -lAkTimeStretchFX -lAkExpanderFX -lAkPitchShifterFX -lAkToneSource \
	-lAkFlangerFX -lAkRoomVerbFX -lAkTremoloFX -lAkGainFX -lAkSilenceSource -lAkVorbisDecoder \
	-lAkGuitarDistortionFX -lAkSineSource -lAkHarmonizerFX \
	-lAkMatrixReverbFX -lAkSoundSeedImpactFX -lAkRecorderFX -lAkSynthOneSource
		
CLIENT_OBJS= \
	client.o \
	clientextras.o \
	clientgame.o \
	clients2c.o \
	command.o \
	console.o \
	editing.o \
	entities.o \
	main.o \
	menus.o \
	monster.o \
	physics.o \
	rendercubes.o \
	renderextras.o \
	rendergl.o \
	rendermd2.o \
	renderparticles.o \
	rendertext.o \
	rndmap.o \
	savegamedemo.o \
	server.o \
	serverbrowser.o \
	serverms.o \
	serverutil.o \
	sound.o \
	tools.o \
	weapon.o \
	world.o \
	worldio.o \
	worldlight.o \
	worldocull.o \
	worldrender.o \
	$(WWISESDK)/samples/SoundEngine/Common/AkFileLocationBase.o \
	$(WWISESDK)/samples/SoundEngine/Common/AkMultipleFileLocation.o \
	$(WWISESDK)/samples/SoundEngine/POSIX/AkDefaultIOHookBlocking.o

ENET_OBJS= \
	../enet/callbacks.o \
	../enet/memory.o \
	../enet/protocol.o \
	../enet/host.o \
	../enet/packet.o \
	../enet/unix.o \
	../enet/list.o \
	../enet/peer.o \
	../enet/win32.o \

SERVER_LIBS=
SERVER_OBJS= \
	standalone.o \
	serverms.o \
	server.o \
	tools.o

default: client

all: client

clean:
	-rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(ENET_OBJS) cube_server ../../cube_game/cube_client
	-make -C ../enet/ clean

standalone.o:
	$(CXX) $(CXXFLAGS) -DSTANDALONE -o standalone.o -c serverutil.cpp

client: $(CLIENT_OBJS) $(ENET_OBJS) 
	$(CXX) $(CXXFLAGS) -o ../../cube_game/cube_client $(CLIENT_OBJS) $(ENET_OBJS) $(CLIENT_LIBS)
	cp -r ../Frameworks/SDL2.framework ../../cube_game/Frameworks

server: $(SERVER_OBJS) $(ENET_OBJS)
	$(CXX) $(CXXFLAGS) -o cube_server $(SERVER_OBJS) $(ENET_OBJS) $(SERVER_LIBS)  
	
