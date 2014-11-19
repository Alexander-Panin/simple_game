#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>
#include <functional>
#include <utility>
#include <stdlib.h>

using token_t = std::string;
using value_t = double;

using relate_t = std::vector<rule>;
std::vector<relate_t> constraints;
std::vector<token_t> stack { "output", "logic", "interface", "input" };
std::map<token_t, value_t> tab { {"true", 1}, {"false", 0} };
std::map<token_t, value_t> result;
value_t lookup(token_t key) { return tab[key]; }
void record(token_t key, value_t val) { tab[key] = val; }
void record_result(token_t key, value_t val) { result[key] = val; }
bool is_number(token_t key) { return key[0] > 47 && key[0] < 58;  } // 0..9

void parse_section_output(token_t& level, token_t& key, token_t& conn) {
  token_t prev_level = level;
  while (parse_step(level, key, conn) && level != prev_level) {
    if (conn == "<==") continue;
    else if (conn == ":") tmp_key = key;
    else if (conn == "," || conn == ';') record_result(tmp_key, to_number(key))
  }
}

void parse_section_logic(token_t& level, token_t& key, token_t& conn) {
  token_t prev_level = level;
  token_t when;
  while (parse_step(level, key, conn) && level != prev_level) {
    bool is_when = false;
    if (key == "relate") { constraints.push_back(relate_t()); }
    else if (key == "when") {
      if (parse_step(level, key, conn)) ; else assert(false);
      when = key; is_when=true;
    }
    else if (conn == "<==") {
      constraints.back().emplace_back( rule { key, is_when ? when : "true" } );
    }
  }
}

template <typename Op>
struct rule {
  token_t key_;
  token_t when;
  bool operator()() {
    if (lookup(when)) ; else return true;
    token_t key, conn;
    buff_stream bs; bs(key, conn);
    numeric_reducer nr { bs };
    return to_number(key_) == nr(key, conn);
  }
}

struct buff_stream {
  std::vector<std::pair<token_t>> buff;
  buff_stream() {
    buff.reserve(16);
    token_t level, key, conn;
    while (parse_step(level, key, conn)) {
      buff.push_back(std::make_pair(key, conn));
      if (conn == ';') break;
    }
    std::reverse(begin(buff), end(buff));
  }
  void operator()(token_t& key, token_t& conn) {
    assert(buff.size());
    auto p = buff.back(); buff.pop_back();
    key = p.first; conn = p.second;
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
    numeric_reducer nr { std::bind1st(parse_step, level) };
    record(tmp_key, nr(key, conn));
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
struct numeric_reducer
{
  Op next;

  value_t operator()(token_t& key, token_t& conn) {
    assert(conn != ";");
    value_t key0 = to_number(key, conn);
    numeric_reduce_step ns { key0, conn, value_t() };
    do {
      if (next(key, conn)) ; else assert(false);
      key0 = to_number(key, conn);
    } while (ns(key0, conn));
    return ns.acc_;
  }
}

struct numeric_reduce_step
{
  // require conn = '+' or '*'
  value_t key_; token_t conn_; value_t acc_;

  bool operator()(value_t key, token_t conn)  {
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
