#pragma once

namespace culprit {
namespace framework {

class UniqueKeyGenerator {
 public:
  template <class Key>
  static int Get() {
    return static_cast<int>(typeid(Key).hash_code());
  }
};

}  // namespace framework
}  // namespace culprit
