// AudioCrypto.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>

#include <fstream>
#include <iterator>
#include <algorithm>

void MainMenu();

void EncryptFile();
void DecryptFile();
void DisplayHeader();

//#define DEBUG_SHOW_FILE_BYTES 256
//#define DEBUG_SHOW_SEED

#define RANDOM_SEED 133742069 // Could make this a timestamp or something else
//#define FIXED_XOR 1337 // Optional, use a fixed XOR char so that it's easier to encrypt and decrypt

void PrintLogo()
{
	std::cout << " ________  ___  ___  ________  ___  ________                          " << std::endl;
	std::cout << "|\\   __  \\|\\  \\|\\  \\|\\   ___ \\|\\  \\|\\   __  \\                         " << std::endl;
	std::cout << "\\ \\  \\|\\  \\ \\  \\\\\\  \\ \\  \\_|\\ \\ \\  \\ \\  \\|\\  \\                        " << std::endl;
	std::cout << " \\ \\   __  \\ \\  \\\\\\  \\ \\  \\ \\\\ \\ \\  \\ \\  \\\\\\  \\                       " << std::endl;
	std::cout << "  \\ \\  \\ \\  \\ \\  \\\\\\  \\ \\  \\_\\\\ \\ \\  \\ \\  \\\\\\  \\                      " << std::endl;
	std::cout << "   \\ \\__\\ \\__\\ \\_______\\ \\_______\\ \\__\\ \\_______\\                     " << std::endl;
	std::cout << "    \\|__|\\|__|\\|_______|\\|_______|\\|__|\\|_______|                     " << std::endl;
	std::cout << "" << std::endl;
	std::cout << " ________  ________      ___    ___ ________  _________  ________     " << std::endl;
	std::cout << "|\\   ____\\|\\   __  \\    |\\  \\  /  /|\\   __  \\|\\___   ___\\\\   __  \\    " << std::endl;
	std::cout << "\\ \\  \\___|\\ \\  \\|\\  \\   \\ \\  \\/  / | \\  \\|\\  \\|___ \\  \\_\\ \\  \\|\\  \\   " << std::endl;
	std::cout << " \\ \\  \\    \\ \\   _  _\\   \\ \\    / / \\ \\   ____\\   \\ \\  \\ \\ \\  \\\\\\  \\  " << std::endl;
	std::cout << "  \\ \\  \\____\\ \\  \\\\  \\|   \\/  /  /   \\ \\  \\___|    \\ \\  \\ \\ \\  \\\\\\  \\ " << std::endl;
	std::cout << "   \\ \\_______\\ \\__\\\\ _\\ __/  / /      \\ \\__\\        \\ \\__\\ \\ \\_______\\" << std::endl;
	std::cout << "    \\|_______|\\|__|\\|__|\\___/ /        \\|__|         \\|__|  \\|_______|" << std::endl;
	std::cout << "                       \\|___|/                                        " << std::endl;
}

int main()
{
	std::cout << "Welcome to..." << std::endl << std::endl;

	MainMenu();

	return 0;
}

void MainMenu()
{
	while (true)
	{
		PrintLogo();
		std::cout << std::endl;

		std::cout << "What would you like to do?" << std::endl;
		std::cout << "\t1: Encrypt audio" << std::endl;
		std::cout << "\t2: Decrypt audio" << std::endl;
		std::cout << "\t3: Display audio header" << std::endl;
		std::cout << "Alternatively, type 'exit' to exit." << std::endl;

		std::cout << std::endl << "Select option [1, 2 or 3]: ";
		std::string answer;
		std::cin >> answer;

		if (answer == "exit" || answer == "quit")
		{
			break;
		}
		else if (answer == "1")
		{
			EncryptFile();
		}
		else if (answer == "2")
		{
			DecryptFile();
		}
		else if (answer == "3")
		{
			DisplayHeader();
		}
		else
		{
			std::cout << "Ermm... Try again." << std::endl << std::endl;
		}

		system("pause");
	}

	std::cout << "Goodbye!" << std::endl;
}

