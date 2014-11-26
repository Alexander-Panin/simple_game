#include <iostream>
//#include <iterator>
//#include <algorithm>
#include <map>
//#include <functional>
//#include <utility>
#include <stdlib.h>
#include <vector>
#include <cassert>

using token_t = std::string;
using value_t = double;

bool is_number(token_t key) { return key[0] > 47 && key[0] < 58;  } // 0..9

template <typename Op>
struct buff_stream {
  Op next;
  std::vector<std::pair<token_t, token_t>> buff;

  Op create() {
    buff.reserve(16);
    token_t level, key, conn;
    while (next(level, key, conn)) {
      buff.push_back(std::make_pair(key, conn));
      if (conn == ";") break;
    }
    std::reverse(begin(buff), end(buff));
    return next;
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
    auto v = parse_step(level) && parse_step(key) && parse_step(conn);
    if (v) std::cout << level << " " << key << " " << conn << "\n";
    return v;
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
  table_t tab { {"true", value_t(1)}, {"false", value_t(0)} };
  table_t result;

  // do not use outside sheet_t
  struct rule {
    token_t key_; token_t when_;
    buff_stream<iparser_t> bs;
    sheet_t* s;
    bool operator()() {
      if (s->lookup(when_)) ; else return true;
      numeric_reducer<decltype(bs)> nr { bs, s };
      return s->to_number(key_) == nr().first;
    }
  };
  using relate_t = std::vector<rule>;
  std::vector<relate_t> constraints;

  // do not use outside sheet_t
  template <typename Op>
  struct numeric_reducer
  {
    Op next;
    sheet_t* s;

    std::pair<value_t, Op> operator()() {
      token_t level, key, conn;
      if (next(level, key, conn)) ; else assert(false);
      assert(conn != ";");
      value_t key0 = s->to_number(key, conn);
      numeric_reduce_step ns { key0, conn, value_t() };
      while (ns(key0, conn)) {
        if (next(level, key, conn)) ; else assert(false);
        key0 = s->to_number(key, conn);
      }
      return std::make_pair(ns.acc_, next);
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

  value_t lookup(token_t k) { return tab[k]; }
  void make_rules_set() { constraints.emplace_back(relate_t()); }
  void add_rule(token_t key, token_t when, buff_stream<iparser_t> bf) {
    constraints.back().emplace_back( rule { key, when, bf, this });
  }
  void record(const token_t& key, value_t val)
  { tab[key] = val; }
  void record(const token_t& key, token_t val)
  { tab[key] = to_number(val); }
  void record_result(const token_t& key, token_t val)
  { result[key] = to_number(val); }
  iparser_t record_expr(const token_t& key, iparser_t p) {
    numeric_reducer<decltype(p)> nr { p, this };
    tab[key] = nr().first;
    return p;
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
template <typename T>
iparser_t parse_section(const T& x, const token_t& l, iparser_t p, sheet_t& s) {
  token_t level, key, conn, tmp_key;
  while (!p.is_match(l) && p(level, key, conn)) {
    if (conn == ":") { tmp_key = key; continue; }
    if (conn == ";") { s.record(key, value_t()); continue; }
    p = s.record_expr(tmp_key, p); //nr(level, key, conn));
  }
  return p;
}

struct parser_section_t {
  template <typename T>
  parser_section_t(T x) : self_(new model<T>(std::move(x))) {}
  friend iparser_t parse_section(const parser_section_t& ps, const token_t& l,
                                  iparser_t p, sheet_t& s) {
    return ps.self_->parse_section_(l,p,s);
  }
  struct concept_t {
    virtual ~concept_t() = default;
    virtual iparser_t parse_section_(const token_t& l, iparser_t p,
                                      sheet_t& s) const = 0;
  };

  template <typename T>
  struct model : concept_t {
    model(T x) : data_(std::move(x)) {}
    iparser_t parse_section_(const token_t& l, iparser_t p, sheet_t& s) const {
      return parse_section(data_, l, p, s);
    }
    T data_;
  };
  std::unique_ptr<concept_t> self_;
};

struct sec_output { };
iparser_t parse_section(const sec_output& x, const token_t& l,
                          iparser_t p, sheet_t &s) {
  token_t level, key, conn, tmp_key;
  while (!p.is_match(l) && p(level, key, conn)) {
    if (conn == ":") tmp_key = key;
    else if (conn == "," || conn == ";") s.record_result(tmp_key, key);
  }
  return p;
}

struct sec_logic { };
iparser_t parse_section(const sec_logic&, const token_t& l, iparser_t p, sheet_t& s) {
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
      buff_stream<decltype(p)> bs { p };
      p = bs.create();
      s.add_rule( key, is_when ? when : "true", bs );
    }
  }
  return p;
}

bool parse(it first, it last, sheet_t& s) {
  std::vector<token_t> keys { "output", "logic", "interface", "input" };
  std::vector<parser_section_t> funcs;
  funcs.emplace_back( parser_section_t { sec_input() } );
  funcs.emplace_back( parser_section_t { sec_interface() } );
  funcs.emplace_back( parser_section_t { sec_logic() } );
  funcs.emplace_back( parser_section_t { sec_output() } );
  auto f = begin(funcs); auto l = end(funcs);
  token_t level, key, conn, sec_level;
  iparser_t p { first, last };
  while (p(level, key, conn)) {
    //assert( sec_level == level );
    sec_level = level;
    if (key == keys.back() && conn == ":") keys.pop_back(); else return false;
    p = parse_section(*f++, level, p, s);
  }
  return true;
}

int main() {
  it l; it f(std::cin);
  sheet_t s;
  assert(parse(f,l,s));
  std::cout << s.tab.size();

  return 0;
}
