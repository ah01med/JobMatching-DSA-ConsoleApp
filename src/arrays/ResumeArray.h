#pragma once
#include <string>
#include <vector>
#include <ostream>

namespace array2 {

    struct Resume {
        int         id{};
        std::string name;
        std::string skills;   // we reuse this as the single-column text in your CSV
    };

    class ResumeArray {
    public:
        // Keep this reasonable to avoid huge memory use
        static const int kMax = 20000;

        ResumeArray();

        void insert(const Resume& r);

        // load single-column CSV (first row = header); returns true/false and count via outLoaded
        bool loadFromCsv(const std::string& path, int* outLoaded);

        const Resume* searchById(int id) const;

        // collect up to maxToCollect matching indexes; return total hits
        int filterBySkill(const std::string& skill,
            std::vector<int>& outIndexes,
            int maxToCollect) const;

        void displayTop(std::ostream& os, int top) const;

        // Sorting by name
        void bubbleSortByName();
        void quickSortByName();

    private:
        int partitionByName(int lo, int hi);
        void qsortByName(int lo, int hi);

        Resume data_[kMax];
        int    count_{ 0 };
    };

} // namespace array2
