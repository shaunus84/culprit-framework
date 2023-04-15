#pragma once

#include <tuple>
#include <typeinfo>

#include "Notifier.hpp"

namespace culprit {
namespace framework {

class SignalBase : public Notifier {};

template <class... Ts>
class Signal : public SignalBase {
 public:
  void Dispatch(Ts&&... args) {
    _params = std::make_tuple(std::forward<Ts>(args)...);
    NotifyObservers();
  }

  template <size_t I = 0,
            typename Enable =
                std::enable_if_t<I<sizeof...(Ts)>> const auto GetParam() {
    return std::get<I>(_params);
  }

 private:
  std::tuple<Ts...> _params;
};

using EmptySignal = Signal<>;

}  // namespace framework
}  // namespace culprit