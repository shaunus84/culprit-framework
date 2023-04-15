#pragma once

namespace culprit {
namespace framework {
class IUpdatable {
 public:
  virtual ~IUpdatable() {}
  virtual void PreUpdate(){};
  virtual void Update(double deltaTime) = 0;
  virtual void PostUpdate(){};
};
}  // namespace framework
}  // namespace culprit
