#include <iostream>
#include <iterator>
#include <algorithm>
#include <map>

using token_t = std::string;
std::map<std::string, bool> btab; // insert "true" and "false"
std::map<std::string, double> dtab;
std::string section_level; 
std::vector<related> rel;

bool is_section(token_t& t) {
  std::vector<std::string> v = { "input", "interface", "logic", "output" };
  return std::find(begin(v), end(v), t) != end(v);
}

template <typename I>
struct parser 
// require value_type(I) == token_t
// && InputIterator(I)
{
  I f; 
  I l;  
  parse_step(token_t& t1, token_t& t2) {
    if (f == l) return l;
    t1 = *f; ++f;
    if (f == l) return l;
    t2 = *f; ++f;
    return 
  }
};

// section ::= 'input' | 'interface | 'logic' | 'output'
// kv ::= 

... input_or_interface_section(...) {
  while(parse_step(key, conn)) {
    if (is_section(key)) return;
    if (conn == ";") dtab.insert(key, default_val()); continue;
    if (conn != ":") return false;
    tmp_key = key;
    if (parse_step(key, conn)) ; else return false;  
    if (is_bool(conn, tmp_bool)) btab.insert(tmp_key, tpm_bool); continue;
    dtab.insert(tmp_key, calc_section(key, conn)); 
  }  
}

... logic_section(...) {
  while (parse_step(key, conn)) {
    if (is_section(key)) return;
    if (key == "when" && conn == "(") { 
      parse_step(key, conn);
      bool flag = true; btoken = key; if (conn != ")") return false;
      continue;
    }
    if (key == "relate" && conn == ":") relate_clause();
  }
}

struct num_tree 
{
  std::vector<double> t;
  struct node { 
    token_t oper;
    double d;
  };
  ... push(key, oper) {
  }
  ... reduce() {
    is_var()
  }
};

is_bool(...) { 
  if (btab.find(key) != "") str = btab.find(key); return true;
  return false;
}

calc_section(key, conn) {
  num_tree t;
  double d = dtab.find(key) || to_double(key)
  do {
    if (conn == ";") return t.reduce();
    t.push(d, conn); 
  } while(parse_step(key, conn)) 
}

.. ...(...) {
  token_t var;
  token_t val;
  while(parse_step(key, conn)) {
    if (is_section(key) && conn == ":") { section_level = key; continue; }
    if (section_level == "input")
    if (conn == ":") { var = key; continue; }
    if (conn == ";") { itab.insert( std::make_pair(var, val)); continue; }
  }
}


int main() {
  using it = std::istream_iterator<std::string>;
  it l; it f(std::cin);
  while (f != l) { 
    
  }

  return 0;
}
