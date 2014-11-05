//obj ::= 'sheet', name, '{', data, '}';
//data ::= { key, ':', map, data };
//key ::= 'input' | 'interface' | 'logic' | 'output';
//map ::= mlogic | moutput | minput | minterface;
//minput ::= name, ':', expr, ';', { minput };
//minterface ::= name, ':', { expr }, ';', { minterface } ;
//moutput ::= name, '<==', kv, { moutput } ;
//mlogic ::= ['when', '(', bexpr, ')'] 'relate' '{', constraint ,'}'
//constraint ::= name, '<==', expr, ';', { constraint };
//expr ::= bexpr | nexpr;
//nexpr ::= number | number, op, expr;
//bexpr ::= bool | bool, bop, bexpr;
//bop ::= '||' | '&&';
//op ::= '+' | '_' | '*' | '\' | '%';
//number ::= \d | var;
//bool ::= 'true' | 'false' | var ;
//name ::= letter, { letter | '_' }

using code_point_t = char;

bool is_sheet(code_point_t& c) {
  //obj ::= 'sheet', name, '{', data, '}';
  for (const char& x: "sheet") if !(is_match(x, c)) return false;
  std::string sheet_name; // token, after move in the table
  if (is_name(sheet_name, c) && is_match('{', c) && is_data(c) && is_match('}',c)) return true;
  else return false;
}

bool is_data(code_point_t& c) {
  //data ::= { key, ':', map, data };
  std::string key;
  if (is_key(key, c)) {
    if !(is_match(':', c)) && is_map(key, c) return false;
    return is_data(c);
  } else return true;
}

bool is_map(code_point_t& c) {
  //map ::= mlogic | moutput | minput | minterface;
  return is_minput(c) || is_minterface(c) || is_mlogic(c) || is_moutput(c);
}

bool is_minput(code_point_t& c, bool optional = false) {
  //minput ::= name, ':', expr, ';', { minput };
  if (is_name(c)) {
    if !(is_match(':', c) && is_expr(c) && is_match(';', c)) return false;
    return is_minput(c, true);
  } else return false || optional;
}


template <typename I>
// require InputIterator(I)
// && valueType(I) = code_point_t
struct parser
{
  template <typename T>
  bool is_production(T& value);
  I f_m;
  I l_m;
}



