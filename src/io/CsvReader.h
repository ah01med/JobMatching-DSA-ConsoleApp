#pragma once
#include <string>
#include "models/ResumeLinkedList.h"
#include "models/JobLinkedList.h"

namespace CsvReader {

    // Portable counters (header declares, .cpp defines)
    extern int lastSkippedResumes;
    extern int lastSkippedJobs;

    // Loaders return true if any rows were loaded.
    // They also update the counters above.
    bool loadResumes(const std::string& path, ResumeLinkedList& out);
    bool loadJobs(const std::string& path, JobLinkedList& out);

} // namespace CsvReader
