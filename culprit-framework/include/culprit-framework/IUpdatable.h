#pragma once

namespace culprit {
namespace framework {

template <typename Updatable>
class IUpdatable {
 public:
  void HandleEvents(const void* pEvent) {
    static_cast<Updatable*>(this)->HandleEvents(pEvent);
  }
  void PreUpdate() { static_cast<Updatable*>(this)->PreUpdate(); }
  void Update(double deltaTime) {
    static_cast<Updatable*>(this)->Update(deltaTime);
  }
  void PostUpdate() { static_cast<Updatable*>(this)->PostUpdate(); }
};
}  // namespace framework
}  // namespace culprit
