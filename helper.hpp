namespace helper {

  struct buff_stream {
    std::vector<std::pair<token_t, token_t>> buff;

    template <typename Op>
    Op create(Op next) {
      buff.reserve(16); // avg length of expr
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
      if (conn_ == "/") { key = 1/key; conn_ = "*"; }
      if (conn_ == "+") acc_ += key_;
      if (conn_ == "*") key *= key_;
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
};

