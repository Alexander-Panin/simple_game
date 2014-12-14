#include <iostream>
#include <map>
#include <stdlib.h>
#include <vector>
#include <cassert>
#include "types.hpp"
#include "helper.hpp"
#include "parser.hpp"
#include "sections.hpp"

value_t to_number(sheet_t& s, const token_t& key)
{ return is_number(key) ? atof(key.c_str()) : s.tab[key]; }

bool parse(it first, it last, sheet_t& s) {
  std::vector<token_t> keys { "output", "logic", "interface", "input" };
  std::vector<sections::parser_t> funcs;
  funcs.emplace_back( sections::parser_t { sections::input() } );
  funcs.emplace_back( sections::parser_t { sections::interface() } );
  funcs.emplace_back( sections::parser_t { sections::logic() } );
  funcs.emplace_back( sections::parser_t { sections::output() } );
  auto f = begin(funcs); auto l = end(funcs);
  token_t level, key, conn;
  iparser_t p { first, last };
  while (p(level, key, conn)) {
    // requires level always the same
    assert(keys.size()); assert(f != l);
    if (key == keys.back() && conn == ":") keys.pop_back(); else return false;
    p = sections::parse(*f++, level, p, s);
  }
  return true;
}

int main() {
  it l; it f(std::cin);
  sheet_t s;
  assert(parse(f,l,s));

  return 0;
}
