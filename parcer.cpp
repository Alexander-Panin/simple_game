//row = key, conn, { row }
//key = letter, { letter | '_' }
//letter = 'a'..'z'
//conn = ':' | ';' | ',' | '(' | ')' | arrow | op
//arrow = '<', { '=' } 
//op = '+' | '-' | '*' | '/'

bool is_row(code_point_t& c) {
  while (f != l) if !(is_key(c) && is_conn(c)) return false;
  return true;
}

bool is_key(code_point_t& c) {
  if !(is_match(is_letter, c) return false;
  while (is_match(is_letter, c) || is_match('_', c)) ;
  return true;
}

bool is_letter(code_point_t x) { return x > 95 && x < 123; }  // a..z

bool is_conn(code_point_t& c) {
  return (is_match(':') || is_match(';') || is_match(',') || is_match('(')
          || is_match(')') || is_match(is_op,c) || is_arrow(c)); 
}

bool is_op(code_point_t& t) {
  return is_match('+', c) || is_match('*', c) || is_match('-', c) || is_match('/',c);
}

bool is_arrow(code_point_t& c) {
  if !(is_match('<', c)) return false;
  while (is_match('=', c)) {}
}

bool is_match(bool (*p)(code_point_t), code_point_t& c) {
  while(f =! l && (*f == ' ' || *f == '\t' || *f == '\n') ++f;
  if (f == l || p(*f) != x) return false;
  c = x; f++; return true; 
}

bool is_match(code_point_t& x, code_point_t& c) {
  while(f =! l && (*f == ' ' || *f == '\t' || *f == '\n') ++f;
  if (f == l || *f != x) return false;
  c = x; f++; return true; 
}

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
