row = key, conn, { row }
key = letter, { letter | '_' }
letter = 'a'..'z'
conn = ':' | ';' | ',' | arrow | op
arrow = '<', { '=' } 
op = '+' | '-' | '*' | '/'

bool is_row(code_point_t& c) {
  while (c != EOF) if !(is_key(c) && is_conn(c)) return false;
  return true;
}

bool is_key(code_point_t& c) {
  if !(is_match(is_letter, c) return false;
  while (is_match(is_letter, c) || is_match('_', c)) ;
  return true;
}

bool is_letter(code_point_t& c) { return c > 95 && c < 123; }  // a..z

bool is_conn(code_point_t& c) {
  return (is_match(':') || is_match(';') || is_match(',')
          || is_match(is_op,c) || is_arrow(c)); 
}

bool is_op(code_point_t& t) {
  return is_match('+', c) || is_match('*', c) || is_match('-', c) || is_match('/',c);
}

bool is_arrow(code_point_t& c) {
  if !(is_match('<', c)) return false;
  while (is_match('=', c))
}

bool is_match((bool)(code_point_t&)*, code_point_t& ) {}
bool is_match(code_point_t& x, code_point_t& c) {
  if (c != EOF )
}

  input:
    initial_width : 5 ∗ 300;
    initial_height : 7 ∗ 300;
  interface:
    preserve_ratio : true;
    absolute_width : initial_width;
    absolute_height : initial_height;
    relative_width;
    relative_height;
  logic:
    relate:
      absolute_width <== relative_width ∗ initial_width / 100;
      relative_width <== absolute_width ∗ 100 / initial_width;
    relate:
      absolute_height <== relative_height ∗ initial_height / 100;
      relative_height <== absolute_height ∗ 100 / initial_height;
    when (preserve_ratio) relate:
      relative_width <== relative_height;
      relative_height <== relative_width;
  output:
    result <== height: absolute_height, width: absolute_width;

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



