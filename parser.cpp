#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>

using token_t = std::string;

// key : -> set (level + key)
// key op -> key lookup | double | bool
// key ; -> key lookup | set key default value also terminated
// key <== val -> pair<key, lazy val>
// key , -> just continue
//
// key ( -> key = "when"
// key ) -> set predicate

struct bt
{
  std::vector<node> pool;
  using node_t = std::size_t;
  struct node {
    token_t key_;
    token_t conn_;
    std::vector<node_t> nodes;
  };

  node& node_(node_t idx) { return pool[idx]; }

  node_t push(node_t node, token_t key, token_t conn) {
    node_(node) =
  }

};

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
  using it = std::istream_iterator<token_t>;
  it l; it f(std::cin);
  while (f != l) { }

  return 0;
}
