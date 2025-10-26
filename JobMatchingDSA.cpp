
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <chrono>

#include "models/JobLinkedList.h"
#include "models/ResumeLinkedList.h"
#include "io/CsvReader.h"
#include "ResumeArray.h"   

#pragma comment(linker, "/STACK:16777216")


// ---------- utils ----------
static std::string lower_copy(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

static std::string preview(const std::string& s, std::size_t n = 80) {
    if (s.size() <= n) return s;
    return s.substr(0, n - 3) + "...";
}

static void print_hr(const std::string& title = std::string(), char ch = '-', int w = 60) {
    if (!title.empty()) {
        std::cout << "\n--- " << title << " ";
        int left = std::max(0, w - (int)title.size() - 4);
        for (int i = 0; i < left; ++i) std::cout << ch;
        std::cout << "\n";
    }
    else {
        for (int i = 0; i < w; ++i) std::cout << ch;
        std::cout << "\n";
    }
}

static bool ask_yes_no(const char* prompt = "Show 3 more? (y/n): ") {
    std::cout << "\n" << prompt;
    std::string a; std::getline(std::cin, a);
    return !a.empty() && (a[0] == 'y' || a[0] == 'Y');
}

// -------- count helpers ----------
static std::size_t countJobs(const JobLinkedList& jobs) {
    std::size_t n = 0;
    jobs.forEach([&](const JobLinkedList::Job&) { ++n; });
    return n;
}

static void print_job_row(const JobLinkedList::Job& j) {
    std::cout << "ID=" << j.id
        << "  Title=\"" << preview(j.title, 42) << "\""
        << "   Required=\"" << preview(j.required, 42) << "\"\n";
}

static void print_resume_row(const ResumeLinkedList::Resume& r) {
    std::cout << "[" << r.id << "]  Name=\"" << preview(r.name, 28) << "\""
        << "   Skills=\"" << preview(r.skills, 60) << "\"\n";
}


static void split_tokens_bar_space_heap(const std::string& s,
    std::string* out, int maxN, int* count) {
    *count = 0;
    std::string tok;
    for (char ch : s) {
        if (ch == '|' || ch == ',' || std::isspace(static_cast<unsigned char>(ch))) {
            if (!tok.empty()) {
                if (*count < maxN) out[(*count)++] = lower_copy(tok);
                tok.clear();
            }
        }
        else {
            tok.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }
    if (!tok.empty() && *count < maxN) out[(*count)++] = lower_copy(tok);
}

// simple Jaccard using heap buffers 
static double jaccard_skills(const std::string& a, const std::string& b) {
    const int MAXT = 512;                   
    std::string* A = new std::string[MAXT];
    std::string* B = new std::string[MAXT];
    int na = 0, nb = 0;
    split_tokens_bar_space_heap(a, A, MAXT, &na);
    split_tokens_bar_space_heap(b, B, MAXT, &nb);
    if (na == 0 && nb == 0) { delete[] A; delete[] B; return 0.0; }

    // count unique sizes
    int uniq = 0;
    for (int i = 0; i < na; ++i) {
        bool dup = false; for (int k = 0; k < i; ++k) if (A[k] == A[i]) { dup = true; break; }
        if (!dup) ++uniq;
    }
    for (int j = 0; j < nb; ++j) {
        bool dup = false; for (int k = 0; k < j; ++k) if (B[k] == B[j]) { dup = true; break; }
        if (!dup) ++uniq;
    }

    int inter = 0;
    for (int i = 0; i < na; ++i) {
        bool ai_unique = true; for (int k = 0; k < i; ++k) if (A[k] == A[i]) { ai_unique = false; break; }
        if (!ai_unique) continue;
        bool inB = false; for (int j = 0; j < nb; ++j) if (A[i] == B[j]) { inB = true; break; }
        if (inB) ++inter;
    }
    int uni = uniq - inter;
    delete[] A; delete[] B;
    if (uni <= 0) return 0.0;
    return (double)inter / (double)uni;
}

// pointer array sorters
static void sort_resume_ptrs_by_name(const ResumeLinkedList::Resume** arr, int n) {
    for (int i = 1; i < n; ++i) {
        const ResumeLinkedList::Resume* key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j]->name > key->name) { arr[j + 1] = arr[j]; --j; }
        arr[j + 1] = key;
    }
}

