#pragma once

#include <array>
#include <vector>
#include <memory>
#include <deque>
#include <limits>

namespace yobtk::ecs {

/**
 * \brief Represents a matrix of access points (only offsets in tables for the moment).
 *        It uses a vector of unique pointers to Blocks where Blocks are fixed size arrays of
 *        access points.
 * 
 * \param N Size of a block.
 * \param M Number of access points per element.
 */
template <std::size_t N, std::size_t M>
class AccessMatrix
{
private:
    using Accessors_ = std::array<std::size_t, M>;
    using Block_     = std::array<Accessors_, N>;

public:
    /**
     * \brief Represents a pointer to the data stored.
     */
    using Index = typename Block_::iterator;

    /**
     * \brief Retrieves the p-th access point at Index i.
     * 
     * \param i The index of interest.
     * \param p The access point number.
     * 
     * \return A reference to the access point's value.
     */
    auto& get(Index i, std::size_t p)
    { return (*i)[p]; }

    /**
     * \brief Check if the Index i has the p-th access point.
     * 
     * \param i The index of interest.
     * \param p The access point number.
     * 
     * \return A boolean answering the check.
     */
    auto has(Index i, std::size_t p)
    { return get(i, p) != maxAccessor_; }

    /**
     * \brief Resets the p-th access point of Index i to the default value.
     * 
     * \param i The index of interest.
     * \param p The access point number.
     */
    void reset(Index i, std::size_t p)
    { get(i, p) = maxAccessor_; }

    /**
     * \brief Creates an Index.
     * 
     * \return The newly created Index.
     */
    Index make()
    {
        if (available_.empty())
        { expand_(); }

        auto i = available_.back();
        available_.pop_back();
        return i;
    }

    /**
     * \brief Releases an Index.
     * 
     * \param i The index to be released.
     */
    void free(Index i)
    {
        *i = defaultAccessors_;
        available_.push_back(i);
    }

private:
    std::vector<std::unique_ptr<Block_>> data_;
    std::deque<Index> available_;

    static constexpr auto maxAccessor_ = std::numeric_limits<std::size_t>::max();
    static constexpr auto defaultAccessors_ = [](){
        Accessors_ d;
        d.fill(maxAccessor_);
        return d;
    }();

    void expand_()
    {
        auto& last = data_.emplace_back(std::make_unique<Block_>());
        last->fill(defaultAccessors_);
        for(auto it = last->begin(); it < last->end(); it++)
        { available_.push_front(it); }
    }
};

}