#include "models/ResumeLinkedList.h"
#include <iostream>
#include <algorithm>
#include <cctype>

char ResumeLinkedList::toLowerChar(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

int ResumeLinkedList::ciCmp(const std::string& a, const std::string& b) {
    const size_t n = (a.size() < b.size()) ? a.size() : b.size();
    for (size_t i = 0; i < n; ++i) {
        const char ca = toLowerChar(a[i]);
        const char cb = toLowerChar(b[i]);
        if (ca < cb) return -1;
        if (ca > cb) return 1;
    }
    if (a.size() < b.size()) return -1;
    if (a.size() > b.size()) return 1;
    return 0;
}

ResumeLinkedList::~ResumeLinkedList() { clear(); }

ResumeLinkedList::ResumeLinkedList(ResumeLinkedList&& other) noexcept {
    head = other.head;
    tail = other.tail;
    count = other.count;
    other.head = other.tail = nullptr;
    other.count = 0;
}

ResumeLinkedList& ResumeLinkedList::operator=(ResumeLinkedList&& other) noexcept {
    if (this == &other) return *this;
    clear();
    head = other.head;
    tail = other.tail;
    count = other.count;
    other.head = other.tail = nullptr;
    other.count = 0;
    return *this;
}

void ResumeLinkedList::clear() {
    Node* p = head;
    while (p) {
        Node* nxt = p->next;
        delete p;
        p = nxt;
    }
    head = tail = nullptr;
    count = 0;
}

void ResumeLinkedList::insertResume(int id,
    const std::string& name,
    const std::string& skills,
    const std::string& degree) {
    if (id <= 0 || name.empty()) return;

    Node* n = new Node(Resume{ id, name, skills, degree });
    n->next = nullptr;

    if (!head) {
        head = tail = n;
    }
    else {
        tail->next = n;
        tail = n;
    }
    ++count;
}

void ResumeLinkedList::displayResumes(int pageSize) const {
    if (!head) {
        std::cout << "[i] No resumes loaded.\n";
        return;
    }

    if (pageSize > 0) {
        std::cout << "---- Resumes (" << count << ")  [showing " << pageSize
            << " per page; 'n' = next, 'q' = quit] ----\n";
    }
    else {
        std::cout << "---- Resumes (" << count << ") ----\n";
    }

    const Node* p = head;
    int        printedOnPage = 0;
    long long  printedTotal = 0;

    while (p) {
        std::cout << "ID: " << p->data.id
            << " | Name: " << p->data.name
            << " | Degree: " << p->data.degree
            << " | Skills: " << p->data.skills << "\n";

        ++printedOnPage;
        ++printedTotal;

        if (pageSize > 0 && printedOnPage >= pageSize) {
            if (!p->next) break;

            std::cout << "--------------------------\n"
                << "[n] next page • [q] quit > ";
            char cmd{};
            if (!(std::cin >> cmd)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                break;
            }
            std::cin.ignore(10000, '\n');

            if (cmd == 'q' || cmd == 'Q') {
                std::cout << "[i] Stopped after " << printedTotal << " rows.\n";
                return;
            }
            printedOnPage = 0;
        }

        p = p->next;
    }

    std::cout << "--------------------------\n";
}

void ResumeLinkedList::mergeSortByName(bool /*verbose*/) {
    if (!head || !head->next) return;

    size_t length = 0;
    for (Node* p = head; p; p = p->next) ++length;

    Node dummy(Resume{ 0, "", "", "" });

    for (size_t step = 1; step < length; step <<= 1) {
        Node* cur = head;
        Node* tailM = &dummy;
        dummy.next = nullptr;

        while (cur) {
            Node* left = cur;
            size_t lcount = 1;
            while (cur->next && lcount < step) {
                cur = cur->next; ++lcount;
            }
            Node* right = cur->next;
            cur->next = nullptr;

            size_t rcount = 0;
            Node* rtail = right;
            while (rtail && rtail->next && rcount + 1 < step) {
                rtail = rtail->next; ++rcount;
            }

            Node* nextChunk = nullptr;
            if (rtail) {
                nextChunk = rtail->next;
                rtail->next = nullptr;
            }

            Node* a = left;
            Node* b = right;
            Node* merged = nullptr;
            Node** outNext = &merged;

            while (a && b) {
                if (ciCmp(a->data.name, b->data.name) <= 0) {
                    *outNext = a; a = a->next;
                }
                else {
                    *outNext = b; b = b->next;
                }
                outNext = &((*outNext)->next);
            }
            *outNext = (a ? a : b);

            tailM->next = merged;
            while (tailM->next) tailM = tailM->next;

            cur = nextChunk;
        }

        head = dummy.next;

        tail = head;
        while (tail && tail->next) tail = tail->next;
    }
}

ResumeLinkedList ResumeLinkedList::clone() const {
    ResumeLinkedList out;
    for (Node* p = head; p; p = p->next) {
        out.insertResume(p->data.id, p->data.name, p->data.skills, p->data.degree);
    }
    return out;
}
