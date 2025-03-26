#pragma once

#include <type_traits>

namespace culprit {
namespace framework {

template <typename Derived>
class IUpdatable {
 public:
  // The base versions do nothing.
  void HandleEvents(const void* /*pEvent*/) {}
  void PreUpdate() {}
  void Update(double /*deltaTime*/) {}
  void PostUpdate() {}

  // The public interface that calls the derived override if one exists.
  void doHandleEvents(const void* pEvent) {
    // Compare the member function pointers.
    if constexpr (!std::is_same_v<decltype(&Derived::HandleEvents),
                                  decltype(&IUpdatable::HandleEvents)>) {
      static_cast<Derived*>(this)->HandleEvents(pEvent);
    }
  }
  void doPreUpdate() {
    if constexpr (!std::is_same_v<decltype(&Derived::PreUpdate),
                                  decltype(&IUpdatable::PreUpdate)>) {
      static_cast<Derived*>(this)->PreUpdate();
    }
  }
  void doUpdate(double deltaTime) {
    if constexpr (!std::is_same_v<decltype(&Derived::Update),
                                  decltype(&IUpdatable::Update)>) {
      static_cast<Derived*>(this)->Update(deltaTime);
    }
  }
  void doPostUpdate() {
    if constexpr (!std::is_same_v<decltype(&Derived::PostUpdate),
                                  decltype(&IUpdatable::PostUpdate)>) {
      static_cast<Derived*>(this)->PostUpdate();
    }
  }
};
}  // namespace framework
}  // namespace culprit
