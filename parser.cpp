#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>
#include <functional>
#include <utility>
#include <stdlib.h>
#include <vector>
#include <cassert>

using token_t = std::string;
using value_t = double;

bool is_number(token_t key) { return key[0] > 47 && key[0] < 58;  } // 0..9

template <typename Op>
struct buff_stream {
  Op* next;
  std::vector<std::pair<token_t, token_t>> buff;

  buff_stream(Op* next) : next(next) {
    buff.reserve(16);
    token_t level, key, conn;
    while ((*next)(level, key, conn)) {
      buff.push_back(std::make_pair(key, conn));
      if (conn == ";") break;
    }
    std::reverse(begin(buff), end(buff));
  }
  bool operator()(token_t& level, token_t& key, token_t& conn) {
    if (buff.size()) ; else return false;
    auto p = buff.back(); buff.pop_back();
    key = p.first; conn = p.second;
    return true;
  }
};

template <typename I>
struct parser_t
// require value_type(I) == token_t
// && InputIterator(I)
{
  I f ; I l;

  bool operator()(token_t& level, token_t& key, token_t& conn) {
    return parse_step(level) && parse_step(key) && parse_step(conn);
  }

  bool is_match(const token_t& x) {
    if (f != l && *f == x) return true;
    else return false;
  }

  bool parse_step(token_t& t) {
    if (f == l) return false;
    t = *f; ++f; return true;
  }
};

using it = std::istream_iterator<token_t>;
using iparser_t = parser_t<it>;

struct sheet_t {
  using table_t = std::map<token_t, value_t>;
  table_t tab { {"true", 1}, {"false", 0} };
  table_t result;

  value_t lookup(token_t k) { return tab[k]; }
  struct rule {
    token_t key_; token_t when_;
    buff_stream<iparser_t> bs;
    sheet_t* s;
    bool operator()() {
      if (s->lookup(when_)) ; else return true;
      numeric_reducer<decltype(bs)> nr { &bs };
      return s->to_number(key_) == nr();
    }
  };
  using relate_t = std::vector<rule>;
  std::vector<relate_t> constraints;

  template <typename Op>
  struct numeric_reducer
  {
    Op* next;
    sheet_t* s;

    value_t operator()() {
      token_t level, key, conn;
      if ((*next)(level, key, conn)) ; else assert(false);
      assert(conn != ";");
      value_t key0 = s->to_number(key, conn);
      numeric_reduce_step ns { key0, conn, value_t() };
      while (ns(key0, conn)) {
        if ((*next)(level, key, conn)) ; else assert(false);
        key0 = s->to_number(key, conn);
      }
      return ns.acc_;
    }
  };

  struct numeric_reduce_step
  {
    // require conn = '+' or '*'
    value_t key_; token_t conn_; value_t acc_;

    bool operator()(value_t key, const token_t& conn)  {
      if (conn_ == "+") acc_ += key_;
      if (conn_ == "*") key *= key_;
      if (conn == ";") { acc_ += key; return false; }
      key_ = key; conn_ = conn; return true;
    }
  };

  void make_rules_set() { constraints.emplace_back(relate_t()); }
  void add_rule(token_t key, token_t when, buff_stream<iparser_t> bf) {
    constraints.back().emplace_back( rule { key, when, bf });
  }
  void record(token_t key, value_t val)
  { tab[key] = val; }
  void record(token_t key, token_t val)
  { tab[key] = to_number(val); }
  void record_result(token_t key, token_t val)
  { result[key] = to_number(val); }
  void record_expr(token_t key, iparser_t& p) {
    numeric_reducer<decltype(p)> nr { &p };
    tab[key] = nr();
  }
  value_t to_number(const token_t& key)
  { return is_number(key) ? atof(key.c_str()) : tab[key]; }

  value_t to_number(const token_t& key, token_t& conn) {
    value_t key0 = is_number(key) ? atof(key.c_str()) : tab[key];
    if (conn == "-") { key0 *= -1; conn = "+"; }
    if (conn == "/") { key0 = 1/key0; conn = "*"; }
    return key0;
  }
};

struct sec_input { };
struct sec_interface { };
template <typename T, typename Op>
void parse_section(const T& x, const token_t& l, iparser_t& p, sheet_t& s) {
  token_t level, key, conn, tmp_key;
  while (!p.is_match(l) && p(level, key, conn)) {
    if (conn == ":") tmp_key = key; continue;
    if (conn == ";") { s.record(key, value_t()); continue; }
    s.record_expr(tmp_key, p); //nr(level, key, conn));
  }
}

struct parser_section_t {
  template <typename T>
  parser_section_t(T x) : self_(new model_t<T>(std::move(x))) {}
  friend void parse_section(const parser_section_t& ps, const token_t& l,
                              iparser_t& p, sheet_t& s) {
    ps.self_->parse_section_(l,p,s);
  }
  struct concept_t {
    virtual ~concept_t() = default;
    virtual void parse_section_(const token_t& l, iparser_t& p, sheet_t& s) = 0;
  };

  template <typename T>
  struct model_t : concept_t {
    model_t(T x) : data_(std::move(x)) {}
    void parse_section_(const token_t& l, iparser_t& p, sheet_t& s) {
      parse_section(data_, l, p, s);
    }
    T data_;
  };
  std::unique_ptr<concept_t> self_;
};

struct sec_output { };
void parse_section(const sec_output& x, const token_t& l, iparser_t& p, sheet_t &s) {
  token_t level, key, conn, tmp_key;
  while (!p.is_match(l) && p(level, key, conn)) {
    if (conn == ":") tmp_key = key;
    else if (conn == "," || conn == ";") s.record_result(tmp_key, key);
  }
}

struct sec_logic { };
void parse_section(const sec_logic&, const token_t& l, iparser_t& p, sheet_t& s) {
  token_t level, key, conn, when;
  while (!p.is_match(l) && p(level, key, conn)) {
    bool is_when = false;
    if (key == "relate") s.make_rules_set();
    else if (key == "when") {
      if (p(level, key, conn)) ; else assert(false); // require the same level
      when = key; is_when=true;
    }
    else if (conn == "<==") {
      if (p(level, key, conn)) ; else assert(false); // require the same level
      s.add_rule( key, is_when ? when : "true", buff_stream<decltype(p)>{ &p });
    }
  }
}


bool parse(it first, it last, sheet_t& s) {
  std::vector<token_t> keys { "output", "logic", "interface", "input" };
  std::vector<const parser_section_t> funct {
    parser_section_t { sec_output() },
    parser_section_t { sec_logic() },
    parser_section_t { sec_interface() },
    parser_section_t { sec_input() }
  };
  token_t level, key, conn, sec_level;
  iparser_t p { first, last };
  while (p(level, key, conn)) {
    assert( keys.size() );
    assert( sec_level == level );
    sec_level = level;
    if (key == keys.back() && conn == ":") keys.pop_back(); else return false;
    parse_section(funct.back(), level, p, s); funct.pop_back();
  }
  return true;
}

int main() {
  it l; it f(std::cin);
  sheet_t s;
  assert(parse(f,l,s));

  return 0;
}
