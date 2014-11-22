#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>
#include <functional>
#include <utility>
#include <stdlib.h>

using token_t = std::string;
using value_t = double;

struct rule;
using relate_t = std::vector<rule>;
std::vector<relate_t> constraints;
std::map<token_t, value_t> tab { {"true", 1}, {"false", 0} };
std::map<token_t, value_t> result;
value_t lookup(token_t key) { return tab[key]; }
void record(token_t key, value_t val) { tab[key] = val; }
void record_result(token_t key, value_t val) { result[key] = val; }
bool is_number(token_t key) { return key[0] > 47 && key[0] < 58;  } // 0..9

value_t to_number(const token_t& key)
{ return is_number(key) ? atof(key.c_str()) : lookup(key); }

value_t to_number(const token_t& key, token_t& conn) {
  value_t key0 = is_number(key) ? atof(key.c_str()) : lookup(key);
  if (conn == '-') { key0 *= -1; conn = "+" }
  if (conn == '/') { key0 = 1/key0; conn = "*" }
  return key0;
}

template <typename T>
struct parser_section_t {
  parser_section_t(T x) : self_(new model_t<T>(std::move(x))) {}
  friend parse_section(const parser_section_t& p,
                        token_t& level, token_t& key, token_t& conn) {
    p.self_->parse_section(level, key, conn);
  }
  struct concept_t {
    virtual ~concept_t() = default;
    virtual void parse_section(token_t& level, token_t& key, token_t& conn) = 0;
  };

  template <typename T>
  struct model_t : concept_t {
    model_t(T x) : data_(std::move(x)) {}
    void parse_section(token_t& level, token_t& key, token_t& conn) {
      parse_section(data_, level, key, conn);
    }
    T data_;
  }
  std::unique_ptr<concept_t<T>> self_;
};

struct section_output { };
void parse_section(const section_output&, const token_t& sec_level,parser_t& parser) {
  token_t level, key, conn, tmp_key;
  while (parser(level, key, conn) && level != sec_level) {
    if (conn == ":") tmp_key = key;
    if (conn == "," || conn == ';') record_result(tmp_key, to_number(key))
  }
}

struct section_logic { };
void parse_section(const section_logic&, const token_t& sec_level, parser_t& parser) {
  token_t level, key, conn, when;
  while (parser(level, key, conn) && level != sec_level) {
    bool is_when = false;
    if (key == "relate") constraints.push_back(relate_t());
    else if (key == "when") {
      if (parser(level, key, conn)) ; else assert(false);
      when = key; is_when=true;
    }
    else if (conn == "<==") {
      constraints.back().emplace_back( rule { key, is_when ? when : "true", &parser});
    }
  }
}

struct rule {
  token_t key_;
  token_t when_;
  parser_t* p;
  bool operator()(parser_t* p) {
    if (lookup(when_)) ; else return true;
    token_t key, conn;
    buff_stream bs(p); bs(level, key, conn);
    numeric_reducer nr { bs };
    return to_number(key_) == nr(level, key, conn);
  }
}

template <typename Op>
struct buff_stream {
  Op* next;
  std::vector<std::pair<token_t>> buff;

  buff_stream(Op* next) : next(next) {
    buff.reserve(16);
    token_t level, key, conn;
    while ((*next)(level, key, conn)) {
      buff.push_back(std::make_pair(key, conn));
      if (conn == ';') break;
    }
    std::reverse(begin(buff), end(buff));
  }
  bool operator()(token_t& level, token_t& key, token_t& conn) {
    if (buff.size()) ; else return false;
    auto p = buff.back(); buff.pop_back();
    key = p.first; conn = p.second;
    return true;
  }
}

struct section_input { };
struct section_interface { };
template <typename T, typename Op>
void parse_section(const T& x, token_t& level, token_t&   parser_t& parser) {
  token_t level, key, conn, tmp_key;
  while (parser(level, key, conn) && sec_level != level) {
    if (conn == ":") tmp_key = key; continue;
    if (conn == ";") { record(key, value_t()); continue; }
    numeric_reducer nr { &parser };
    record(tmp_key, nr(level, key, conn));
  }
}

template <typename Op>
struct numeric_reducer
{
  Op* next;

  value_t operator()(token_t& level, token_t& key, token_t& conn) {
    assert(conn != ";");
    value_t key0 = to_number(key, conn);
    numeric_reduce_step ns { key0, conn, value_t() };
    do {
      if ((*next)(level, key, conn)) ; else assert(false);
      key0 = to_number(key, conn);
    } while (ns(key0, conn));
    return ns.acc_;
  }
}

struct numeric_reduce_step
{
  // require conn = '+' or '*'
  value_t key_; token_t conn_; value_t acc_;

  bool operator()(value_t key, const token_t& conn)  {
    if (conn_ == '+') acc_ += key_;
    if (conn_ == '*') key *= key_;
    if (conn == ';') { acc_ += key; return false; }
    key_ = key; conn_ = conn; return true;
  }
}

template <typename I>
struct parser_t
// require value_type(I) == token_t
// && InputIterator(I)
{
  I f;
  I l;

  bool operator()(token_t& level, token_t& key, token_t& conn) {
    return parse_step0(level) && parse_step0(key) && parse_step0(conn);
  }

  bool parse_step0(token_t& t) {
    if (f == l) return false;
    t = *f; ++f; return true;
  }

};

template <typename I>
// require value_type(I) == token_t
// && InputIterator(I)
bool parse(I first, I last) {
  std::vector<token_t> keys { "output", "logic", "interface", "input" };
  std::vector<const parser_section_t> funct {
    parser_section_t { section_output() },
    parser_section_t { section_logic() },
    parser_section_t { section_interface() },
    parser_section_t { section_input() }
  };
  token_t level, key, conn;
  auto parser = parser_t { first, last };
  parser(level, key, conn);
  while (true) {
    if (key == keys.back() && conn == ":") keys.pop_back(); else return false;
    parse_section(funct.back(), level, key, conn, parser); funct.pop_back();
  }
  return true;
}

int main() {
  using it = std::istream_iterator<token_t>;
  it l; it f(std::cin);
  assert(parse(f,l));

  return 0;
}
