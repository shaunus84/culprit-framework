#pragma once

#include "Signal.hpp"

namespace culprit {
namespace framework {

class EnterContextSignal : public EmptySignal {};
class ExitContextSignal : public EmptySignal {};
class PreUpdateContextSignal : public EmptySignal {};
class UpdateContextSignal : public EmptySignal {};
class PostUpdateContextSignal : public EmptySignal {};

}  // namespace framework
}  // namespace culprit
