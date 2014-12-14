namespace sections {

  struct input { };
  struct interface { };
  template <typename T>
  iparser_t parse(const T& x, const token_t& l, iparser_t p, sheet_t& s) {
    token_t level, key, conn, tmp_key;
    bool default_val = true;
    while (!p.is_match(l) && p(level, key, conn)) {
      if (conn == ":") { tmp_key = key; default_val = false; continue; }
      if (conn == ";" && default_val) { record(s, key, value_t()); continue; }
      p = record_expr(s, tmp_key, p, level, key, conn); default_val = true;
    }
    return p;
  }

  struct parser_t {
    template <typename T>
    parser_t(T x) : self_(new model<T>(std::move(x))) {}
    friend iparser_t parse_(const parser_t& ps, const token_t& l,
                                    iparser_t p, sheet_t& s) {
      return ps.self_->parse_(l,p,s);
    }
    struct concept_t {
      virtual ~concept_t() = default;
      virtual iparser_t parse_(const token_t& l, iparser_t p, sheet_t& s) const = 0;
    };

    template <typename T>
    struct model : concept_t {
      model(T x) : data_(std::move(x)) {}
      iparser_t parse_(const token_t& l, iparser_t p, sheet_t& s) const
      { return parse(data_, l, p, s); }
      T data_;
    };
    std::unique_ptr<concept_t> self_;
  };

  struct output { };
  iparser_t parse(const output& x, const token_t& l, iparser_t p, sheet_t &s) {
    token_t level, key, conn, tmp_key;
    std::vector<std::pair<token_t, token_t>> m;
    while (!p.is_match(l) && p(level, key, conn)) {
      if (conn == ":") tmp_key = key;
      else if (conn == "," || conn == ";") m.emplace_back(std::make_pair(tmp_key, key));
    }
    result(s, m);
    return p;
  }

  struct logic { };
  iparser_t parse(const logic&, const token_t& l, iparser_t p, sheet_t& s) {
    token_t level, key, conn, when;
    while (!p.is_match(l) && p(level, key, conn)) {
      bool is_when = false;
      if (key == "when") {
        if (p(level, key, conn)) ; else assert(false); // require the same level
        when = key; is_when=true;
      }
      else if (key == "relate") make_rules_set(s);
      else if (conn == "<==") {
        helper::buff_stream bs;
        p = bs.create( p );
        add_rule(s, key, is_when ? when : "true", bs );
      }
    }
    return p;
  }
};
