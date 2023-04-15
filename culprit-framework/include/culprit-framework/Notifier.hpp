#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <iomanip>
#include <vector>

namespace culprit {
namespace framework {

class Notifier {
 public:
  virtual ~Notifier() = default;

  template <class T>
  int Attach(T* instance, void (T::*memberFunction)()) {
    assert(instance != nullptr);
    assert(memberFunction != nullptr);

    const int attachToken = GetAttachToken();

    auto handler_ident =
        std::make_tuple(static_cast<void*>(instance), attachToken,
                        std::bind(std::mem_fn(memberFunction), instance));

    handlers.emplace_back(handler_ident);
    return attachToken;
  }

  int Attach(void (*function)()) {
    assert(function != nullptr);

    const int attachToken = GetAttachToken();

    auto handler_ident = std::make_tuple(static_cast<void*>(nullptr),
                                         attachToken, std::bind(function));

    handlers.emplace_back(handler_ident);
    return attachToken;
  }

  void Detach(int attachToken) {
    auto attach_tokens_match = [attachToken](handler i) {
      return attachToken == std::get<1>(i);
    };

    handlers.erase(
        std::remove_if(handlers.begin(), handlers.end(), attach_tokens_match),
        handlers.end());
  }

  void DetachAll(void* instance) {
    assert(instance != nullptr);

    auto handler_instance_matches = [instance](handler i) {
      return instance == std::get<0>(i);
    };

    handlers.erase(std::remove_if(handlers.begin(), handlers.end(),
                                  handler_instance_matches),
                   handlers.end());
  }

  void NotifyObservers() const {
    auto tempHandlers = handlers;
    for (auto& handler : tempHandlers) {
      if (std::get<2>(handler) != nullptr) {
        std::get<2>(handler)();
      }
    }
  }

 private:
  int GetAttachToken() { return nextUniqueToken++; }

 private:
  // <pointer to observing class, function identifier, function>
  using handler = std::tuple<void*, int, std::function<void()>>;
  std::vector<handler> handlers;
  std::atomic<int> nextUniqueToken;
};

}  // namespace framework
}  // namespace culprit
