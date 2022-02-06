#pragma once

#include "model.hpp"

namespace yobtk {

/**
 * \brief Implements a basic ECS (Entity-Component-System) design
 *        where Components are static but Entities and Systems are dynamic.
 *        This is a shortcut to "yobtk::ecs::Model<2048, Ts ...>"
 * 
 * \param Ts List of types used in Components (must all be different).
 */
template <typename ... Ts>
using ECSModel = yobtk::ecs::Model<2048, Ts ...>;

}