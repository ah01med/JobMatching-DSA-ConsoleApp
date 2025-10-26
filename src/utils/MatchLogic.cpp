#include "utils/MatchLogic.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

// ---- helpers ----
static char toLowerChar(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

static std::string toLowerTrim(std::string s) {
    // trim left
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    // trim right
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    std::transform(s.begin(), s.end(), s.begin(), toLowerChar);
    return s;
}

// split by multiple delimiters: | , ; /
static std::vector<std::string> splitByAny(const std::string& s, const std::string& delims = "|,;/") {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (delims.find(c) != std::string::npos) {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
        }
        else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

static std::unordered_set<std::string> toSet(const std::string& raw) {
    std::unordered_set<std::string> s;
    for (auto& part : splitByAny(raw)) {
        auto t = toLowerTrim(part);
        if (!t.empty()) s.insert(t);
    }
    return s;
}

static int overlapScore(const std::unordered_set<std::string>& a,
    const std::unordered_set<std::string>& b) {
    int score = 0;
    for (const auto& k : a) if (b.count(k)) ++score;
    return score;
}

// ---- match logic ----
void MatchLogic::matchLinkedLists(const ResumeLinkedList& resumes,
    const JobLinkedList& jobs)
{
    struct JobView { int id; std::string title; std::unordered_set<std::string> need; };
    std::vector<JobView> jobVec;
    jobs.forEach([&](const JobLinkedList::Job& j) {
        jobVec.push_back({ j.id, j.title, toSet(j.required) });
        });

    if (jobVec.empty()) {
        std::cout << "[!] No jobs loaded. Use menu option 1.\n";
        return;
    }

    bool anyPrinted = false;
    resumes.forEach([&](const ResumeLinkedList::Resume& r) {
        auto have = toSet(r.skills);
        std::vector<std::pair<int, size_t>> scores; 
        for (size_t i = 0; i < jobVec.size(); ++i) {
            int s = overlapScore(have, jobVec[i].need);
            if (s > 0) scores.emplace_back(s, i);
        }
        if (scores.empty()) {
            std::cout << "Candidate: " << r.name << " — no matches.\n";
            return;
        }
        std::sort(scores.begin(), scores.end(), [](auto& a, auto& b) { return a.first > b.first; });
        std::cout << "Candidate: " << r.name << " matches:\n";
        size_t topk = std::min<size_t>(3, scores.size());
        for (size_t k = 0; k < topk; ++k) {
            const auto& jv = jobVec[scores[k].second];
            std::cout << "  - " << jv.title << "  (score: " << scores[k].first << ")\n";
        }
        anyPrinted = true;
        });

    if (!anyPrinted) std::cout << "[i] Matching finished. No overlapping skills found.\n";
}