std::filesystem::path SelectFile()
{
	// Requirements
	std::filesystem::path currentPath = std::filesystem::current_path();
	std::string extension = ".wav";

	// Fetch all music files in the current dir
	std::vector<std::filesystem::path> musicFiles;
	for (const auto& entry : std::filesystem::directory_iterator(currentPath))
	{
		const auto& fileName = entry.path().filename().string();
		if (fileName.size() > extension.size())
		{
			if (fileName.substr(fileName.size() - extension.size(), fileName.size()) == extension)
			{
				musicFiles.push_back(entry.path());
			}
		}
	}

	// Ask for a selection
	if (musicFiles.empty())
	{
		std::cout << "Uh oh... Seems like there are no files with the extension \"" << extension << "\"! Please fix this first.";
		return currentPath;
	}
	else if (musicFiles.size() > 1)
	{
		while (true)
		{
			std::cout << "Which file would you like to play?" << std::endl;

			for (uint32_t i = 0; i < musicFiles.size(); ++i)
			{
				std::cout << "\t" << std::to_string(i + 1) << ": " << musicFiles[i].filename() << std::endl;
			}

			std::string answer;
			std::cout << std::endl << "Please select which song you want (number): ";
			std::cin >> answer;

			try
			{
				int32_t intAnswer = std::stoi(answer) - 1;

				if (intAnswer < musicFiles.size())
				{
					std::cout << "Alright, " << musicFiles[intAnswer] << " it is!" << std::endl;
					return musicFiles[intAnswer];
				}
				else
				{
					std::cout << "No that's not possible... Try again!" << std::endl;
				}
			}
			catch (std::invalid_argument e)
			{
				std::cout << "Erm.. I asked for an integer number. Please try again." << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Auto-selected file " << musicFiles[0].filename() << ", since it's the only one available." << std::endl;
		return musicFiles[0];
	}
}

// WAV header
struct RIFFChunk
{
	char id[4];
	uint32_t size;
	char format[4];
};

struct FMTChunk
{
	uint32_t id;
	uint32_t size;
	uint16_t audioformat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
};

struct DataChunk
{
	char id[4];
	uint32_t size;
};

struct WAVHeader
{
	RIFFChunk riff;
	FMTChunk fmt;
	DataChunk data;
};

void PrintHeader(const std::string& acFileBuffer)
{
	WAVHeader header;
	memcpy(&header, &acFileBuffer[0], sizeof(WAVHeader));

	std::cout << "--- WAV HEADER ---" << std::endl;

	std::string tmp;

	// RIFF
	std::cout << "\tRIFF" << std::endl;
	tmp = header.riff.id;
	tmp.resize(sizeof(header.riff.id));
	std::cout << "\t\t" << "ID: "<< "\t\t" << tmp << std::endl;
	std::cout << "\t\t" << "Size: "<< "\t\t" << header.riff.size << std::endl;
	tmp = header.riff.format;
	tmp.resize(sizeof(header.riff.format));
	std::cout << "\t\t" << "Format: "<< "\t" << tmp << std::endl;

	// FMT
	std::cout << "\tFMT" << std::endl;
	std::cout << "\t\t" << "ID: "<< "\t\t" << header.fmt.id << std::endl;
	std::cout << "\t\t" << "Size: "<< "\t\t" << header.fmt.size << std::endl;
	std::cout << "\t\t" << "audioFormat: "<< "\t" << header.fmt.audioformat << std::endl;
	std::cout << "\t\t" << "numChannels: "<< "\t" << header.fmt.numChannels << std::endl;
	std::cout << "\t\t" << "sampleRate: "<< "\t" << header.fmt.sampleRate << std::endl;
	std::cout << "\t\t" << "byteRate: "<< "\t" << header.fmt.byteRate << std::endl;
	std::cout << "\t\t" << "blockAlign: "<< "\t" << header.fmt.blockAlign << std::endl;
	std::cout << "\t\t" << "bitsPerSample: "<< "\t" << header.fmt.bitsPerSample << std::endl;

	// Data
	std::cout << "\tData" << std::endl;
	tmp = header.data.id;
	tmp.resize(sizeof(header.data.id));
	std::cout << "\t\t" << "ID: "<< "\t\t" << tmp << std::endl;
	std::cout << "\t\t" << "Size: "<< "\t\t" << header.data.size << std::endl;
}

// Crypto
void EncryptFile()
{
	// Fetch file to use
	std::filesystem::path file;
	while (true)
	{
		file = SelectFile();
		if (file != std::filesystem::current_path())
		{
			break;
		}
	}

	// Fetch output file to use
	std::wstring outPathStr;
	outPathStr += file.parent_path().c_str();
#if _WIN32 || _WIN64
	outPathStr += L"\\";
#else
	outPathStr += L"/";
#endif
	outPathStr += L"encrypted_";
	outPathStr += file.filename().c_str();

	std::filesystem::path outputPath = std::filesystem::path(outPathStr);
	std::cout << "Will be saving to: " << outputPath.string().c_str() << std::endl;

	// Ready files
	std::ifstream inputFile(file.string().c_str(), std::ios::binary);
	std::string fileBytes;

	// Get file size
	std::streampos begin, end;
	begin = inputFile.tellg();
	inputFile.seekg(0, std::ios::end);
	end = inputFile.tellg();
	std::cout << "File size is: " << (end - begin) << " bytes.\n";

	// Read file data
	std::stringstream fileData;
	inputFile.seekg(0);
	fileData << inputFile.rdbuf();
	fileBytes = fileData.str();

	//WAVHeader header;
	//memcpy(&header, &fileBytes[0], sizeof(WAVHeader));
	
#ifdef DEBUG_SHOW_FILE_BYTES
	std::cout << "DEBUG: [PRE-ENCRYPTION] Showing first " << DEBUG_SHOW_FILE_BYTES << " bytes:" << std::endl;
	for (size_t i = 0; i < DEBUG_SHOW_FILE_BYTES; ++i)
	{
		std::cout << std::to_string(uint32_t(fileBytes[i])) << (i == (DEBUG_SHOW_FILE_BYTES - 1) ? "" : ", ");
	}
	std::cout << std::endl;
#endif

	// OTP here
	std::cout << "Encrypting file..." << std::endl;
	srand(RANDOM_SEED);

#ifdef DEBUG_SHOW_SEED
	std::cout << "Encryption seed:" << std::endl;
#endif

	PrintHeader(fileBytes);

	for (size_t i = sizeof(WAVHeader); i < fileBytes.size(); ++i)
	{
#ifdef FIXED_XOR
		char xorChar = char(FIXED_XOR);
#else
		char xorChar = char(rand());
#endif

#ifdef DEBUG_SHOW_SEED
		std::cout << std::to_string(int32_t(xorChar)) << " ";
#endif

		fileBytes[i] ^= xorChar;
	}

#ifdef DEBUG_SHOW_SEED
	std::cout << std::endl;
#endif

	// Save to file
	std::cout << "Saving encrypted file..." << std::endl;
	std::ofstream outputFile;
	outputFile.open(outputPath.string().c_str(), std::ios::binary | std::ios::trunc);
	for (size_t i = 0; i < fileBytes.size(); ++i)
	{
		outputFile << fileBytes[i];
	}
	outputFile.close();

#ifdef DEBUG_SHOW_FILE_BYTES
	std::cout << "DEBUG: [POST-ENCRYPTION] Showing first " << DEBUG_SHOW_FILE_BYTES << " bytes:" << std::endl;
	for (size_t i = 0; i < DEBUG_SHOW_FILE_BYTES; ++i)
	{
		std::cout << std::to_string(uint32_t(fileBytes[i])) << (i == (DEBUG_SHOW_FILE_BYTES - 1) ? "" : ", ");
	}
	std::cout << std::endl;
#endif

	std::cout << "Successfully encrypted " << file.filename() << " into " << outputPath.string().c_str() << "!" << std::endl;
}

void DecryptFile()
{
	// Fetch file to use
	std::filesystem::path file;
	while (true)
	{
		file = SelectFile();
		if (file != std::filesystem::current_path())
		{
			break;
		}
	}

	// Fetch output file to use
	std::wstring outPathStr;
	outPathStr += file.parent_path().c_str();
#if _WIN32 || _WIN64
	outPathStr += L"\\";
#else
	outPathStr += L"/";
#endif
	outPathStr += L"decrypted_";
	outPathStr += file.filename().c_str();

	std::filesystem::path outputPath = std::filesystem::path(outPathStr);
	std::cout << "Will be saving to: " << outputPath.string().c_str() << std::endl;

	// Ready files
	std::ifstream inputFile(file.string().c_str(), std::ios::binary);
	std::string fileBytes;

	// Get file size
	std::streampos begin, end;
	begin = inputFile.tellg();
	inputFile.seekg(0, std::ios::end);
	end = inputFile.tellg();
	std::cout << "File size is: " << (end - begin) << " bytes.\n";

	// Read file data
	std::stringstream fileData;
	inputFile.seekg(0);
	fileData << inputFile.rdbuf();
	fileBytes = fileData.str();

	// OTP here
	std::cout << "Decrypting file..." << std::endl;

	PrintHeader(fileBytes);

	srand(RANDOM_SEED);
	for (size_t i = sizeof(WAVHeader); i < fileBytes.size(); ++i)
	{
#ifdef FIXED_XOR
		fileBytes[i] ^= char(FIXED_XOR);
#else
		fileBytes[i] ^= char(rand());
#endif
	}

	// Save to file
	std::cout << "Saving decrypted file..." << std::endl;
	std::ofstream outputFile;
	outputFile.open(outputPath.string().c_str(), std::ios::binary | std::ios::trunc);
	for (size_t i = 0; i < fileBytes.size(); ++i)
	{
		outputFile << fileBytes[i];
	}
	outputFile.close();

#ifdef DEBUG_SHOW_FILE_BYTES
	std::cout << "DEBUG: [POST-DECRYPTION] Showing first " << DEBUG_SHOW_FILE_BYTES << " bytes:" << std::endl;
	for (size_t i = 0; i < DEBUG_SHOW_FILE_BYTES; ++i)
	{
		std::cout << std::to_string(uint32_t(fileBytes[i])) << (i == (DEBUG_SHOW_FILE_BYTES - 1) ? "" : ", ");
	}
	std::cout << std::endl;
#endif

	std::cout << "Successfully decrypted " << file.filename() << " into " << outputPath.string().c_str() << "!" << std::endl;
}

void DisplayHeader()
{
	// Fetch file to use
	std::filesystem::path file;
	while (true)
	{
		file = SelectFile();
		if (file != std::filesystem::current_path())
		{
			break;
		}
	}	

	// Open file reader
	std::ifstream inputFile(file.string().c_str(), std::ios::binary);
	std::string fileBytes;

	// Get file size
	std::streampos begin, end;
	begin = inputFile.tellg();
	inputFile.seekg(0, std::ios::end);
	end = inputFile.tellg();

	// Read file data
	std::stringstream fileData;
	inputFile.seekg(0);
	fileData << inputFile.rdbuf();
	fileBytes = fileData.str();

	// Print result
	PrintHeader(fileBytes);
}
