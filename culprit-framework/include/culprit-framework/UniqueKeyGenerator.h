#pragma once

#include <atomic>

namespace culprit {
namespace framework {

class UniqueKeyGenerator {
 public:
  template <class Key>
  static int Get() {
    static int keyId = nextUniqueKeyId++;
    return keyId;
  }

 private:
  static std::atomic<int> nextUniqueKeyId;
};

}  // namespace framework
}  // namespace culprit
