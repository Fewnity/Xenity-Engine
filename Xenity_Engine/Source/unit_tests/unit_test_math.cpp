#include "unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/tools/math.h>
#include <engine/vectors/vector3.h>

bool MathBasicTest::Start(std::string& errorOut)
{
	bool result = true;

	// ----------------- Clamp test
	const float clampValue0 = Math::Clamp(5.0f, -1.0f, 10.0f);
	const float clampValue1 = Math::Clamp(-2.0f, -1.0f, 10.0f);
	const float clampValue2 = Math::Clamp(15.0f, -1.0f, 10.0f);

	if (!Compare(clampValue0, 5.0f))
	{
		errorOut += "Bad Math Clamp\n";
		result = false;
	}

	if (!Compare(clampValue1, -1.0f))
	{
		errorOut += "Bad Math Clamp\n";
		result = false;
	}

	if (!Compare(clampValue2, 10.0f))
	{
		errorOut += "Bad Math Clamp\n";
		result = false;
	}

	// ----------------- Lerp test
	const float lerpValue0 = Math::Lerp(1.0f, 2.0f, 0.5f);
	const float lerpValue1 = Math::Lerp(1.0f, 2.0f, 1.0f);
	const float lerpValue2 = Math::Lerp(1.0f, 2.0f, 0.0f);

	if (!Compare(lerpValue0, 1.5f))
	{
		errorOut += "Bad Math Lerp\n";
		result = false;
	}

	if (!Compare(lerpValue1, 2.0f))
	{
		errorOut += "Bad Math Lerp\n";
		result = false;
	}

	if (!Compare(lerpValue2, 1.0f))
	{
		errorOut += "Bad Math Lerp\n";
		result = false;
	}

	const int nextPow2_0 = Math::nextPow2(256);
	const int nextPow2_1 = Math::nextPow2(255);
	const int nextPow2_2 = Math::nextPow2(257);

	if (!Compare(nextPow2_0, 256))
	{
		errorOut += "Bad Math NextPow2\n";
		result = false;
	}

	if (!Compare(nextPow2_1, 256))
	{
		errorOut += "Bad Math NextPow2\n";
		result = false;
	}

	if (!Compare(nextPow2_2, 512))
	{
		errorOut += "Bad Math NextPow2\n";
		result = false;
	}

	const int previousPow2_0 = Math::previousPow2(256);
	const int previousPow2_1 = Math::previousPow2(255);
	const int previousPow2_2 = Math::previousPow2(257);

	if (!Compare(previousPow2_0, 256))
	{
		errorOut += "Bad Math PreviousPow2\n";
		result = false;
	}

	if (!Compare(previousPow2_1, 128))
	{
		errorOut += "Bad Math PreviousPow2\n";
		result = false;
	}

	if (!Compare(previousPow2_2, 256))
	{
		errorOut += "Bad Math PreviousPow2\n";
		result = false;
	}

	return result;
}

bool MathMatrixTest::Start(std::string& errorOut)
{
	bool result = true;

	glm::mat4 simpleMatrix = Math::CreateModelMatrix(Vector3(1.0f, 2.0f, 3.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));

	glm::vec3 position = simpleMatrix[3];
	if (!Compare(position, glm::vec3(-1.0f,2.0f,3.0f)))
	{
		errorOut += "Bad Math CreateModelMatrix Position\n";
		result = false;
	}

	return result;
}