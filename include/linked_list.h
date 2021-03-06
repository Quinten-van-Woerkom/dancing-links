//===-- list_view.h - Non-owning linked list --------------------*- C++ -*-===//
//
// Constraint-propagating sudoku solver.
// Copyright(C) 2019 Quinten van Woerkom
//
// This program is free software; you can redistribute it and / or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110 - 1301 USA.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of a non-owning linked list.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <vector>

#include "utility.h"

namespace dlx {
//===-- list iterators --------------------------------------------------=====//
/// List iterators
namespace {
template <typename T> class iterator {
public:
  using value_type = T;
  using difference_type = std::uintptr_t;
  using reference = T &;
  using pointer = T *;
  using iterator_category = std::input_iterator_tag;

  constexpr iterator(T &node) : current{&node} {};

  constexpr auto operator++() noexcept -> iterator & {
    current = &(current->next());
    return *this;
  };

  constexpr auto operator--() noexcept -> iterator & {
    current = &(current->previous());
    return *this;
  };

  constexpr bool operator!=(const iterator &other) const noexcept {
    return other.current != current;
  };

  constexpr bool operator==(const iterator &other) const noexcept {
    return other.current == current;
  };

  // constexpr auto operator*() const noexcept -> const T & { return *current;
  // };
  constexpr auto operator*() noexcept -> T & { return *current; };

private:
  T *current;
};

template <typename T> class const_iterator {
public:
  using value_type = T;
  using difference_type = std::uintptr_t;
  using reference = T &;
  using pointer = T *;
  using iterator_category = std::input_iterator_tag;

  constexpr const_iterator(const T &node) : current{&node} {};

  constexpr auto operator++() noexcept -> const_iterator & {
    current = &current->next();
    return *this;
  };

  constexpr auto operator--() noexcept -> const_iterator & {
    current = &current->previous();
    return *this;
  };

  constexpr bool operator!=(const const_iterator &other) const noexcept {
    return other.current != current;
  };

  constexpr bool operator==(const const_iterator &other) const noexcept {
    return other.current == current;
  };

  constexpr auto operator*() const noexcept -> const T & { return *current; };

private:
  const T *current;
};
} // namespace

//===-- linked list -----------------------------------------------------=====//
/// Linked list that allows reversible removal and insertion of its nodes.
template <typename T> class linked_list {
public:
  using iterator = iterator<T>;
  using const_iterator = const_iterator<T>;

  linked_list() : head{&root()}, tail{&root()} {}

  linked_list(std::size_t size) : nodes{size} {
    for (auto [previous, current] : stx::pairwise(nodes)) {
      previous.link_next(current);
    }
    link_head(nodes.front());
    link_tail(nodes.back());
  }

  /// Iterators into the linked list.
  /// @{
  constexpr auto begin() const noexcept { return const_iterator{*head}; };
  constexpr auto begin() noexcept { return iterator{*head}; };
  constexpr auto end() const noexcept { return const_iterator{root()}; };
  constexpr auto end() noexcept { return iterator{root()}; };
  /// @}

  /// A linked list is empty if its root node has itself as neighbours.
  constexpr auto empty() const noexcept -> bool {
    return head == &root();
  };

  /// Size can be determined by full traversal of the linked list.
  constexpr auto size() const noexcept -> std::size_t {
    return std::distance(begin(), end());
  }

  /// Adds an element to the back of the linked list.
  constexpr void push_back(const T& other) {
    nodes.push_back(other);
    tail->link_next(nodes.back());
    link_tail(nodes.back());
  };

  /// Adds an element to the back of the linked list.
  template<typename... Args>
  constexpr void emplace_back(Args&&... args) {
    nodes.emplace_back(args...);
    tail->link_next(nodes.back());
    link_tail(nodes.back());
  };

  /// Indexing directly into the vector is possible.
  /// @{
  constexpr auto operator[](std::size_t index) const -> const T & {
    return nodes[index];
  }
  constexpr auto operator[](std::size_t index) -> T & { return nodes[index]; }
  /// @}

private:
  T *tail, *head;
  std::vector<T> nodes;

  /// Returns the this pointer interpreted as a pointer to a node.
  /// @{
  auto root() -> T & { return *reinterpret_cast<T *>(this); }
  auto root() const -> const T & { return *reinterpret_cast<const T *>(this); }
  /// @}

  void link_tail(T &node) { root().link_previous(node); }
  void link_head(T &node) { root().link_next(node); }
};

//===-- list view -------------------------------------------------------=====//
/// Non-owning linked list allowing reversible removal and insertion.
template <typename T> class list_view {
public:
  using iterator = iterator<T>;
  using const_iterator = const_iterator<T>;

  constexpr list_view() { root().link_next(root()); };

  template <typename Iterable> list_view(Iterable &nodes) {
    for (auto [previous, current] : stx::pairwise(nodes)) {
      previous.link_next(current);
    }
    link_head(nodes.front());
    link_tail(nodes.back());
  }

  /// Iterators into the linked list.
  /// @{
  constexpr auto begin() const noexcept { return const_iterator{*head}; };
  constexpr auto begin() noexcept { return iterator{*head}; };
  constexpr auto end() const noexcept { return const_iterator{root()}; };
  constexpr auto end() noexcept { return iterator{root()}; };
  /// @}

  /// A linked list is empty if its root node is its own neighbour.
  constexpr auto empty() const noexcept -> bool {
    return head == &root();
  };

  /// Size can be determined by full traversal of the linked list.
  constexpr auto size() const noexcept -> std::size_t {
    return std::distance(begin(), end());
  }

  /// Adds an element to the back of the linked list.
  constexpr void push_back(T &other) {
    head->link_next(other);
    link_tail(other);
  };

  /// Adds a container of elements to the back of the linked list.
  template <typename Iterable> void push_back(Iterable &nodes) {
    for (auto &node : stx::pairwise(nodes)) {
      previous.link_next(node);
    }
    link_head(nodes.front());
    link_tail(nodes.back());
  }

private:
  /// Returns the this pointer interpreted as a pointer to a node.
  /// @{
  auto root() -> T & { return *reinterpret_cast<T *>(this); }
  auto root() const -> const T & { return *reinterpret_cast<const T *>(this); }
  /// @}

  void link_tail(T &node) { root().link_previous(node); }
  void link_head(T &node) { root().link_next(node); }

  T *tail, *head;
};
} // namespace dlx