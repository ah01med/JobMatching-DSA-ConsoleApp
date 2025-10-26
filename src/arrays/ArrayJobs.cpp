#include "arrays/ArrayJobs.h"
#include "arrays/ArraySearch.h"
#include "arrays/ArraySimilarity.h"
#include "io/SingleColumnCsv.h"
#include <string>


namespace arrayjobs {


LoadStats load(Store& s){
s.jobs.clear(); s.resumes.clear();
bool ok1 = ioutils::readSingleColumn(s.jobsPath, "job_description", s.jobs);
bool ok2 = ioutils::readSingleColumn(s.resPath, "resume", s.resumes);
return LoadStats{ok1, ok2, s.jobs.size(), s.resumes.size()};
}


void insertJob(Store& s, const std::string& text){ s.jobs.push_back(text); }
void insertResume(Store& s, const std::string& text){ s.resumes.push_back(text); }


std::size_t searchJobsByKeyword(const Store& s, const std::string& kw,
DynamicArray<std::size_t>* sampleIdx,
std::size_t maxSample){
std::size_t hits=0;
if(sampleIdx) sampleIdx->clear();
for(std::size_t i=0;i<s.jobs.size();++i){
if(arrayalgo::containsKeywordCI(s.jobs[i], kw)){
if(sampleIdx && sampleIdx->size()<maxSample) sampleIdx->push_back(i);
++hits;
}
}
return hits;
}


std::size_t searchResumesByKeyword(const Store& s, const std::string& kw,
DynamicArray<std::size_t>* sampleIdx,
std::size_t maxSample){
std::size_t hits=0;
if(sampleIdx) sampleIdx->clear();
for(std::size_t i=0;i<s.resumes.size();++i){
if(arrayalgo::containsKeywordCI(s.resumes[i], kw)){
if(sampleIdx && sampleIdx->size()<maxSample) sampleIdx->push_back(i);
++hits;
}
}
return hits;
}


void topKJobsForResume(const Store& s, std::size_t resumeIndex1based, std::size_t K,
DynamicArray<Match>& outMatches){
outMatches.clear();
if(resumeIndex1based==0 || resumeIndex1based> s.resumes.size()) return;


const std::string& resumeText = s.resumes[resumeIndex1based-1];
DynamicArray<std::string> Rset; arrayalgo::splitUniqueSorted(resumeText, Rset);


struct Pair { std::size_t idx; double score; };
DynamicArray<Pair> scores;


for(std::size_t i=0;i<s.jobs.size();++i){
DynamicArray<std::string> Jset; arrayalgo::splitUniqueSorted(s.jobs[i], Jset);
double sc = arrayalgo::jaccard(Rset, Jset);
scores.push_back(Pair{i, sc});
}


auto more = [](const Pair& a, const Pair& b){ return a.score > b.score; };
scores.quicksort(more);


for(std::size_t i=0;i<K && i<scores.size(); ++i){
outMatches.push_back(Match{scores[i].idx, scores[i].score});
}
}


static std::size_t estimate_bytes(const DynamicArray<std::string>& arr){
std::size_t bytes = sizeof(arr);
for(std::size_t i=0;i<arr.size();++i) bytes += arr[i].capacity();
return bytes;
}


std::size_t approxBytes(const Store& s){
return estimate_bytes(s.jobs) + estimate_bytes(s.resumes);
}


bool save(const Store& s){
bool ok1 = ioutils::writeSingleColumn(s.jobsPath, "job_description", s.jobs);
bool ok2 = ioutils::writeSingleColumn(s.resPath, "resume", s.resumes);
return ok1 && ok2;
}


} // namespace arrayjobs