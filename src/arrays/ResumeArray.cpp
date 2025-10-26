#include "ResumeArray.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace array2 {

    ResumeArray::ResumeArray() = default;

    void ResumeArray::insert(const Resume& r) {
        if (count_ < kMax) {
            data_[count_] = r;
            ++count_;
        }
    }

    // ---------------- CSV load ----------------
    bool ResumeArray::loadFromCsv(const std::string& path, int* outLoaded) {
        std::ifstream f(path);
        if (!f.is_open()) {
            std::cerr << "[Array2] ERROR: cannot open " << path << "\n";
            if (outLoaded) *outLoaded = 0;
            return false;
        }

        std::string line;
        // skip header
        if (!std::getline(f, line)) {
            if (outLoaded) *outLoaded = 0;
            return false;
        }

        int loaded = 0;
        while (std::getline(f, line) && count_ < kMax) {
            if (line.empty()) continue;

            Resume r;
            r.id = count_ + 1;                            // synthetic id
            r.name = "Candidate_" + std::to_string(r.id);
            r.skills = line;                                  // keep whole line as text

            insert(r);
            ++loaded;
        }

        if (outLoaded) *outLoaded = loaded;
        return true;
    }

    // ---------------- searching ----------------
    const Resume* ResumeArray::searchById(int id) const {
        if (id <= 0 || id > count_) return nullptr;
        for (int i = 0; i < count_; ++i)
            if (data_[i].id == id) return &data_[i];
        return nullptr;
    }

    int ResumeArray::filterBySkill(const std::string& skill,
        std::vector<int>& outIndexes,
        int maxToCollect) const {
        outIndexes.clear();
        int hits = 0;
        for (int i = 0; i < count_; ++i) {
            if (data_[i].skills.find(skill) != std::string::npos) {
                ++hits;
                if ((int)outIndexes.size() < maxToCollect)
                    outIndexes.push_back(i);
            }
        }
        return hits;
    }

    // ---------------- display ----------------
    static std::string preview(const std::string& s, std::size_t n = 80) {
        if (s.size() <= n) return s;
        return s.substr(0, n) + "...";
    }

    void ResumeArray::displayTop(std::ostream& os, int top) const {
        const int limit = std::min(top, count_);
        for (int i = 0; i < limit; ++i) {
            os << "[" << data_[i].id << "] "
                << "Name=\"" << data_[i].name << "\"  "
                << "Text=\"" << preview(data_[i].skills, 90) << "\"\n";
        }
    }

    // ---------------- sorting by name ----------------
    void ResumeArray::bubbleSortByName() {
        auto t0 = std::chrono::high_resolution_clock::now();

        int swaps = 0;
        for (int i = 0; i < count_ - 1; ++i) {
            bool changed = false;
            for (int j = 0; j < count_ - i - 1; ++j) {
                if (data_[j].name > data_[j + 1].name) {
                    std::swap(data_[j], data_[j + 1]);
                    changed = true;
                    ++swaps;
                }
            }
            if (!changed) break;
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << "[Array2] BubbleSort(name) time: " << ms
            << " ms, swaps: " << swaps
            << "  | memory ~" << (count_ * sizeof(Resume)) / 1024 << " KB\n";
    }

    int ResumeArray::partitionByName(int lo, int hi) {
        int mid = lo + (hi - lo) / 2;
        std::swap(data_[mid], data_[hi]);
        const std::string& pivot = data_[hi].name;
        int i = lo - 1;
        for (int j = lo; j < hi; ++j) {
            if (data_[j].name <= pivot) {
                ++i;
                std::swap(data_[i], data_[j]);
            }
        }
        std::swap(data_[i + 1], data_[hi]);
        return i + 1;
    }

    void ResumeArray::qsortByName(int lo, int hi) {
        if (lo < hi) {
            int p = partitionByName(lo, hi);
            qsortByName(lo, p - 1);
            qsortByName(p + 1, hi);
        }
    }

    void ResumeArray::quickSortByName() {
        if (count_ <= 1) return;
        auto t0 = std::chrono::high_resolution_clock::now();
        qsortByName(0, count_ - 1);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        int approxDepth = (int)std::max(1.0, std::log2(std::max(1, count_)));
        std::cout << "[Array2] QuickSort(name) time: " << ms
            << " ms  | memory ~" << (count_ * sizeof(Resume)) / 1024
            << " KB  | stack~" << approxDepth << " levels\n";
    }

} // namespace array2
