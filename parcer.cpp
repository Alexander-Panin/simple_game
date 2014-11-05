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
  if (is_name(sheet_name, c) && is_match('{', c) 
      && is_data(c) && is_match('}',c)) return true;
  else return false;
}

bool is_data(code_point_t& c) {
  //data ::= { key, ':', map, data };
  if (is_key(c)) {
    return is_match(':', c) && is_map(c) && is_data(c);
  } else return true;
}

bool is_key(code_point_t& c, const char* key = "input") {
  //key ::= 'input' | 'interface' | 'logic' | 'output';
  if !(is_name(c)) return false;
  const char* key = register_lookup_name(); // 
  return strcmp("input", key) || strcmp("interface", key) 
         || strcmp("logic", key) || strcmp("output", key);
}

bool is_map(code_point_t& c) {
  //map ::= mlogic | moutput | minput | minterface;
  return is_minput(c) || is_minterface(c) || is_mlogic(c) || is_moutput(c);
}

bool is_minput(code_point_t& c, bool optional = false) {
  //minput ::= name, ':', expr, ';', { minput };
  if !(is_name(c)) return optional; 
  if !(is_match(':', c) && is_expr(c) && is_match(';', c)) return false; 
  store("input", register_lookup_name(), register_lookup_expr());
  return is_minput(c, true);
}

bool is_minterface(code_point_t& c, bool optional = false) {
  //minterface ::= name, ':', { expr }, ';', { minterface } ;
  if !(is_name(c)) return optional;
  if !(is_match(':', c)) return false;
  is_expr(c); if !(is_match(';', c)) return false;
  store("interface", register_lookup_name(), register_lookup_expr());
  return is_minterface(c, true);
}

bool is_mlogic(code_point_t& c) {
  //mlogic ::= ['when', '(', bexpr, ')'] 'relate' '{', constraint ,'}'


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



