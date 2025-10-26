#include "io/CsvReader.h"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

// ===== counters definition =====
int CsvReader::lastSkippedResumes = 0;
int CsvReader::lastSkippedJobs = 0;

namespace {

    static inline void stripBOM(std::string& s) {
        if (s.size() >= 3 &&
            (unsigned char)s[0] == 0xEF &&
            (unsigned char)s[1] == 0xBB &&
            (unsigned char)s[2] == 0xBF) {
            s.erase(0, 3);
        }
    }

    static std::string makeTitleFromSentence(const std::string& s) {
        std::istringstream iss(s);
        std::string words[6];
        int count = 0;
        std::string w;
        while (iss >> w) {
            if (count < 6) {
                words[count++] = w;
            }
            else break;
        }
        if (count == 0) return "Untitled";
        std::ostringstream out;
        for (int i = 0; i < count; ++i) {
            if (i) out << ' ';
            out << words[i];
        }
        return out.str();
    }

    static std::string extractSkillsFromSentence(const std::string& s) {
        static const char* keys[] = {
            "python","java","sql","excel","power bi","tableau","reporting",
            "machine learning","deep learning","nlp","pandas","statistics",
            "git","docker","spring boot","rest apis","system design","agile",
            "computer vision","tensorflow","keras","pytorch","mlops","cloud",
            "scrum","stakeholder management","product roadmap","user stories"
        };
        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](unsigned char c) { return (char)std::tolower(c); });

        std::ostringstream out;
        bool first = true;
        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
            const char* k = keys[i];
            if (lower.find(k) != std::string::npos) {
                if (!first) out << '|';
                out << k;
                first = false;
            }
        }
        return out.str();
    }

    static bool getlineCSV(std::istream& in, std::string& out) {
        out.clear();
        int c = in.peek();
        if (c == EOF) return false;

        if (c == '"') {
            in.get();
            bool inQuote = true;
            while (in.good()) {
                char ch = (char)in.get();
                if (!in.good()) break;
                if (inQuote) {
                    if (ch == '"') {
                        int nxt = in.peek();
                        if (nxt == '"') {
                            out.push_back('"');
                            in.get();
                        }
                        else {
                            inQuote = false;
                        }
                    }
                    else {
                        out.push_back(ch);
                    }
                }
                else {
                    if (ch == '\n') break;
                }
            }
            return true;
        }
        else {
            std::getline(in, out);
            return !out.empty() || in.good();
        }
    }

} 

bool CsvReader::loadResumes(const std::string& path, ResumeLinkedList& out) {
    lastSkippedResumes = 0;

    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[CsvReader] cannot open: " << path << "\n";
        return false;
    }

    out.clear();

    std::string line;
    if (!std::getline(f, line)) return false;
    stripBOM(line);

    int id = 1;
    int loaded = 0;

    while (true) {
        std::string text;
        if (!getlineCSV(f, text)) break;

        while (!text.empty() && (text.back() == '\r' || text.back() == '\n')) text.pop_back();

        if (text.empty()) { ++lastSkippedResumes; continue; }

        std::string name = "Candidate " + std::to_string(id);
        std::string degree = "";
        std::string skills = extractSkillsFromSentence(text);
        if (skills.empty()) skills = text;

        out.insertResume(id, name, skills, degree);
        ++id;
        ++loaded;
    }

    return loaded > 0;
}

bool CsvReader::loadJobs(const std::string& path, JobLinkedList& out) {
    lastSkippedJobs = 0;

    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[CsvReader] cannot open: " << path << "\n";
        return false;
    }

    out.clear();

    std::string line;
    if (!std::getline(f, line)) return false;
    stripBOM(line);

    int id = 1;
    int loaded = 0;

    while (true) {
        std::string desc;
        if (!getlineCSV(f, desc)) break;

        while (!desc.empty() && (desc.back() == '\r' || desc.back() == '\n')) desc.pop_back();

        if (desc.empty()) { ++lastSkippedJobs; continue; }

        std::string title = makeTitleFromSentence(desc);
        std::string required = extractSkillsFromSentence(desc);
        if (required.empty()) required = desc;

        out.insertJob(id, title, required);
        ++id;
        ++loaded;
    }

    return loaded > 0;
}
