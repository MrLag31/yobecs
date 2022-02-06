# yobecs: *YOhann Bosqued's ECS*

## Description

Entity-Component-System (or **ECS** for short) is a method to arrange and modify huge sets of data. It centers the organisation around said data rather than the objects itselves and their hierarchies. This is a design based on Data Oriented Programming (as opposed to the classic Object Oriented Programming) and is commonly used in programs handling a lot of objects at the same time. Its goal is to take avantage of contiguous data and simple operations.

In the ECS design, an Entity is simply a unique object that represents, well, an entity inside the program. A Component, parametrized by a type, regroups the data in a contiguous array. A System is simply a function called on each Entity inside a set of Components.

In the current implementation, some important things can differ from others:
* An Entity is more complex than a simple id and id reuse might happen.
* Components are registered at compile-time, not at runtime.
* To the user, a System is handled like an Entity.
* Entity and System creation and removal are not thread safe.

More information is present in the [Details](#Details) section.

## Install

This library being a header-only library, simply clone this repository and include the file `./yobecs/ecs.hpp` once in your project. Beware that it uses some C++20 features (concepts and `operator<=>`).

## Details

My goal in my implementation of this **ECS** was to reduce the number of indirections between an Entity and its data inside of a Component. The idea is that an element of the set that defines where the data is for each entity uniquely identifies said entity. And thus, an Entity is simply a pointer to this element.

However, one must garantee that this pointer is never invalidated. The only basic standard structure having this guarantee is `std::array` in which each iterator is valid until the destruction of the array. The manager of this set thus simply handles a vector of unique pointers to array called *Blocks* and gives out available spots. Finally, elements of these *Blocks* are a collection of indices indicating where the Entity's data is stored in each Component. This requires to have a constant a number of Components.

## TODOs

If I ever come back to this project and try to update it, these are the features I will try to bring:
* Usage of Models as Component's type.
* Abstract the concept of Systems (it is currently pretty close to being an Entity)
* Garbage collection for AccessMatrix objects.
* Optimisation for small Component's types.
* Apply the Entity design to Component's data.
* Optimise Entity and System creation and deletion. It is currently rather slow.

## Thanks
This **ECS** implementation is ~~slightly~~ based upon [Austin Morlan's implementation](https://code.austinmorlan.com/austin/ecs). Many thanks to him for his detailed explanation of his implementation.