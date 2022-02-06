#pragma once

#include <set>
#include <functional>

namespace yobtk::ecs {

/**
 * \brief Represents a System. Holds a function and a set of entities that can
 *        be processed.
 * 
 * \param S Signature type.
 * \param E Entity type.
 * \param M Model type.
 */
template <typename S, typename E, typename M>
class System
{
public:
    /**
     * \brief Process function signature.
     */
    using ProcessF = std::function<void(const std::set<E>&, M&)>;
    
    /**
     * \brief Creates a System.
     * 
     * \param signature System's signature.
     * \param f Process function. Must have the following signature:
     *          (const std::set<Entity>&, Model&) -> void
     */
    System(S signature, ProcessF f)
    : signature_ { signature }
    , f_ { f }
    {}

    /**
     * \brief Gets the system's signature.
     * 
     * \return System's signature.
     */
    S signature()
    { return signature_; }

    /**
     * \brief Inserts an entity in the set.
     */
    void insert(E e)
    { entities_.insert(e); }

    /**
     * \brief Remove an entity from the set.
     */
    void remove(E e)
    { entities_.erase(e); }

    /**
     * \brief Processes the entity set.
     * 
     * \param m The model so that the process function have access to the entities' data.
     */
    void process(M& m)
    { f_(entities_, m); }

private:
    S signature_;
    std::set<E> entities_;
    ProcessF f_;
};

}