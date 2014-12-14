using token_t = std::string;
using value_t = double;

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
