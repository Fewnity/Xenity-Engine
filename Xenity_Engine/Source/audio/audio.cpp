#include "audio.h"

ISoundEngine* Audio::engine;

int Audio::Init()
{
	engine = createIrrKlangDevice();

	if (!engine)
		return 1; // error starting up the engine

	return 0;
}