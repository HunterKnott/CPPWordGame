#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <map>
#include <cctype>
#include <string>
#include <cstdlib>

const int NWORDS = 81483;
std::vector<std::string> words;
std::string word = " ";
std::vector<std::string> sub_words;
std::map<int, std::vector<std::string>> wordDictionary;
std::map<int, std::vector<std::string>> dashDictionary;

void read_words(const std::string& fname) {
  std::ifstream wordf(fname);
  std::string line;
  while(std::getline(wordf, line)) {
    words.push_back(line);
  }
}

bool sort_by_size(std::string word1, std::string word2) {
  return word1.size() < word2.size();
}

void initializeDictionaries() {
  // if(argc > 1) {
  //   std::string test = argv[1];
  //   if (test.length() >= 3) {
  //     word = argv[1];
  //   }
  // }

  if (word == " ") {
    std::default_random_engine dre(std::time(nullptr));
    std::uniform_int_distribution<int> di(0, NWORDS);
    word = words.at(di(dre));
    while (word.length() < 3) {
      word = words.at(di(dre));
    }
  }

  std::vector<char> word_chars;
  for (char c: word) {
    word_chars.push_back(c);
  }

  for(std::string wrd: words) {
    bool is_sub = true;
    std::vector<char> word_char_loop = word_chars;
    for (char c: wrd) {
      auto it = find(word_char_loop.begin(), word_char_loop.end(), c);
      if (it != word_char_loop.end()) {
        word_char_loop.erase(it);
      }
      else {
        is_sub = false;
      }
    }
    if (is_sub && wrd.length() > 3) {
      sub_words.push_back(wrd);
    }
  }

  sort(sub_words.begin(), sub_words.end(), sort_by_size);

  int curr_length = 4;
  int start_pos = 0;

  // std::cout << "The word is " << word << std::endl << std::endl;
  std::vector<std::string> subword_list;

  for(int i = 0; i < sub_words.size(); i++) {
    if (sub_words.at(i).length() > curr_length) {
      wordDictionary[curr_length] = std::vector<std::string>(sub_words.begin() + start_pos, sub_words.begin() + i);

      std::vector<std::string> dashes;
      for (const std::string& wrd : wordDictionary[curr_length]) {
        dashes.push_back(std::string(wrd.length(), '-'));
      }
      dashDictionary[curr_length] = dashes;
      subword_list.insert(subword_list.end(), sub_words.begin() + start_pos, sub_words.begin() + i);

      start_pos = i;
      curr_length++;
    }
  }
}

void displayDashes() {
  for (const auto& entry : dashDictionary) {
    std::cout << "Words of length " << entry.first << ": ";
    for (const std::string& dash : entry.second) {
      std::cout << dash << " ";
    }
    std::cout << std::endl;
  }
}

int main(int argc, char* argv[]) {
  read_words("words.txt");
  initializeDictionaries();

  std::default_random_engine dre(std::time(nullptr));
  std::uniform_int_distribution<int> di(0, NWORDS);

  char playAgain = 'y';

  while (playAgain == 'y' || playAgain == 'Y') {
    int randomLength = 3;
    while (wordDictionary.find(randomLength) == wordDictionary.end()) {
      randomLength++;
    }

    std::vector<std::string>& wordsList = wordDictionary[randomLength];
    std::string& dashes = dashDictionary[randomLength][0];

    std::vector<char> charVector(word.begin(), word.end());
    std::random_device rd;
    std::mt19937 g(rd());
    charVector.pop_back();
    std::shuffle(charVector.begin(), charVector.end(), g);
    std::string scrambledWord(charVector.begin(), charVector.end());

    std::cout << scrambledWord << std::endl;
    displayDashes();

    std::string guess;
    std::cout << "Enter your guess (or 'q' to quit): ";
    std::cin >> guess;
    guess.erase(std::remove_if(guess.begin(), guess.end(), [](unsigned char c) { return std::isspace(c); }), guess.end());

    if (guess == "q" || guess == "Q") {
      break;
    }

    // FIXME: Correct words are being rejected
    auto it = std::find_if(sub_words.begin(), sub_words.end(), [guess](const std::string& subword) {
        return std::equal(subword.begin(), subword.end(), guess.begin(), guess.end(), [](char a, char b) {
            return std::tolower(a) == std::tolower(b);
        });
    });
    std::cout << *it << std::endl;
    if (it != sub_words.end()) {
      int index = std::distance(wordsList.begin(), it);
      dashes = wordsList[index];
      std::cout << dashes << std::endl;
      std::cout << "Correct! The word is: " << wordsList[index] << std::endl;
    } else {
      std::cout << "Incorrect! Try again." << std::endl << std::endl;
    }

    //** Check if all words are guessed
    bool allGuessed = true;
    for (const auto& entry : dashDictionary) {
      if (std::find(entry.second.begin(), entry.second.end(), std::string(entry.first, '-')) != entry.second.end()) {
        allGuessed = false;
        break;
      }
    }

    if (allGuessed) {
      std::cout << "Congratulations! You've guessed all the words." << std::endl;
      std::cout << "Do you want to play again? (y/n): ";
      std::cin >> playAgain;
      if (playAgain == 'y' || playAgain == 'Y') {
        initializeDictionaries();
      }
    }
  }

  std::cout << "Thanks for playing!" << std::endl;

  return 0;
}
