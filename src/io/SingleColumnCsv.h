#pragma once
#include <string>
#include "arrays/DynamicArray.h"


namespace ioutils {
bool readSingleColumn(const std::string& path,
const std::string& expectedHeader,
DynamicArray<std::string>& out);
bool writeSingleColumn(const std::string& path,
const std::string& header,
const DynamicArray<std::string>& data);
}