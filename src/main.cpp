#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>

enum class LetterClass {
	RIGHT,
	WRONG_POS,
	WRONG
};

std::vector<std::string> getWordsFromFile(const char* path)
{
	std::ifstream wordsFile(path);
	std::vector<std::string> words;
	std::string line;
	while (std::getline(wordsFile, line)) {
		words.push_back(line);
	}

	return words;
}

std::string getColoredWord(const std::string& word, std::vector<LetterClass> colors)
{
	const std::string green = "\x1b[42m";
	const std::string yellow = "\x1b[43m";
	const std::string reset = "\x1b[0m";

	std::string result = "";

	for (size_t i = 0; i < word.length(); i++)
	{
		switch (colors[i]) {
		case LetterClass::RIGHT:
			result += green + word[i] + reset;
			break;
		case LetterClass::WRONG_POS:
			result += yellow + word[i] + reset;
			break;
		case LetterClass::WRONG:
			result += word[i];
			break;
		}
	}

	return result;
}

std::vector<LetterClass> processGuess(const std::string& correctWord, const std::string& guessedWord)
{
	std::vector<LetterClass> guess;
	std::vector<bool> lettersUsed(5, false);

	for (size_t i = 0; i < guessedWord.length(); i++)
	{
		if (guessedWord[i] == correctWord[i]) {
			guess.push_back(LetterClass::RIGHT);
			lettersUsed[i] = true;
		} else {
			guess.push_back(LetterClass::WRONG);
		}
	}

	for (size_t i = 0; i < guessedWord.length(); i++)
	{
		if (guess[i] == LetterClass::WRONG) {
			for (size_t j = 0; j < guessedWord.length(); j++)
			{
				if (guessedWord[i] == correctWord[j] && !lettersUsed[j]) {
					guess[i] = LetterClass::WRONG_POS;
					lettersUsed[j] = true;
					break;
				}
			}
		}
	}

	return guess;
}

bool isGameWon(std::vector<LetterClass> guess)
{
	for (auto& letter : guess) {
		if (letter != LetterClass::RIGHT) {
			return false;
		}
	}

	return true;
}

int main()
{
	auto allWords = getWordsFromFile("./wordlists/allwords.txt");
	auto possibleWords = getWordsFromFile("./wordlists/possiblewords.txt");

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distWord(0, (int)possibleWords.size() - 1);

	std::string correctWord = possibleWords[distWord(rng)];

	while (true) {
		std::string guessedWord;
		std::getline(std::cin, guessedWord);
		auto guess = processGuess(correctWord, guessedWord);

		// check if guess is valid
		bool guessFound = false;
		for (auto& word : allWords) {
			if (word == guessedWord) {
				guessFound = true;
				break;
			}
		}

		if (!guessFound) {
			for (auto& word : possibleWords) {
				if (word == guessedWord) {
					guessFound = true;
					break;
				}
			}

			if (!guessFound) {
				std::cout << "Not in word list!\n";
				continue;
			}
		}

		std::cout << "\033[A\r" << getColoredWord(guessedWord, guess) << '\n';

		if (isGameWon(guess)) {
			std::cout << "Congratulations, you won!";
			break;
		}
	}
}