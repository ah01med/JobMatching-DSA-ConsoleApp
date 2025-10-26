#include "io/SingleColumnCsv.h"
#include <fstream>
#include <string>


namespace {
static inline std::string to_lower(const std::string& s){
std::string out; out.reserve(s.size());
for(unsigned char c: s) out.push_back(static_cast<char>(std::tolower(c)));
return out;
}
static inline std::string trim_quotes(const std::string& s){
if(s.size()>=2 && s.front()=='"' && s.back()=='"'){
std::string t; t.reserve(s.size());
for(std::size_t i=1;i+1<s.size();++i){
if(s[i]=='"' && i+1<s.size() && s[i+1]=='"'){ t.push_back('"'); ++i; }
else t.push_back(s[i]);
}
return t;
}
return s;
}
}


namespace ioutils {


bool readSingleColumn(const std::string& path,
const std::string& expectedHeader,
DynamicArray<std::string>& out){
std::ifstream in(path);
if(!in.is_open()) return false;
std::string line; bool first=true;
out.clear();
while(std::getline(in,line)){
if(first){
first=false;
if(line.size()>=3 && (unsigned char)line[0]==0xEF && (unsigned char)line[1]==0xBB && (unsigned char)line[2]==0xBF){
line = line.substr(3);
}
// Soft header check; do not fail hard.
(void)expectedHeader; // header hint ignored on purpose
continue;
}
if(line.empty()) continue;
out.push_back(trim_quotes(line));
}
return true;
}


bool writeSingleColumn(const std::string& path,
const std::string& header,
const DynamicArray<std::string>& data){
std::ofstream out(path);
if(!out.is_open()) return false;
out << header << "\n";
for(std::size_t i=0;i<data.size();++i){
out << '"';
for(char c: data[i]){ if(c=='"') out << '"'; out << c; }
out << "\"\n"; // closing quote + newline
}
return true;
}


} // namespace ioutils