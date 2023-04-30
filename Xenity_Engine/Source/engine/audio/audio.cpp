#include "audio.h"
#include "../debug/debug.h"

ISoundEngine* Audio::engine = nullptr;

int Audio::Init()
{
	engine = createIrrKlangDevice();

	if (!engine)
		return 1; // error starting up the engine

	Debug::Print("---- Audio System initiated ----");

	return 0;
}