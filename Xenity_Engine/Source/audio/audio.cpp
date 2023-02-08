#include "audio.h"
#include "../debug.h"

ISoundEngine* Audio::engine = nullptr;

int Audio::Init()
{
	engine = createIrrKlangDevice();

	if (!engine)
		return 1; // error starting up the engine

	Debug::Print("---- Audio system initiated ----");

	return 0;
}