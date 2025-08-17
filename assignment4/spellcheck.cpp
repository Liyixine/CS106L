#include "spellcheck.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>
#include <cctype>

template <typename Iterator, typename UnaryPred>
std::vector<Iterator> find_all(Iterator begin, Iterator end, UnaryPred pred);

Corpus tokenize(std::string& source) {
  /* TODO: Implement this method */
  auto it_space = find_all(source.begin(),source.end(),[](auto it){return std::isspace(it);});
  Corpus tokens;
  std::transform(it_space.begin(),it_space.end()-1,it_space.begin()+1,std::inserter(tokens,tokens.begin()),
    [&source](auto it1,auto it2) {return Token(source,it1,it2);});
    // use std:inserter to modify std::set(tokens)
  std::erase_if(tokens,[](Token token){return token.content.empty();});
  return tokens;
}

std::set<Misspelling> spellcheck(const Corpus& source, const Dictionary& dictionary) {
  /* TODO: Implement this method */
  namespace rv = std::ranges::views;
  auto view = source | rv::filter([&dictionary](auto word){return !(dictionary.contains(word.content));})
                     | rv::transform([&dictionary](auto word){
                      auto lv1 = dictionary | rv::filter([&word](auto test_word){return levenshtein(word.content,test_word)==1;});
                      std::set<std::string> lv1_set(lv1.begin(),lv1.end());
                      return Misspelling{word,lv1_set};})
                     | rv::filter([](auto ms){return !ms.suggestions.empty();});
  std::set<Misspelling> ms(view.begin(),view.end());
  return ms;
};

/* Helper methods */

#include "utils.cpp"