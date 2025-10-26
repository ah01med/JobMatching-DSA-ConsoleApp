#ifndef JOB_LINKED_LIST_H
#define JOB_LINKED_LIST_H

#include <string>
#include <iostream>

class JobLinkedList {
public:
    struct Job {
        int id;
        std::string title;
        std::string required;
    };

private:
    struct Node {
        Job data;
        Node* next;
    };
    Node* head;

public:
    JobLinkedList();
    ~JobLinkedList();

    void insertJob(int id, const std::string& title, const std::string& required);

    JobLinkedList::Job* searchById(int id) const;
    JobLinkedList::Job* searchByKeyword(const std::string& keyword) const;

    void displayJobs(int pageSize = 0) const;  // 0 = no paging
    void clear();

    template<typename Fn>
    void forEach(Fn fn) const {
        for (Node* p = head; p; p = p->next)
            fn(p->data);
    }
};

#endif
