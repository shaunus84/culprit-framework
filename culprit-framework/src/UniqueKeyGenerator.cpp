#include "culprit-framework/UniqueKeyGenerator.h"

#include <atomic>

using culprit::framework::UniqueKeyGenerator;

std::atomic<int> UniqueKeyGenerator::nextUniqueKeyId{0};
