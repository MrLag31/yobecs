#pragma once

#include <vector>

namespace yobtk::ecs {

/**
 * \brief Represents a Component. Groups data inside a contiguous vector.
 * 
 * \param T The type used.
 * \param E The Entity type, used to handle owners.
 */
template <typename T, typename E>
class Component
{
public:
    /**
     * \brief Inserts entity e to the component with value val.
     * 
     * \param e   The entity.
     * \param val An optional default value.
     * 
     * \return The offset of the data inside the vector.
     */
    auto insert(E e, const T& val = {})
    {
        auto a = data_.size();
        data_.push_back(val);
        owners_.push_back(e);
        return a;
    }

    /**
     * \brief Removes the data at offset a.
     * 
     * \param a The offset.
     * 
     * \return The owner having its data moved to a.
     */
    auto remove(std::size_t a)
    {
        owners_[a] = owners_.back();
        auto e = owners_[a];
        owners_.resize(owners_.size() - 1);
        
        data_[a] = data_.back();
        data_.resize(data_.size() - 1);

        return e;
    }

    /**
     * \brief Accesses the data at offset a.
     * 
     * \param a The offset.
     * 
     * \return A reference to the data.
     */
    auto& access(std::size_t a)
    { return data_[a]; }

private:
    std::vector<T> data_;
    std::vector<E> owners_;
};

}