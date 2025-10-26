#include "JobLinkedList.h"
#include <iostream>
#include <algorithm>
#include <cctype>

static std::string toLowerCopy(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return out;
}

JobLinkedList::JobLinkedList() : head(nullptr) {}
JobLinkedList::~JobLinkedList() { clear(); }

void JobLinkedList::insertJob(int id, const std::string& title, const std::string& required) {
    if (id <= 0 || title.empty() || required.empty()) {
        std::cerr << "[JobLinkedList] Invalid data, skipping.\n";
        return;
    }

    Node* cur = head;
    while (cur) {
        if (cur->data.id == id) {
            std::cerr << "[JobLinkedList] Duplicate Job ID: " << id << std::endl;
            return;
        }
        cur = cur->next;
    }

    Node* newNode = new Node{ {id, title, required}, nullptr };
    if (!head)
        head = newNode;
    else {
        Node* p = head;
        while (p->next) p = p->next;
        p->next = newNode;
    }
}

JobLinkedList::Job* JobLinkedList::searchById(int id) const {
    Node* cur = head;
    while (cur) {
        if (cur->data.id == id)
            return &cur->data;
        cur = cur->next;
    }
    return nullptr;
}

JobLinkedList::Job* JobLinkedList::searchByKeyword(const std::string& keyword) const {
    if (keyword.empty()) return nullptr;
    std::string k = toLowerCopy(keyword);
    Node* cur = head;
    while (cur) {
        std::string t = toLowerCopy(cur->data.title);
        std::string r = toLowerCopy(cur->data.required);
        if (t.find(k) != std::string::npos || r.find(k) != std::string::npos)
            return &cur->data;
        cur = cur->next;
    }
    return nullptr;
}

void JobLinkedList::displayJobs(int pageSize) const {
    if (!head) {
        std::cout << "[JobLinkedList] No jobs found.\n";
        return;
    }

    if (pageSize > 0) {
        std::cout << "---- Jobs [showing " << pageSize
            << " per page; 'n' = next, 'q' = quit] ----\n";
    }
    else {
        std::cout << "---- Jobs ----\n";
    }

    const Node* p = head;
    int printedOnPage = 0;
    long long printedTotal = 0;

    while (p) {
        std::cout << p->data.id << " | " << p->data.title
            << " | " << p->data.required << std::endl;

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

void JobLinkedList::clear() {
    while (head) {
        Node* tmp = head;
        head = head->next;
        delete tmp;
    }
}
