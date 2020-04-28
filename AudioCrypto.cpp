/*
	AUDIO CRYPTO
		WAV encrypter and decrypter
		Made by f13rce

	Compile using C++17 or later
*/

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
void EncodeAudio();
void DecodeAudio();

//#define DEBUG_SHOW_FILE_BYTES 256
//#define DEBUG_SHOW_SEED

#define RANDOM_SEED 133742069 // Could make this a timestamp or something else
//#define FIXED_XOR 1337 // Optional, use a fixed XOR char so that it's easier to encrypt and decrypt

// Encoding
#define BITS_PER_BYTE 8
#define LENGTH_PER_BIT 512
#define BYTES_PER_SAMPLE 8192

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
		std::cout << "\t4: Encode audio" << std::endl;
		std::cout << "\t5: Decode audio" << std::endl;
		std::cout << "Alternatively, type 'exit' to exit." << std::endl;

		std::cout << std::endl << "Select option [1, 2, 3, 4 or 5]: ";
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
		else if (answer == "4")
		{
			EncodeAudio();
		}
		else if (answer == "5")
		{
			DecodeAudio();
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
	char id[4];
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

WAVHeader GetDefaultMonoHeader()
{
	WAVHeader header;

	// RIFF
	memcpy(&header.riff.id, "RIFF", sizeof("RIFF"));
	header.riff.size = sizeof(WAVHeader);
	memcpy(&header.fmt, "WAVE", sizeof("WAVE"));

	// FMT
	memcpy(&header.riff.id, "fmt ", sizeof("fmt "));
	header.fmt.size = 16;
	header.fmt.audioformat = 1;
	header.fmt.numChannels = 1;
	header.fmt.sampleRate = 44100;
	header.fmt.byteRate = 88200;
	header.fmt.blockAlign = 2;
	header.fmt.bitsPerSample = 16;

	// Data
	memcpy(&header.data.id, "data", sizeof("data"));
	header.data.size = 0;

	return header;
}

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
		char xorChar = char(rand() % 5 - 10);
#endif

#ifdef DEBUG_SHOW_SEED
		std::cout << std::to_string(int32_t(xorChar)) << " ";
#endif

		fileBytes[i] += xorChar;
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

	/*std::cout << "Overwriting header OwO" << std::endl;
	WAVHeader header;
	memcpy(&header, &fileBytes[0], sizeof(WAVHeader));
	header.fmt.size = 16;
	header.fmt.audioformat = 1;
	header.fmt.numChannels = 1;
	header.fmt.sampleRate = 44100;
	header.fmt.byteRate = 88200;
	header.fmt.blockAlign = 2;
	header.fmt.bitsPerSample = 16;
	memcpy(&fileBytes[0], &header, sizeof(WAVHeader));

	PrintHeader(fileBytes);*/

	srand(RANDOM_SEED);
	for (size_t i = sizeof(WAVHeader); i < fileBytes.size(); ++i)
	{
#ifdef FIXED_XOR
		fileBytes[i] -= char(FIXED_XOR);
#else
		fileBytes[i] -= char(rand() % 5 - 10);
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

void EncodeAudio()
{
	WAVHeader header = GetDefaultMonoHeader();

	std::string buffer;

	std::string content;
	content.append("Once upon a time there were 3 MSc students trying to make their own encoding scheme, going against the general advice or not reinventing the wheel... :) It did not practically work so well but it's good a proof of concept regardless, as well it being a good learning experience. Hello Jaap and classmates!");

	std::string payload;

	std::cout << "Encoding text: \"";
	for (auto i = 0; i < content.size(); ++i)
	{
		std::cout << content[i];
		for (int32_t b = 0; b < BITS_PER_BYTE; ++b)
		{
			int32_t res = ((content[i] & (1 << b)) >> b);

			/*if (res == 1)
			{
				std::cout << "1";
			}
			else
			{
				std::cout << "0";
			}*/

			for (int32_t j = 0; j < LENGTH_PER_BIT; ++j)
			{
				payload.push_back(res == 1 ? 0x7F : 0x00);
			}
		}
	}
	std::cout << "\". Done!" << std::endl;

	//std::cout << "Payload: " << payload << std::endl;

	header.riff.size += payload.size();
	header.data.size += payload.size();

	buffer.resize(sizeof(WAVHeader));
	memcpy(&buffer[0], &header, sizeof(WAVHeader));

	if (!payload.empty())
	{
		buffer.append(payload);
	}

	//std::cout << "Final buffer: " << buffer << std::endl;

	// Save to file
	std::cout << "Saving to encoded file..." << std::endl;
	std::ofstream outputFile;
	outputFile.open("encoded.wav", std::ios::binary | std::ios::trunc);
	for (size_t i = 0; i < buffer.size(); ++i)
	{
		outputFile << buffer[i];
	}
	outputFile.close();

	std::cout << "All done!" << std::endl;
}

void CompareBitstream(const std::string& aStreamA, const std::string& aStreamB)
{
	uint32_t total = 0;
	uint32_t correct = 0;
	uint32_t incorrect = 0;

	for (auto i = 0; i < aStreamA.size(); ++i)
	{
		if (i >= aStreamB.size())
		{
			break;
		}

		if (aStreamA[i] == aStreamB[i])
		{
			correct++;
		}
		else
		{
			incorrect++;
		}
	}

	total = correct + incorrect;

	std::cout << "Bit stream comparison results:" << std::endl;
	std::cout << "\tTotal bits:\t" << total << std::endl;
	std::cout << "\tCorrect:\t" << correct << " (" << (100.f / total * correct) << "%)" << std::endl;
	std::cout << "\tIncorrect:\t" << incorrect << " (" << (100.f / total * incorrect) << "%)" << std::endl;
	std::cout << std::endl;
}

void DecodeAudio()
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

	std::string bits;

	bool isReadingZero = true;

	char sampleVal[sizeof(int16_t)];
	int32_t sampleIt = 0;

	float averageBit = 0.0f;

	int32_t bitIt = 0;
	//LENGTH_PER_BIT

	int32_t readIt = 0;
	int32_t previous = 0;

	int16_t high = INT16_MIN;
	int16_t low = INT16_MAX;

	for (auto i = sizeof(WAVHeader); i < fileBytes.size(); i += 1)
	{
		sampleVal[sampleIt] = fileBytes[i];
		sampleIt++;
		if (sampleIt >= sizeof(int16_t))
		{
			sampleIt = 0;

			int16_t numSampleVal;
			memcpy(&numSampleVal, &sampleVal[0], sizeof(int16_t));

			if (numSampleVal >= 0)
			{
				high = numSampleVal;
			}
			
			if (numSampleVal <= low)
			{
				low = numSampleVal;
			}
		}
	}

	std::cout << "High: " << high << " Low: " << low << std::endl;

	sampleIt = 0;

	uint32_t bitSteps = 0;

	for (auto i = sizeof(WAVHeader); i < fileBytes.size(); i += 1)
	{
		bitSteps++;
		sampleVal[sampleIt] = fileBytes[i];
		sampleIt++;
		if (sampleIt >= sizeof(int16_t))
		{
			sampleIt = 0;

			int16_t numSampleVal;
			memcpy(&numSampleVal, &sampleVal[0], sizeof(int16_t));

			if (numSampleVal >= (INT16_MIN / 4))//INT16_MAX - 1000)
			{
				float count = roundf(float(bitSteps) / 1024);
				if (count > 0.f)
				{
					bitSteps = 0;
					for (auto ii = 0; ii < count; ++ii)
					{
						bits += '1';
					}
				}

				isReadingZero = false;
			}
			else if (numSampleVal <= INT16_MIN)
			{
				float count = roundf(float(bitSteps) / 1024);
				if (count > 0.f)
				{
					bitSteps = 0;
					for (auto ii = 0; ii < count; ++ii)
					{
						bits += '0';
					}
				}
				//isReadingZero = true;
			}
			//std::cout << "Now reading a " << (isReadingZero ? "0" : "1") << std::endl;
		}

		//averageBit += isReadingZero ? 0.0f : 1.0f;

		/*bitIt++;
		if (bitIt >= LENGTH_PER_BIT)
		{
			averageBit /= LENGTH_PER_BIT;

			readIt++;

			if (readIt % 2 == 0)
			{
				int32_t current = (fabsf(averageBit) >= 0.5f) ? 1 : 0;
				current += previous;

				if (current >= 1)
				{
					bits += '0';
				}
				else
				{
					bits += '1';
				}
			}
			else
			{
				if (fabsf(averageBit) >= 0.5f)
				{
					previous = 1;
				}
				else
				{
					previous = 0;
				}
			}

			averageBit = 0.0f;
			bitIt = 0;
		}*/

		/*for (int32_t j = 0; j < LENGTH_PER_BIT; ++j)
		{
			numVal += (float)(fileBytes[i]);
		}

		floatVal[floatIt] = fileBytes[i];
		floatIt++;
		if (floatIt >= sizeof(int16_t))
		{
			floatIt = 0;

			int16_t val;
			memcpy(&val, &floatVal[0], sizeof(int16_t));
			std::cout << "Val: " << val << std::endl;

			//std::cout << "ID: " << (i - sizeof(WAVHeader)) << " | Byte: 0x" << std::hex << fileBytes[i] << " (" << std::dec << int32_t(fileBytes[i]) << ")" << std::endl;

		}*/
		/*char byte = fileBytes[i];
		(void)byte;

		if (isReadingZero)
		{
			if (byte == 0xFF)
			{
				isReadingZero = false;
			}
		}*/

		/*float numVal = 0.f;

		for (int32_t j = 0; j < LENGTH_PER_BIT; ++j)
		{
			numVal += (float)(fileBytes[i]);
		}

		numVal /= float(LENGTH_PER_BIT);

		if (0x7F - numVal >= (0x7F / 2))
		{
			bits += '0';
		}
		else
		{
			bits += '1';
		}*/
	}

	std::cout << "Collected " << bits.size() << " bits." << std::endl;

	std::string result;
	std::cout << "Result: \"";
	for (int32_t i = 0; i < bits.size(); i += BITS_PER_BYTE)
	{
		if (i + BITS_PER_BYTE >= bits.size())
		{
			break;
		}

		char c = 0x00;
		std::string bitString;
		for (auto b = 0; b < BITS_PER_BYTE; ++b)
		{
			if (bits[i + b] == '1')
			{
				c |= 1UL << b;
				bitString += "1";
			}
			else
			{
				c &= ~(1UL << b);
				bitString += "0";
			}
		}

		std::cout << c;// << bitString;
		result += c;
	}

	std::cout << "\"." << std::endl;

	std::cout << "Comparing bit streams..." << std::endl;
	std::string originalContent;
	originalContent.append("Once upon a time there were 3 MSc students trying to make their own encoding scheme, going against the general advice or not reinventing the wheel... :) It did not practically work so well but it's good a proof of concept regardless, as well it being a good learning experience. Hello Jaap and classmates!");

	std::string originalStream;
	for (auto i = 0; i < originalContent.size(); ++i)
	{
		for (int32_t b = 0; b < BITS_PER_BYTE; ++b)
		{
			int32_t res = ((originalContent[i] & (1 << b)) >> b);

			if (res == 1)
			{
				originalStream += '1';
			}
			else
			{
				originalStream += '0';
			}
		}
	}

	CompareBitstream(bits, originalStream);

	std::cout << std::endl << "All done!" << std::endl;
}
