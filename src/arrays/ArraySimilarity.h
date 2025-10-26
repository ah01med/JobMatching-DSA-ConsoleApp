#pragma once
#include <string>
#include "arrays/DynamicArray.h"


namespace arrayalgo {
void splitUniqueSorted(const std::string& s, DynamicArray<std::string>& out);
double jaccard(const DynamicArray<std::string>& A,
const DynamicArray<std::string>& B);
}