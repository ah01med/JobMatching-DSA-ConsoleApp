#include "arrays/ArraySimilarity.h"
#include <string>
#include <cctype>


namespace {
static inline std::string to_lower(const std::string& s){
std::string out; out.reserve(s.size());
for(unsigned char c: s) out.push_back(static_cast<char>(std::tolower(c)));
return out;
}
static inline bool is_word_char(unsigned char c){ return std::isalnum(c); }
}


namespace arrayalgo {


void splitUniqueSorted(const std::string& s, DynamicArray<std::string>& out){
std::string lower = to_lower(s);
DynamicArray<std::string> tmp;
std::string cur; cur.reserve(16);
for(char ch: lower){
if(is_word_char(static_cast<unsigned char>(ch))) cur.push_back(ch);
else{ if(cur.size()>=2){ tmp.push_back(cur); } cur.clear(); }
}
if(cur.size()>=2) tmp.push_back(cur);


auto less_str = [](const std::string& a, const std::string& b){ return a<b; };
tmp.quicksort(less_str);


out.clear(); out.reserve(tmp.size());
std::string last;
for(std::size_t i=0;i<tmp.size();++i){
if(i==0 || tmp[i]!=last){ out.push_back(tmp[i]); last=tmp[i]; }
}
}


static double jaccard_from_sets(const DynamicArray<std::string>& A,
const DynamicArray<std::string>& B){
std::size_t i=0,j=0,inter=0,uni=0;
while(i<A.size() && j<B.size()){
if(A[i]==B[j]){ ++inter; ++uni; ++i; ++j; }
else if(A[i]<B[j]){ ++uni; ++i; }
else { ++uni; ++j; }
}
uni += (A.size()-i) + (B.size()-j);
if(uni==0) return 0.0;
return static_cast<double>(inter)/static_cast<double>(uni);
}


double jaccard(const DynamicArray<std::string>& A,
const DynamicArray<std::string>& B){
return jaccard_from_sets(A,B);
}


} // namespace arrayalgo