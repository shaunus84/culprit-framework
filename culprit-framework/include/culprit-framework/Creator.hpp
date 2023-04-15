#pragma once

namespace culprit {
namespace framework {

template <typename... Args>
struct creator;

template <typename Type>
struct creator<Type> {
  using createdType = Type;
};

template <typename CreatedType, typename... Deps>
struct creator<CreatedType, Deps...> : creator<CreatedType> {
  using deps = creator<Deps...>;
};

}  // namespace framework
}  // namespace culprit
