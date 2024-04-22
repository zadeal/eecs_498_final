#include "engineUtilities.h"

#include "../vendor/rapidjson/filereadstream.h"
#include "../vendor/rapidjson/document.h"
#include "../vendor/rapidjson/error/error.h"

#include <string>
#include <cstdio>
#include <cstdint>
#include <iostream>

const void EngineUtilities::readJsonFile(const std::string& filePath,
	rapidjson::Document& resultingDocument)
{
	FILE* filePointer = nullptr;

#ifdef _WIN32
	fopen_s(&filePointer, filePath.c_str(), "rb");
#else
	filePointer = fopen(filePath.c_str(), "rb");
#endif

	// Load the JSON file into the specified document.
	char buffer[65536];
	rapidjson::FileReadStream stream(filePointer, buffer, sizeof(buffer));
	resultingDocument.ParseStream(stream);
	std::fclose(filePointer);

	// Check the JSON file for parsing errors.
	if (resultingDocument.HasParseError())
	{
		rapidjson::ParseErrorCode errorCode =
			resultingDocument.GetParseError();
		std::cout << "error parsing json at [" << filePath << "]" << std::endl;

		std::exit(EXIT_SUCCESS);
	}
}

uint64_t EngineUtilities::createCompositeKey(int x, int y)
{
	// Cast to ensure the ints become exactly 32 bits in size.
	uint32_t ux = static_cast<uint32_t>(x);
	uint32_t uy = static_cast<uint32_t>(y);

	// Place x into right 32 bits.
	uint64_t result = static_cast<uint64_t>(ux);

	// Move x to left 32 bits.
	result = result << 32;

	// Place y into right 32 bits.
	result = result | static_cast<uint64_t>(uy);

	return result;
}

std::string EngineUtilities::obtainWordAfterPhrase(const std::string& input, const std::string& phrase)
{
	// Fnd the position of the phrase in the string
	size_t pos = input.find(phrase);
	// If phrase is not found, return an empty string
	if (pos == std::string::npos) return "";
	// Find the starting position of the next word (skip spaces after the phrase)
	pos += phrase.length();
	while (pos < input.size() && std::isspace(input[pos])) {
		++pos;
	}
	// If we're at the end of the string, return an empty string
	if (pos == input.size()) return "";
	// Find the end position of the word (until a space or the end of the string)
	size_t endPos = pos;
	while (endPos < input.size() && !std::isspace(input[endPos])) {
		++endPos;
	}

	// Extract and return the word
	return input.substr(pos, endPos - pos);
}
