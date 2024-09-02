#pragma once

#include <atomic>

#ifdef unique_key_generator_EXPORTS
/*Enabled as "export" while compiling the dll project*/
#define DLLEXPORT __declspec(dllexport)
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DLLEXPORT __declspec(dllimport)
#endif

namespace culprit {
namespace framework {

class DLLEXPORT UniqueKeyGenerator {
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
