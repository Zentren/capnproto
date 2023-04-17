#pragma once

#include "common.h"
#include "map.h"
#include "memory.h"

KJ_BEGIN_HEADER

namespace kj {

template <typename T>
class Observer {
public:
  virtual void update(const T& value) = 0;
  virtual ~Observer() noexcept(false) {}
};

template <>
class Observer<void> {
public:
  virtual void update() = 0;
  virtual ~Observer() noexcept(false) {}
};

template <typename T = void>
class Subject {
public:
  template <typename U = T, typename = EnableIf<isSameType<U, void>()>>
  void notifyObservers() {
    for (auto observer: observers) {
      observer->update();
    }
  }

  template <typename U = T, typename = EnableIf<!isSameType<U, void>()>>
  void notifyObservers(const T& event) {
    for (auto observer: observers) {
      observer->update(event);
    }
  }

  void addObserver(Observer<T>& observer) {
    observers.insert(&observer);
  }

  void removeObserver(Observer<T>& observer) {
    auto ptr = &observer;
    observers.erase(ptr);
  }

  virtual ~Subject() noexcept(false) {}

private:
  HashSet<Observer<T>*> observers;
};

using VoidObserver = Observer<void>;
using VoidSubject = Subject<void>;

template <class T>
class ScopedObserver : public Observer<T> {
public:
  explicit ScopedObserver(Own<Subject<T>>&& subject) : subject(mv(subject)) {
    subject->addObserver(*this);
  }

  ScopedObserver(ScopedObserver&& other) noexcept(false) : subject(mv(other.subject)) {
    subject->removeObserver(other);
    subject->addObserver(*this);
  }

  ~ScopedObserver() noexcept(false) {
    subject->removeObserver(*this);
  }

protected:
  Subject<T>& getSubject() {
    return *subject;
  }

private:
  Own<Subject<T>> subject;
};

}

KJ_END_HEADER