static void sort_job_ptrs_by_title(const JobLinkedList::Job** arr, int n) {
    for (int i = 1; i < n; ++i) {
        const JobLinkedList::Job* key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j]->title > key->title) { arr[j + 1] = arr[j]; --j; }
        arr[j + 1] = key;
    }
}

//  PERF HELPERS  


static void perf_copy_list_to_array(const ResumeLinkedList& ll,
    ResumeLinkedList::Resume* out, size_t n) {
    size_t i = 0;
    ll.forEach([&](const ResumeLinkedList::Resume& r) {
        if (i < n) out[i++] = r;
        });
}

// In place bubble sort by name 
static void perf_array_bubble_by_name(ResumeLinkedList::Resume* a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (a[j].name > a[j + 1].name) {
                ResumeLinkedList::Resume t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}


static int perf_part_by_name(ResumeLinkedList::Resume* a, int lo, int hi) {
    const std::string& pivot = a[(lo + hi) / 2].name;
    int i = lo, j = hi;
    while (i <= j) {
        while (a[i].name < pivot) ++i;
        while (a[j].name > pivot) --j;
        if (i <= j) {
            ResumeLinkedList::Resume t = a[i]; a[i] = a[j]; a[j] = t;
            ++i; --j;
        }
    }
    return i;
}

// Iterative quicksort by name — O(n log n)
static void perf_array_quick_by_name(ResumeLinkedList::Resume* a, int n) {
    if (n <= 1) return;
    struct Range { int lo, hi; };
    Range stack[64];               
    int sp = 0; stack[sp++] = { 0, n - 1 };
    while (sp) {
        Range rg = stack[--sp];
        int lo = rg.lo, hi = rg.hi;
        if (lo >= hi) continue;
        int idx = perf_part_by_name(a, lo, hi);
        if (idx - 1 - lo > hi - idx) { stack[sp++] = { lo, idx - 1 }; stack[sp++] = { idx, hi }; }
        else { stack[sp++] = { idx, hi };     stack[sp++] = { lo, idx - 1 }; }
    }
}

// Linear search by exact name on array
static const ResumeLinkedList::Resume*
perf_array_find_by_name(const ResumeLinkedList::Resume* a, int n, const std::string& name) {
    for (int i = 0; i < n; ++i) if (a[i].name == name) return &a[i];
    return nullptr;
}

// Linear search by exact name on linked list
static const ResumeLinkedList::Resume*
perf_list_find_by_name(const ResumeLinkedList& ll, const std::string& name) {
    const ResumeLinkedList::Resume* hit = nullptr;
    ll.forEach([&](const ResumeLinkedList::Resume& r) {
        if (!hit && r.name == name) hit = &r;
        });
    return hit;
}

// ---------- Menu ----------
static void printMenu() {
    std::cout << "\n=== Job Matching  ===\n"
        << "1) Load CSV data\n"
        << "2) Show resumes\n"
        << "3) Filter resumes by SKILL and sort by name (top 3 + jobs)\n"
        << "4) Match resumes -> jobs\n"
        << "5) Show jobs by TITLE keyword (paged x3)\n"
        << "6) Search job by ID\n"
        << "7) Search jobs by KEYWORD (paged x3)\n"
        << "8) Performance comparison (LL vs Array)\n"
        << "9) Array (load → quicksort → top 5)\n"
        << "0) Exit\n> ";
}

int main() {
    JobLinkedList     jobs;
    ResumeLinkedList  resumes;

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool running = true;
    while (running) {
        printMenu();
        int choice = -1;
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
        std::cin.ignore(10000, '\n');

        switch (choice) {
        case 1: {
            bool okR = CsvReader::loadResumes("data/resume.csv", resumes);
            bool okJ = CsvReader::loadJobs("data/job_description.csv", jobs);
            (void)okR; (void)okJ;

            print_hr("LOAD SUMMARY");
            std::cout << "[Done] resumes loaded : " << resumes.size() << "\n";
            std::cout << "[Done] jobs loaded    : " << countJobs(jobs) << "\n";

            int shown = 0;
            std::cout << "\n// show two examples from resumes\n";
            resumes.forEach([&](const ResumeLinkedList::Resume& r) {
                if (shown < 2) { print_resume_row(r); ++shown; }
                });
            shown = 0;
            std::cout << "\n// show two examples from jobs\n";
            jobs.forEach([&](const JobLinkedList::Job& j) {
                if (shown < 2) { print_job_row(j); ++shown; }
                });
            break;
        }

        case 2: {
            print_hr("Show resumes (5 examples)");
            int shown = 0;
            resumes.forEach([&](const ResumeLinkedList::Resume& r) {
                if (shown < 5) { print_resume_row(r); ++shown; }
                });

            std::size_t printed = 5;
            while (printed < resumes.size()) {
                std::cout << "\nShow 5 more? (y/n): ";
                std::string ans; std::getline(std::cin, ans);
                if (ans.empty() || (ans[0] != 'y' && ans[0] != 'Y')) break;

                std::size_t i = 0;
                resumes.forEach([&](const ResumeLinkedList::Resume& r) {
                    if (i >= printed && i < printed + 5) print_resume_row(r);
                    ++i;
                    });
                printed += 5;
            }
            break;
        }

        case 3: {
            print_hr("Filter resumes by SKILL and sort by name");
            std::cout << "Skill keyword (e.g., SQL, Python, Excel): ";
            std::string kw; std::getline(std::cin, kw); kw = lower_copy(kw);

            int count = 0;
            resumes.forEach([&](const ResumeLinkedList::Resume& r) {
                std::string s = lower_copy(r.skills);
                if (s.find(kw) != std::string::npos) ++count;
                });

            if (count == 0) { std::cout << "[i] 0 resumes matched \"" << kw << "\".\n"; break; }

            const ResumeLinkedList::Resume** rows = new const ResumeLinkedList::Resume * [count];
            int idx = 0;
            resumes.forEach([&](const ResumeLinkedList::Resume& r) {
                if (idx < count) {
                    std::string s = lower_copy(r.skills);
                    if (s.find(kw) != std::string::npos) rows[idx++] = &r;
                }
                });

            sort_resume_ptrs_by_name(rows, count);

            std::cout << "[i] " << count << " resumes matched \"" << kw << "\".\n";
            std::cout << "---- Sorted by name (top 3) ----\n";
            int top = (count < 3 ? count : 3);
            for (int i = 0; i < top; ++i) { std::cout << "  " << (i + 1) << ") "; print_resume_row(*rows[i]); }

            print_hr("Jobs requiring the same skill");
            int jcount = 0;
            jobs.forEach([&](const JobLinkedList::Job& j) {
                std::string req = lower_copy(j.required);
                if (req.find(kw) != std::string::npos) ++jcount;
                });

            if (jcount == 0) { std::cout << "[i] No job required \"" << kw << "\".\n"; delete[] rows; break; }

            const JobLinkedList::Job** matches = new const JobLinkedList::Job * [jcount];
            int jidx = 0;
            jobs.forEach([&](const JobLinkedList::Job& j) {
                if (jidx < jcount) {
                    std::string req = lower_copy(j.required);
                    if (req.find(kw) != std::string::npos) matches[jidx++] = &j;
                }
                });

            int i = 0, page = 1;
            while (i < jcount) {
                int take = ((jcount - i) < 3 ? (jcount - i) : 3);
                std::cout << "\n--- Results page " << page << " -----------------------------\n";
                std::cout << "ID     Title                                   Required\n";
                std::cout << "----------------------------------------------------------\n";
                for (int k = 0; k < take; ++k) print_job_row(*matches[i + k]);
                i += take; ++page;
                if (i < jcount && !ask_yes_no()) break;
            }

            delete[] matches;
            delete[] rows;
            break;
        }

        case 4: {
            print_hr("Match resumes -> jobs (demo)");
            std::cout << "Skill keyword to pick a sample resume (e.g., SQL): ";
            std::string kw; std::getline(std::cin, kw); kw = lower_copy(kw);

            const ResumeLinkedList::Resume* chosen = nullptr;
            resumes.forEach([&](const ResumeLinkedList::Resume& r) {
                if (!chosen) {
                    std::string s = lower_copy(r.skills);
                    if (s.find(kw) != std::string::npos) chosen = &r;
                }
                });
            if (!chosen) { std::cout << "[i] No resume contained \"" << kw << "\".\n"; break; }

            int jcount = (int)countJobs(jobs);
            if (jcount == 0) { std::cout << "[i] No jobs to match.\n"; break; }

            struct Scored { const JobLinkedList::Job* j; double score; };
            Scored* scored = new Scored[jcount];
            int sidx = 0;
            jobs.forEach([&](const JobLinkedList::Job& j) {
                double s = jaccard_skills(chosen->skills, j.required);
                if (sidx < jcount) { scored[sidx].j = &j; scored[sidx].score = s; ++sidx; }
                });

            // selection sort by descending score
            for (int a = 0; a < sidx - 1; ++a) {
                int best = a;
                for (int b = a + 1; b < sidx; ++b)
                    if (scored[b].score > scored[best].score) best = b;
                if (best != a) { Scored t = scored[a]; scored[a] = scored[best]; scored[best] = t; }
            }

            std::cout << "\n--- Match (1 resume -> top 3 jobs) ---\n";
            std::cout << "Resume  [" << chosen->id << "]  Name=\"" << chosen->name
                << "\"  Skills=\"" << preview(chosen->skills, 68) << "\"\n";
            int take = (sidx < 3 ? sidx : 3);
            for (int i2 = 0; i2 < take; ++i2) {
                const Scored& s = scored[i2];
                std::cout << "  " << (i2 + 1) << ") [Job " << s.j->id << "]  score=" << s.score
                    << "  Title=\"" << preview(s.j->title, 40) << "\"\n"
                    << "     Required=\"" << preview(s.j->required, 60) << "\"\n";
            }

            delete[] scored;
            break;
        }

        case 5: {
            print_hr("Show jobs by TITLE keyword");
            std::cout << "Title keyword: ";
            std::string kw; std::getline(std::cin, kw); kw = lower_copy(kw);

            int jcount = 0;
            jobs.forEach([&](const JobLinkedList::Job& j) {
                std::string t = lower_copy(j.title);
                if (t.find(kw) != std::string::npos) ++jcount;
                });
            if (jcount == 0) { std::cout << "[i] No jobs matched that title keyword.\n"; break; }

            const JobLinkedList::Job** found = new const JobLinkedList::Job * [jcount];
            int fidx = 0;
            jobs.forEach([&](const JobLinkedList::Job& j) {
                if (fidx < jcount) {
                    std::string t = lower_copy(j.title);
                    if (t.find(kw) != std::string::npos) found[fidx++] = &j;
                }
                });
            sort_job_ptrs_by_title(found, jcount);

            int i = 0, page = 1;
            while (i < jcount) {
                int take = ((jcount - i) < 3 ? (jcount - i) : 3);
                std::cout << "\n--- Results page " << page << " -----------------------------\n";
                std::cout << "ID     Title                                   Required\n";
                std::cout << "----------------------------------------------------------\n";
                for (int k = 0; k < take; ++k) print_job_row(*found[i + k]);
                i += take; ++page;
                if (i < jcount && !ask_yes_no()) break;
            }
            delete[] found;
            break;
        }

        case 6: {
            std::cout << "Enter Job ID: ";
            int id{}; if (!(std::cin >> id)) { std::cin.clear(); std::cin.ignore(10000, '\n'); std::cout << "[x] invalid input.\n"; break; }
            std::cin.ignore(10000, '\n');
            const auto* j = jobs.searchById(id);
            if (!j) { std::cout << "[i] Not found.\n"; break; }
            print_hr("Job by ID"); print_job_row(*j);
            break;
        }

        case 7: {
            print_hr("Search jobs by KEYWORD (paged x3)");
            std::cout << "Keyword (title/required): ";
            std::string kw; std::getline(std::cin, kw); kw = lower_copy(kw);

            int rcount = 0;
            jobs.forEach([&](const JobLinkedList::Job& j) {
                std::string t = lower_copy(j.title);
                std::string r = lower_copy(j.required);
                if (t.find(kw) != std::string::npos || r.find(kw) != std::string::npos) ++rcount;
                });
            if (rcount == 0) { std::cout << "[i] No jobs matched \"" << kw << "\".\n"; break; }

            const JobLinkedList::Job** results = new const JobLinkedList::Job * [rcount];
            int ridx = 0;
            jobs.forEach([&](const JobLinkedList::Job& j) {
                if (ridx < rcount) {
                    std::string t = lower_copy(j.title);
                    std::string r = lower_copy(j.required);
                    if (t.find(kw) != std::string::npos || r.find(kw) != std::string::npos) results[ridx++] = &j;
                }
                });
            sort_job_ptrs_by_title(results, rcount);

            int i = 0, page = 1;
            while (i < rcount) {
                int take = ((rcount - i) < 3 ? (rcount - i) : 3);
                std::cout << "\n--- Results page " << page << " -----------------------------\n";
                std::cout << "ID     Title                                   Required\n";
                std::cout << "----------------------------------------------------------\n";
                for (int k = 0; k < take; ++k) print_job_row(*results[i + k]);
                i += take; ++page;
                if (i < rcount && !ask_yes_no()) break;
            }
            delete[] results;
            break;
        }

        case 8: {
            print_hr("Performance comparison (LL vs Array) — STL-free");

            const size_t n = resumes.size();
            if (n == 0) { std::cout << "[i] Load resumes first (option 1).\n"; break; }

            ResumeLinkedList ll_clone = resumes.clone();

            ResumeLinkedList::Resume* arr_bubble = new ResumeLinkedList::Resume[n];
            ResumeLinkedList::Resume* arr_quick = new ResumeLinkedList::Resume[n];
            perf_copy_list_to_array(resumes, arr_bubble, n);
            perf_copy_list_to_array(resumes, arr_quick, n);

            using clock = std::chrono::high_resolution_clock;

            auto t0 = clock::now(); ll_clone.mergeSortByName(false); auto t1 = clock::now();
            auto ll_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

            auto b0 = clock::now(); perf_array_bubble_by_name(arr_bubble, (int)n); auto b1 = clock::now();
            auto bs_ms = std::chrono::duration_cast<std::chrono::milliseconds>(b1 - b0).count();

            auto q0 = clock::now(); perf_array_quick_by_name(arr_quick, (int)n); auto q1 = clock::now();
            auto qs_ms = std::chrono::duration_cast<std::chrono::milliseconds>(q1 - q0).count();

            std::cout << "[Sort] LinkedList merge sort : " << ll_ms << " ms\n";
            std::cout << "[Sort] Array bubble sort     : " << bs_ms << " ms\n";
            std::cout << "[Sort] Array quick sort      : " << qs_ms << " ms\n";

            std::string needle = (n ? arr_quick[n - 1].name : std::string());
            auto s0 = clock::now(); auto* lhit = perf_list_find_by_name(resumes, needle); auto s1 = clock::now();
            auto a0 = clock::now(); auto* ahit = perf_array_find_by_name(arr_quick, (int)n, needle); auto a1 = clock::now();
            auto ll_us = std::chrono::duration_cast<std::chrono::microseconds>(s1 - s0).count();
            auto ar_us = std::chrono::duration_cast<std::chrono::microseconds>(a1 - a0).count();
            std::cout << "[Find] LinkedList (linear)   : " << ll_us << " µs  " << (lhit ? "(found)" : "(miss)") << "\n";
            std::cout << "[Find] Array (linear)        : " << ar_us << " µs  " << (ahit ? "(found)" : "(miss)") << "\n";

            const size_t node_ptr = sizeof(void*);
            const size_t resume_sz = sizeof(ResumeLinkedList::Resume);
            size_t ll_bytes = n * (resume_sz + node_ptr);
            size_t arr_bytes = n * (resume_sz);
            std::cout << "[Mem] LinkedList ~ " << (ll_bytes / 1024) << " KB  (" << resume_sz << " + " << node_ptr << " bytes per node)\n";
            std::cout << "[Mem] Array      ~ " << (arr_bytes / 1024) << " KB  (" << resume_sz << " bytes per element)\n";
            std::cout << "      (String internals/allocator overhead ignored for both.)\n";

            delete[] arr_bubble; delete[] arr_quick;
            break;
        }

        case 9: {
            print_hr("Array demo (load → quicksort → top 5)");
            static array2::ResumeArray arr;  
            int loaded = 0;

            arr = array2::ResumeArray(); 
            if (!arr.loadFromCsv("data/resume.csv", &loaded) || loaded == 0) {
                std::cout << "[x] Failed to load any rows from data/resume.csv\n";
                break;
            }
            std::cout << "[Array] loaded: " << loaded
                << " rows (kMax=" << array2::ResumeArray::kMax << ")\n";

            auto t0 = std::chrono::high_resolution_clock::now();
            arr.quickSortByName();
            auto t1 = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

            std::cout << "[Array] quickSortByName time: " << ms << " ms\n";
            std::cout << "---- Top 5 by name ----\n";
            arr.displayTop(std::cout, 5);
            break;
        }

        case 0: running = false; std::cout << "Goodbye.\n"; break;
        default: std::cout << "Invalid option.\n"; break;
        }
    }
    return 0;
}
