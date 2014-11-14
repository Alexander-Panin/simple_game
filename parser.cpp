#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>

using token_t = std::string;
std::map<std::string, bool> btab;
std::map<std::string, double> dtab;

// key : -> set (level + key)
// key op -> key lookup | double | bool
// key ; -> key lookup | set key default value also terminated
// key <== val -> pair<key, lazy val>
// key , -> just continue
//
// ???
// key ( -> key should eq when
// key ) -> just continue

// lookup (dtab, btab) functor
// vector<key> depends on level
// vector<constraints>, constrains = vector<relate>, relate = pair<key, lazy val>
// in quick-alg compare relate consistents
// when output apply lazy val and return val

bool is_bool(...);
bool is_number(...);

template <typename I>
struct parser
// require value_type(I) == token_t
// && InputIterator(I)
{
  I f;
  I l;

  bool parse_step(token_t& level, token_t& key, token_t& conn)
  { return parse_step0(level) && parse_step0(key) && parse_step0(conn); }

  bool parse_step0(token_t& t) {
    if (f == l) return false;
    t = *f; ++f; return true;
  }
};

struct num_tree
{
  std::vector<node> t;
  struct node {
    token_t oper;
    double d;
  };
  ... push(...) { }
  ... reduce(...) { }
};

int main() {
  using it = std::istream_iterator<std::string>;
  it l; it f(std::cin);
  while (f != l) { }

  return 0;
}
