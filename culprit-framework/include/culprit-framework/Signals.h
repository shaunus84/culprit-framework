#pragma once

#include "Signal.hpp"

namespace culprit {
namespace framework {

class EnterContextSignal : public EmptySignal {};
class ExitContextSignal : public EmptySignal {};

}  // namespace framework
}  // namespace culprit
