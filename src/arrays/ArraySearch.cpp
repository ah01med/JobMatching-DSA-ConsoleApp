#include "arrays/ArraySearch.h"
#include "arrays/DynamicArray.h"
#include <string>
#include <cctype>


namespace {
static inline std::string to_lower(const std::string& s){
std::string out; out.reserve(s.size());
for(unsigned char c: s) out.push_back(static_cast<char>(std::tolower(c)));
return out;
}
}


namespace arrayalgo {


bool containsKeywordCI(const std::string& text, const std::string& pattern){
if(pattern.empty()) return true;
std::string T = to_lower(text);
std::string P = to_lower(pattern);


DynamicArray<int> lps; lps.reserve(P.size());
for(std::size_t i=0;i<P.size();++i) lps.push_back(0);
int len=0;
for(std::size_t i=1;i<P.size();){
if(P[i]==P[len]){ lps[i]=++len; ++i; }
else if(len!=0){ len = lps[len-1]; }
else { lps[i]=0; ++i; }
}
std::size_t i=0,j=0;
while(i<T.size()){
if(T[i]==P[j]){ ++i; ++j; if(j==P.size()) return true; }
else{ if(j!=0) j = lps[j-1]; else ++i; }
}
return false;
}


} // namespace arrayalgo