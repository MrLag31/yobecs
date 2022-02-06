#pragma once

#include <bitset>
#include <map>

#include "utils.hpp"
#include "accessMatrix.hpp"
#include "component.hpp"
#include "wrappedHandle.hpp"
#include "system.hpp"

namespace yobtk::ecs {

/**
 * \brief Implements a basic ECS (Entity-Component-System) design
 *        where Components are static but Entities and Systems are dynamic.
 * 
 * \param N  Parameter for the AccessMatrix system. Used as the number of 
 *           items per block.
 * \param Ts List of types used in Components (must all be different).
 */
template <std::size_t N, typename ... Ts>
class Model
{
/* MISC */
private:
    template <typename T>
    static constexpr auto typeId_ = yobtk::utils::indexVariadicTypePack<T, Ts ...>;

/* ACCESS MATRIX */
private:
    using AccessMatrix_ = AccessMatrix<N, sizeof...(Ts)>;

    AccessMatrix_ accessMatrix_;

/* ENTITY */
public:
    /**
     * \brief Represents an Entity for the user.
     */
    using Entity = WrappedHandle<typename AccessMatrix_::Index>;

    /**
     * \brief Creates a new Entity.
     * 
     * \return A new entity. Beware that it might be
     *         the same as an already deleted entity.
     */
    auto createEntity()
    {
        Entity e (accessMatrix_.make());
        spawnedEntities_.insert(e);
        insertInSystems_(e, Signature_());
        return e;
    }

    /**
     * \brief Deletes an Entity.
     * 
     * \param e The entity to be removed. All of its data
     *          will be unaccessible.
     */
    void removeEntity(Entity e)
    {
        (checkedRemove_<Ts>(e), ...); 
        accessMatrix_.free(*e);
        spawnedEntities_.erase(e);
        removeFromSystems_(e, Signature_());
    }

private:
    std::set<Entity> spawnedEntities_;

    template <typename T>
    auto& getAccess_(Entity e)
    { return accessMatrix_.get(*e, typeId_<T>); }

    template <typename T>
    auto hasAccess_(Entity e)
    { return accessMatrix_.has(*e, typeId_<T>); }

    template <typename T>
    void resetAccess_(Entity e)
    { accessMatrix_.reset(*e, typeId_<T>); }

/* COMPONENTS */
public:
    /**
     * \brief Inserts the Entity e inside the Component of type T.
     * 
     * \param T   The type of the Component.
     * \param e   The Entity to be inserted.
     * \param val An optional default value.
     */
    template <typename T>
    void insert(Entity e, const T& val = {})
    {
        auto a = getComponent_<T>().insert(e, val);
        getAccess_<T>(e) = a;
        insertInSystems_(e, computeSignature_(e));
    }

    /**
     * \brief Removes the Entity e from the Component of type T.
     * 
     * \param T The type of the Component.
     * \param e The Entity to be removed.
     */
    template <typename T>
    void remove(Entity e)
    {
        auto a = getAccess_<T>(e);
        auto repE = getComponent_<T>().remove(a);
        resetAccess_<T>(e);
    
        if (repE != e)
        { getAccess_<T>(repE) = a; }

        removeFromSystems_(e, computeSignature_<T>());
    }

    /**
     * \brief Retrieves the data of an Entity e from the Component of
     *        type T.
     * 
     * \param T The type of the Component.
     * \param e The Entity of interest.
     * 
     * \return A reference to the stored data.
     */
    template <typename T>
    auto& access(Entity e)
    { return getComponent_<T>().access(getAccess_<T>(e)); }

private:
    std::tuple<Component<Ts, Entity> ...> components_;

    template<typename T>
    auto& getComponent_()
    { return std::get<typeId_<T>>(components_); }

    template <typename T>
    void checkedRemove_(Entity e)
    {
        if (hasAccess_<T>(e))
        { remove<T>(e); }
    }

/* SIGNATURES */
private:
    using Signature_ = std::bitset<sizeof...(Ts)>;

    template <typename ... Us>
    static auto computeSignature_()
    {
        static const auto s = (
            Signature_()
            | ... |
            Signature_().set(typeId_<Us>));
        return s;
    }

    auto computeSignature_(Entity e)
    {
        return (
            Signature_()
            | ... |
            Signature_().set(typeId_<Ts>, hasAccess_<Ts>(e))
        );
    }

/* SYSTEMS */
private:
    using System_       = System<Signature_, Entity, Model>;
    using SystemPtr_    = std::unique_ptr<System_>;

public:
    /**
     * \brief Represents a System for the user.
     */
    using SystemHandle = WrappedHandle<typename SystemPtr_::pointer>;

    /**
     * \brief Creates a new System attached to the types Us from the function f.
     * 
     * \param Us Set of types that the System is attached to.
     * \param f  Function called to process entities. It must have the
     *           following signature: (const std::set<Entity>&, Model&) -> void
     * 
     * \return A handle to the newly created System.
     */
    template <typename ... Us>
    auto createSystem(System_::ProcessF f)
    {
        auto tmpSys = std::make_unique<System_>(computeSignature_<Us ...>(), f);
        SystemHandle hSys (tmpSys.get());
        auto& sys = systems_[hSys] = std::move(tmpSys);

        auto sSys = sys->signature();
        for (auto e : spawnedEntities_)
        {
            auto s = computeSignature_(e);
            if ((s & sSys) == sSys)
            { sys->insert(e); }
        }

        return hSys;
    }

    /**
     * \brief Removes a System.
     * 
     * \param hSys A handle to the System to be removed.
     */
    void removeSystem(SystemHandle hSys)
    { systems_.erase(hSys); }

    /**
     * \brief Processes the entites. Calls "process(*this)" to
     *        every created Systems.
     */
    void process()
    {
        for (auto& [_, sys] : systems_)
        { sys->process(*this); }
    }

private:
    std::map<SystemHandle, SystemPtr_> systems_;

    void insertInSystems_(Entity e, Signature_ s)
    {
        for (auto& [_, sys] : systems_)
        {
            auto sSys = sys->signature();
            if ((s & sSys) == sSys)
            { sys->insert(e); }
        }
    }

    void removeFromSystems_(Entity e, Signature_ s)
    {
        for (auto& [_, sys] : systems_)
        {
            auto sSys = sys->signature();
            if ((s & sSys) == sSys)
            { sys->remove(e); }
        }
    }
};

}