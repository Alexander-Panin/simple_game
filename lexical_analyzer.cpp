#include <iostream>
#include <iterator>
#include <cassert>

// row ::= key, conn, { row }
// key ::= name | rnumber
// name ::= letter, { letter | '_' }
// rnumber ::= number | number, '.', number
// number ::= digit, { digit }
// letter ::= 'a'..'z'
// digit ::= '0'..'9'
// conn ::= ':' | ';' | ',' | '(' | ')' | arrow | op
// arrow ::= '<', { '=' }
// op ::= '+' | '-' | '*' | '/'

using code_point_t = char;
using token_t = std::string;

bool is_letter(code_point_t x) { return x > 96 && x < 123; }  // a..z
bool is_op(code_point_t x) { return x == '+' || x == '-' || x == '*' || x == '/'; }
bool is_digit(code_point_t x) { return x > 47 && x < 58; } // 0..9

template <typename I>
// require value_type(I) = code_point_t
//   && InputIterator(I)
struct lexical_analyzer
{
  I f; I l;
  std::size_t level;

  bool is_row(std::ostream& cout) {
    code_point_t c;
    while (f != l) {
      token_t key, conn;
      if (is_key(c, key) && is_conn(c, conn)) ; else return false;
      cout << level << ' ' << key << ' ' << conn << std::endl;
      // if (*f == 'n')
    }
    return true;
  }

  bool is_key(code_point_t& c, token_t& result) {
    return is_name(c, result) || is_rnumber(c, result);
  }

  bool is_name(code_point_t& c, token_t& result) {
    if (is_match(is_letter, c)) ; else return false;
    result += c; while (is_match(is_letter, c) || is_match('_', c)) result += c;
    return true;
  }

  bool is_rnumber(code_point_t& c, token_t& result) {
    if (is_number(c, result)) ; else return false;
    if (is_match('.', c)) {
      result += c;
      if (is_number(c, result)) ; else return false;
    }
    return true;
  }

  bool is_number(code_point_t& c, token_t& result) {
    if (is_match(is_digit, c)) ; else return false;
    result += c; while (is_match(is_digit, c)) result += c;
    return true;
  }

  bool is_conn(code_point_t& c, token_t& result) {
    if (is_match(':', c) || is_match(';', c) || is_match(',', c) || is_match('(', c)
        || is_match(')', c) || is_match(is_op, c)) { result += c; return true; }
    return is_arrow(c, result);
  }

  bool is_arrow(code_point_t& c, token_t& result) {
    if (is_match('<', c)) ; else return false;
    result += c; while (is_match('=', c)) result += c;
    return true;
  }

  bool is_match(bool (*p)(code_point_t), code_point_t& c) {
    skip_whitespaces();
    if (f == l || !p(*f)) return false;
    c = *f; f++; return true;
  }

  bool is_match(code_point_t x, code_point_t& c) {
    skip_whitespaces();
    if (f == l || *f != x) return false;
    c = x; f++; return true;
  }

  void skip_whitespaces() {
    skip_step("\t ");
    if (f !=l && *f == '\n') { skip_step("\n"); level = skip_step("\t \n"); }
  }

  int skip_step(std::string pat, std::size_t c = 0) {
    while(f != l && find(begin(pat), end(pat), *f) != end(pat)) { ++f; ++c; }
    return c;
  }

};

int main() {
  using it = std::istream_iterator<code_point_t>;
  std::cin.unsetf(std::ios::skipws);
  it eos; it f(std::cin);
  lexical_analyzer<it> la { f, eos, 0 };
  assert(la.is_row(std::cout));
  return 0;
}

// *** example ***
//
//  input:
//    initial_width : 5 ∗ 300;
//    initial_height : 7 ∗ 300;
//  interface:
//    preserve_ratio : true;
//    absolute_width : initial_width;
//    absolute_height : initial_height;
//    relative_width;
//    relative_height;
//  logic:
//    relate:
//      absolute_width <== relative_width ∗ initial_width / 100;
//      relative_width <== absolute_width ∗ 100 / initial_width;
//    relate:
//      absolute_height <== relative_height ∗ initial_height / 100;
//      relative_height <== absolute_height ∗ 100 / initial_height;
//    when (preserve_ratio) relate:
//      relative_width <== relative_height;
//      relative_height <== relative_width;
//  output:
//    result <== height: absolute_height, width: absolute_width;
