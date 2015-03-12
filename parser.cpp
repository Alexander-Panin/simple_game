#include <iostream>
#include <map>
#include <stdlib.h>
#include <vector>
#include <cassert>

using token_t = std::string;
using value_t = double;

// assume if starts from digit it's a digit
bool is_number(const token_t& key) { return key[0] > 47 && key[0] < 58;  } // 0..9

template <typename I>
struct parser_t
// require value_type(I) == token_t
// && InputIterator(I)
{
  I f ; I l;

  bool next(token_t& level, token_t& key, token_t& conn)
  { return step(level) && step(key) && step(conn); }

  bool is_match(const token_t& x) {
    if (f != l && *f == x) return true;
    else return false;
  }

  bool step(token_t& t) {
    if (f == l) return false;
    t = *f; ++f; return true;
  }
};

// helper's functors -------------------------

struct buff_stream {
  std::vector<std::pair<token_t, token_t>> buff;

  template <typename Op>
  Op create(Op next) {
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
    auto p = buff.back();
    key = p.first; conn = p.second;
    buff.pop_back();
    return true;
  }
};

struct numeric_reduce_step
{
  value_t key_; token_t conn_; value_t acc_;

  bool operator()(value_t key, const token_t& conn)  {
    if (conn_ == "-") { key *= -1; conn_ = "+"; }
    else if (conn_ == "/") { key = 1/key; conn_ = "*"; }

    if (conn_ == "+") acc_ += key_;
    else if (conn_ == "*") key *= key_;

    if (conn == ";") { acc_ += key; return false; }
    key_ = key; conn_ = conn; return true;
  }
};

template <typename Op, typename T>
// TODO add requires
// plz provide owner to use
struct numeric_reducer
{
  Op next;
  T* table;

  std::pair<value_t, Op> operator()(token_t& level, token_t& key, token_t& conn) {
    if (conn == ";") return std::make_pair(to_number(*table, key), next);
    value_t key0 = to_number(*table, key);
    numeric_reduce_step ns { key0, conn, value_t() };
    do {
      if (next(level, key, conn)) ; else assert(false);
      key0 = to_number(*table, key);
    } while (ns(key0, conn));
    return std::make_pair(ns.acc_, next);
  }
};

template<typename T>
// TODO add requires
// plz provide owner to use
struct rule {
  token_t key_;
  token_t when_;
  buff_stream bs;
  T* table;
  bool operator()() {
    if (lookup(*table, when_)) ; else return true;
    token_t level, key, conn;
    bs(level, key, conn);
    numeric_reducer<decltype(bs), T> nr { bs, table };
    value_t res = nr(level, key, conn).first;
    value_t res0 = to_number(*table, key_);
    return res0 == res;
  }
};

// main state object after parsing

struct sheet_t {
  using table_t = std::map<token_t, value_t>;
  using relate_t = std::vector<rule<sheet_t>>;
  using result_t = std::vector<std::pair<token_t, token_t>>;

  table_t tab { {"true", value_t(1)}, {"false", value_t(0)} };
  std::vector<relate_t> constraints;
  result_t result;

  void make_rules_set() { constraints.emplace_back(relate_t()); }
  void add_rule(token_t key, token_t when, buff_stream bf)
  { constraints.back().emplace_back( rule<sheet_t> { key, when, bf, this }); }

  iparser_t record_expr(const token_t& k, iparser_t p, token_t& level,
                          token_t& key, token_t& conn) {
    numeric_reducer<decltype(p), sheet_t> nr { p, this };
    auto res = nr(level, key, conn);
    record(*this, k, res.first);
    return res.second;
  }

  friend value_t to_number(sheet_t& s, const token_t& key);
  friend value_t lookup(sheet_t& s, token_t k) { return s.tab[k]; }
  friend void record(sheet_t& s, const token_t& key, value_t val) { s.tab[key] = val;}

  table_t result_map() {
    auto f = begin(result); auto l = end(result); table_t m;
    while(f != l) { m[(*f).first] = tab[(*f).second]; f++; }
    return m;
  }
};

value_t to_number(sheet_t& s, const token_t& key)
{ return is_number(key) ? atof(key.c_str()) : s.tab[key]; }

// section's polymorphism

template <typename Parser>
// input, interface
Parser parse0(Parser p, const token_t& l, sheet_t& s) {
  token_t level, key, conn, tmp_key;
  bool default_val = true;
  while (!p.is_match(l) && p.next(level, key, conn)) {
    if (conn == ":") { tmp_key = key; default_val = false; continue; }
    if (conn == ";" && default_val) { record(s, key, value_t()); continue; }
    p = s.record_expr(tmp_key, p, level, key, conn); default_val = true;
  }
  return p;
}

template <typename Parser>
// output
Parser parse1(Parser p, const token_t& l, sheet_t &s) {
  token_t level, key, conn, tmp_key;
  std::vector<std::pair<token_t, token_t>> m;
  while (!p.is_match(l) && p.next(level, key, conn)) {
    if (conn == ":") tmp_key = key;
    else if (conn == "," || conn == ";") m.emplace_back(std::make_pair(tmp_key, key));
  }
  s.result = std::move(m);
  return p;
}

template <typename Parser>
// logic
Parser parse2(Parser p, const token_t& l, sheet_t& s) {
  token_t level, key, conn, when;
  while (!p.is_match(l) && p.next(level, key, conn)) {
    bool is_when = false;
    if (key == "when") {
      if (p.next(level, key, conn)) ; else assert(false);
      when = key; is_when=true;
    }
    else if (key == "relate") s.make_rules_set();
    else if (conn == "<==") {
      buff_stream bs;
      op = bs.create( op );
      s.add_rule( key, is_when ? when : "true", bs );
    }
  }
  return p;
}

// main parse method

template<typename I>
bool parse(I first, I last, sheet_t& s) {
  token_t level, key, conn;
  parser_t<I> p { first, last };
  while (p.next(level, key, conn)) {
    if (key == "input" or key == "interface") parse0(p, level, s);
    else if (key == "logic") parse2(p, level, s);
    else if (key == "output") parse1(p, level, s);
    else assert(false);
  }
  return true;
}

int main() {
  using it = std::istream_iterator<token_t>;
  it l; it f(std::cin);
  sheet_t s;
  assert(parse(f,l,s));

  return 0;
}
