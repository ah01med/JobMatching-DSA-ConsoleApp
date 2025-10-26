#pragma once
#include <string>
#include <cstddef>

class ResumeLinkedList {
public:
    struct Resume {
        int         id{};
        std::string name;
        std::string skills;   
        std::string degree;
    };

private:
    struct Node {
        Resume data;
        Node* next;
        explicit Node(const Resume& r) : data(r), next(nullptr) {}
    };

    Node* head = nullptr;
    Node* tail = nullptr;
    size_t count = 0;

public:
    // ---- lifecycle ----
    ResumeLinkedList() = default;
    ~ResumeLinkedList();

    
    ResumeLinkedList(const ResumeLinkedList&) = delete;
    ResumeLinkedList& operator=(const ResumeLinkedList&) = delete;

    ResumeLinkedList(ResumeLinkedList&& other) noexcept;
    ResumeLinkedList& operator=(ResumeLinkedList&& other) noexcept;

    // ---- core API ----
    void insertResume(int id,
        const std::string& name,
        const std::string& skills,
        const std::string& degree);

    void displayResumes(int pageSize = 0) const;

    
    void mergeSortByName(bool verbose = false);

    // utils
    void   clear();
    size_t size() const { return count; }

    template<typename Fn>
    void forEach(Fn fn) const {
        for (Node* p = head; p; p = p->next) fn(p->data);
    }

    ResumeLinkedList clone() const;

private:
    static int  ciCmp(const std::string& a, const std::string& b);
    static char toLowerChar(char c);
};
