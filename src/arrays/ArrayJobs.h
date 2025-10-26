#pragma once
#include <string>
#include "arrays/DynamicArray.h"


namespace arrayjobs {


struct Store {
DynamicArray<std::string> jobs;
DynamicArray<std::string> resumes;
std::string jobsPath{"data/job_description.csv"};
std::string resPath{"data/resume.csv"};
};


struct LoadStats { bool okJobs{false}; bool okRes{false}; std::size_t nJobs{0}; std::size_t nRes{0}; };


LoadStats load(Store& s);
void insertJob(Store& s, const std::string& text);
void insertResume(Store& s, const std::string& text);


// Returns hit count; optionally fills up to maxSample indices for preview (0‑based)
std::size_t searchJobsByKeyword(const Store& s, const std::string& kw,
DynamicArray<std::size_t>* sampleIdx = nullptr,
std::size_t maxSample = 5);


std::size_t searchResumesByKeyword(const Store& s, const std::string& kw,
DynamicArray<std::size_t>* sampleIdx = nullptr,
std::size_t maxSample = 5);


struct Match { std::size_t jobIndex; double score; };
void topKJobsForResume(const Store& s, std::size_t resumeIndex1based, std::size_t K,
DynamicArray<Match>& outMatches);


std::size_t approxBytes(const Store& s);
bool save(const Store& s);


} // namespace arrayjobs