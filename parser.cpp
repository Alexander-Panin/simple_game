#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>
#include <functional>

using token_t = std::string;
using value_t = double;

// key : -> set (level + key)
// key op -> key lookup | double | bool
// key ; -> key lookup | set key default value also terminated
// key <== val -> pair<key, lazy val>
// key , -> just continue
//
// key ( -> key = "when"
// key ) -> set predicate


bool is_bool(...);
bool is_number(...);

std::vector<token_t> first_level_keys { "output", "logic", "interface", "input" };
token_t key1, key2, key3;
//std::vector<std::function<value_t()>> ops { std::plus, std::mult };
std::vector<std::function<value_t()>> ops {
  [](value_t a, value_t b) { return a+b; },
  [](value_t a, value_t b) { return a*b; }
}

template <typename T>
void parse_section(token_t& level, token_t& key, token_t& conn) {
  token_t prev_level = level;
  token_t k_acc;
  do {
    if (parse_step(level, key, conn)); else return;
    if (conn == ":") k_acc = key; continue;
    if (conn == ";") { record(key, value_t()); continue; } // default value
    record(k_acc, num_reducer(level, key, conn));
  } while (level != prev_level);
  parse_header (level, key, conn);
}

value_t num_reducer(level, key, conn) {
  value_t acc = (conn == ("*" || "/") ? 1 : 0);
  token_t prev_op;
  do {
    acc = num_reduce_step(key, conn, prev_op, acc);
    prev_op = conn;
    parse_step(level, key, conn);
  } while (conn != ";");

  return num_reduce_step(key, prev_op, prev_op, acc);
}

value_t value_t num_reduce_step(const token_t& key, const token_t& conn,
                                  const token_t& prev_op, const value_t& acc) {
  value_t tmp = is_number(key) ? to_double(key) : lookup(key);
  if (prev_op == "-") tmp *= -1;
  if (prev_op == "/") tmp = 1/tmp;
  auto op = (op == "+" || op == "-") ? ops[0] : ops[1];
  return op(acc, tmp);
}

void parse_header(token_t& level, token_t& key, conn) {
  if ( key == stack.back() && conn == ":") stack.pop_back(); else return;
  parse_section<key>(level, key, conn);
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
