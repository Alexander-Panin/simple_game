#include <iostream>
#include <iterator>

int main() {
  typedef std::istream_iterator<std::string> it;
  it l; it f(std::cin);
  while(f != l) {
    std::cout << *f << "<--" << std::endl; 
    ++f;
  }
  
  return 0;
}
