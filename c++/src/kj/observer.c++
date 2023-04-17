#include "observer.h"

namespace kj {

//void Subject::notifyObservers() {
//  for (auto observer: observers) {
//    observer->update();
//  }
//}
//
//void Subject::addObserver(Observer& observer) {
//  observers.insert(&observer);
//}
//
//void Subject::removeObserver(Observer& observer) {
//  auto ptr = &observer;
//  observers.erase(ptr);
//}
//
//ScopedObserver::ScopedObserver(kj::Own<Subject>&& subject) : subject(kj::mv(subject)) {
//  subject->addObserver(*this);
//}
//
//ScopedObserver::ScopedObserver(ScopedObserver&& other) noexcept(false)
//    : subject(kj::mv(other.subject)) {
//  subject->removeObserver(other);
//  subject->addObserver(*this);
//}
//
//ScopedObserver::~ScopedObserver() {
//  subject->removeObserver(*this);
//}
//
//Subject& ScopedObserver::getSubject() {
//  return *subject;
//}

}
