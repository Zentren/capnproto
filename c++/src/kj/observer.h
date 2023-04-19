#pragma once

#include "common.h"
#include "debug.h"
#include "function.h"
#include "map.h"
#include "memory.h"
#include "refcount.h"

KJ_BEGIN_HEADER

namespace kj {

class Subscription;

namespace _ {  // private
class SubjectBase {
  friend class ::kj::Subscription;
protected:
  virtual void unsubscribe(uint id) = 0;
  virtual ~SubjectBase() noexcept(false) {}
};
}

class Subscription {
  template <typename>
  friend class Subject;

public:
  Subscription() {}
  Subscription(Subscription&&) = default;

  void unsubscribe() {
    KJ_IF_MAYBE(subjectAndId, subjectAndIdMaybe) {
      subjectAndId->subject->unsubscribe(subjectAndId->id);
    }
  }

  bool isSubscribed() const {
    KJ_IF_MAYBE(subjectAndId, subjectAndIdMaybe) {
      return true;
    }
    return false;
  }

  ~Subscription() {
    unsubscribe();
  }

private:
  struct SubjectAndId {
    Own<_::SubjectBase> subject;
    uint id;
  };

  Maybe<SubjectAndId> subjectAndIdMaybe;

  Subscription(Own<_::SubjectBase>&& subject, uint id) : subjectAndIdMaybe({mv(subject), id}) {}
};

template <typename T = void>
class Subject : public _::SubjectBase, public Refcounted {
  friend class Subscription;
public:
  template <typename U = T, typename = EnableIf<isSameType<U, void>()>>
  void notify() {
    deferUnsubscribes = true;
    KJ_DEFER(deferUnsubscribes = false);
    for (auto& observer : observers) {
      observer();
    }
    processDeferredUnsubscribes();
  }

  template <typename U = T, typename = EnableIf<!isSameType<U, void>()>>
  void notify(const U& event) {
    deferUnsubscribes = true;
    KJ_DEFER(deferUnsubscribes = false);
    for (auto& entry : observers) {
      entry.value(event);
    }
    processDeferredUnsubscribes();
  }

  template <typename Func>
  Subscription subscribe(Func&& observer) {
    auto id = nextId++;
    observers.insert(id, Function<void(T)>(mv(observer)));
    return {addRef(), id};
  }

  Own<SubjectBase> addRef() {
    return addRef(*this);
  }

private:
  uint nextId{0};
  HashMap<uint, Function<void(T)>> observers;
  bool deferUnsubscribes{false};
  Vector<uint> deferredUnsubscribes;

  void unsubscribe(uint id) override {
    if (deferUnsubscribes) {
      deferredUnsubscribes.add(id);
    } else {
      observers.erase(id);
    }
  }

  void processDeferredUnsubscribes() {
    for (auto id : deferredUnsubscribes) {
      unsubscribe(id);
    }
    deferredUnsubscribes.clear();
  }
};

}

KJ_END_HEADER
