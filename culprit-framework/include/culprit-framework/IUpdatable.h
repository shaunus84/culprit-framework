#pragma once

namespace culprit {
namespace framework {
class IUpdatable {
 public:
  virtual ~IUpdatable() {}
  virtual void HandleEvents(const void* pEvent) {}
  virtual void PreUpdate() {}
  virtual void Update(double deltaTime) {}
  virtual void PostUpdate() {}
};
}  // namespace framework
}  // namespace culprit
