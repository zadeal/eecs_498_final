#pragma once

#include "../vendor/rapidjson/document.h"

#include <cstdint>
#include <string>

// This class contains various engine utility functions such as a JSON file
// loader.
class EngineUtilities
{
public:
	const static void readJsonFile(const std::string& filePath,
		rapidjson::Document& resultingDocument);

	// Converts two ints into an uint64_t for faster unordered_map keying.
	static uint64_t createCompositeKey(int x, int y);

	// Used for scene loading.
	static std::string obtainWordAfterPhrase(const std::string& input, const std::string& phrase);

};
