// assume vars do not start with number
bool is_number(const token_t& key) { return key[0] > 47 && key[0] < 58;  } // 0..9

struct sheet_t {
  using table_t = std::map<token_t, value_t>;
  using relate_t = std::vector<helper::rule<sheet_t>>;
  using result_t = std::vector<std::pair<token_t, token_t>>;

  table_t tab { {"true", value_t(1)}, {"false", value_t(0)} };
  std::vector<relate_t> constraints;
  result_t result_;

  friend void result(sheet_t& s, result_t res) { s.result_ = std::move(res); }

  friend void make_rules_set(sheet_t& s) { s.constraints.emplace_back(relate_t()); }
  friend void add_rule(sheet_t& s, token_t key, token_t when, helper::buff_stream bf)
  { s.constraints.back().emplace_back( helper::rule<sheet_t>{key, when, bf, &s }); }

  friend iparser_t record_expr(sheet_t& s, const token_t& k, iparser_t p,
                                token_t& level, token_t& key, token_t& conn);

  friend value_t to_number(sheet_t& s, const token_t& key);
  friend value_t lookup(sheet_t& s, token_t k) { return s.tab[k]; }
  friend void record(sheet_t& s, const token_t& key, value_t val) { s.tab[key] = val;}

  table_t result_map() {
    auto f = begin(result_); auto l = end(result_); table_t m;
    while(f != l) { m[(*f).first] = tab[(*f).second]; f++; }
    return m;
  }
};

void record(sheet_t& s, const token_t& key, value_t val);
void make_rules_set(sheet_t& s);
void result(sheet_t& s, std::vector<std::pair<token_t, token_t>>);

template <typename T>
// require T is stream
void add_rule(sheet_t& s, token_t key, token_t when, T bf);

iparser_t record_expr(sheet_t& s, const token_t& k, iparser_t p,
                                token_t& level, token_t& key, token_t& conn) {
  helper::numeric_reducer<decltype(p), sheet_t> nr { p, &s };
  auto res = nr(level, key, conn);
  s.tab[k] = res.first;
  return res.second;
}

