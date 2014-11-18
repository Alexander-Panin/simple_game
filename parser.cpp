#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>
#include <functional>
#include <stdlib.h>

using token_t = std::string;
using value_t = double;

// key ; -> key lookup | set key default value also terminated
// key <== val -> pair<key, lazy val>
// key , -> just continue
//
// key ( -> key = "when"
// key ) -> set predicate

std::vector<token_t> stack { "output", "logic", "interface", "input" };
std::map<token_t, value_t> tab { {"true", 1}, {"false", 0} };
value_t lookup(token_t key) { return tab[key]; }
void record(token_t key, value_t val) { tab[key] = val; }
bool is_number(token_t key) { return key[0] > 47 && key[0] < 58;  } // 0..9

struct when
{
  token_t key;
  bool operator()() { return (key == token_t() ? true : lookup(key)); }
}

void parse_section_logic(token_t& level, token_t& key, token_t& conn) {
  token_t prev_level = level;
  token_t tmp_key;
  while (parse_step(level, key, conn) && level != prev_level) {
    when w;
    if (key == "relate" && conn == ":") { push_back ...; continue; }
    if (key == "when" ... get next ) {  }
    if (conn == "<==") {
      tmp_key = key;
      grasp_relate();

    }

  }
}



template <typename T>
void parse_section(token_t& level, token_t& key, token_t& conn) {
  token_t prev_level = level;
  token_t tmp_key;
  while (parse_step(level, key, conn) && level != prev_level) {
    if (conn == ":") tmp_key = key; continue;
    if (conn == ";") { record(key, value_t()); continue; }
    //assert(is_number(key) && is_var(key));
    numeric num { key, conn, std::bind1st(parse_step, level) };
    record(tmp_key, num.reduce());
  }
  parse_header (level, key, conn);
}

void parse_header(token_t& level, token_t& key, conn) {
  if ( key == stack.back() && conn == ":") stack.pop_back(); else return;
  parse_section<key>(level, key, conn);
}

value_t to_number(const token_t& key, token_t& conn) {
  value_t key0 = is_number(key) ? atof(key.c_str()) : lookup(key);
  if (conn == '-') { key0 *= -1; conn = "+" }
  if (conn == '/') { key0 = 1/key0; conn = "*" }
  return key0;
}

template <typename Op>
// Op should provide new key and conn
struct numeric
{
  // require conn != ';'
  token_t key; token_t conn;
  Op next;
  value_t reduce() {
    value_t key0 = to_number(key, conn);
    numeric_step ns { key0, conn, value_t() };
    do {
      if (next(key, conn)) ; else assert(false);
      key0 = to_number(key, conn);
    } while (ns.reduce(key0, conn));
    return ns.acc;
  }
}

struct numeric_step
{
  // require conn = '+' or '*'
  value_t key_; token_t conn_;
  value_t acc_;
  bool reduce()(value_t key, token_t conn)  {
    if (conn_ == '+') acc_ += key_;
    if (conn_ == '*') key *= key_;
    if (conn == ';') { acc_ += key; return false; }
    key_ = key; conn_ = conn; return true;
  }
}


bool parse() {
  token_t level, key, conn;
  if (parse_step(level, key, conn)) ; else return false;
  parse_header(level, key, conn);
  return stack.empty();
}

template <typename I>
struct parser
// require value_type(I) == token_t
// && InputIterator(I)
{
  I f;
  I l;

  bool parse_step(token_t& level, token_t& key, token_t& conn) {
    return parse_step0(level) && parse_step0(key) && parse_step0(conn);
  }

  bool parse_step0(token token_t& t) {
    if (f == l) return false;
    t = *f; ++f; return true;
  }
};

int main() {
  using it = std::istream_iterator<token_t>;
  it l; it f(std::cin);
  while (f != l) { }

  return 0;
}
