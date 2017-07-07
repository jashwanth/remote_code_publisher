#ifndef CPP11_BLOCKINGQUEUE_H
#define CPP11_BLOCKINGQUEUE_H
///////////////////////////////////////////////////////////////
// Cpp11-BlockingQueue.h - Thread-safe Blocking Queue        //
// ver 1.4                                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015 //
///////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package contains one thread-safe class: BlockingQueue<T>.
 * Its purpose is to support sending messages between threads.
 * It is implemented using C++11 threading constructs including 
 * std::condition_variable and std::mutex.  The underlying storage
 * is provided by the non-thread-safe std::queue<T>.
 *
 * Required Files:
 * ---------------
 * Cpp11-BlockingQueue.h
 *
 * Build Process:
 * --------------
 * devenv Cpp11-BlockingQueue.sln /rebuild debug
 *
 * Maintenance History:
 * --------------------
 * ver 1.4 : 29 Jul 2016
 * - wrapped with namespace Async
 * ver 1.3 : 04 Mar 2016
 * - changed behavior of front() to throw exception
 *   on empty queue.
 * - added comment about std::unique_lock in deQ()
 * ver 1.2 : 27 Feb 2016
 * - added front();
 * - added move ctor and move assignment
 * - deleted copy ctor and copy assignment
 * ver 1.1 : 26 Jan 2015
 * - added copy constructor and assignment operator
 * ver 1.0 : 03 Mar 2014
 * - first release
 *
 */

#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <iostream>
#include <sstream>

namespace Async
{
  template <typename T>
  class BlockingQueue {
  public:
    BlockingQueue() {}
    BlockingQueue(BlockingQueue<T>&& bq);
    BlockingQueue<T>& operator=(BlockingQueue<T>&& bq);
    BlockingQueue(const BlockingQueue<T>&) = delete;
    BlockingQueue<T>& operator=(const BlockingQueue<T>&) = delete;
    T deQ();
    void enQ(const T& t);
    T& front();
    void clear();
    size_t size();
  private:
    std::queue<T> q_;
    std::mutex mtx_;
    std::condition_variable cv_;
  };
  //----< move constructor >---------------------------------------------

  template<typename T>
  BlockingQueue<T>::BlockingQueue(BlockingQueue<T>&& bq) // need to lock so can't initialize
  {
    std::lock_guard<std::mutex> l(mtx_);
    q_ = bq.q_;
    while (bq.q_.size() > 0)  // clear bq
      bq.q_.pop();
    /* can't copy  or move mutex or condition variable, so use default members */
  }
  //----< move assignment >----------------------------------------------

  template<typename T>
  BlockingQueue<T>& BlockingQueue<T>::operator=(BlockingQueue<T>&& bq)
  {
    if (this == &bq) return *this;
    std::lock_guard<std::mutex> l(mtx_);
    q_ = bq.q_;
    while (bq.q_.size() > 0)  // clear bq
      bq.q_.pop();
    /* can't move assign mutex or condition variable so use target's */
    return *this;
  }
  //----< remove element from front of queue >---------------------------

  template<typename T>
  T BlockingQueue<T>::deQ()
  {
    std::unique_lock<std::mutex> l(mtx_);
    /*
       This lock type is required for use with condition variables.
       The operating system needs to lock and unlock the mutex:
       - when wait is called, below, the OS suspends waiting thread
         and releases lock.
       - when notify is called in enQ() the OS relocks the mutex,
         resumes the waiting thread and sets the condition variable to
         signaled state.
       std::lock_quard does not have public lock and unlock functions.
     */
    if (q_.size() > 0)
    {
      T temp = q_.front();
      q_.pop();
      return temp;
    }
    // may have spurious returns so loop on !condition

    while (q_.size() == 0)
      cv_.wait(l, [this]() { return q_.size() > 0; });
    T temp = q_.front();
    q_.pop();
    return temp;
  }
  //----< push element onto back of queue >------------------------------

  template<typename T>
  void BlockingQueue<T>::enQ(const T& t)
  {
    {
      std::unique_lock<std::mutex> l(mtx_);
      q_.push(t);
    }
    cv_.notify_one();
  }
  //----< peek at next item to be popped >-------------------------------

  template <typename T>
  T& BlockingQueue<T>::front()
  {
    std::lock_guard<std::mutex> l(mtx_);
    if (q_.size() > 0)
      return q_.front();
    throw std::exception("attempt to deQue empty queue");
  }
  //----< remove all elements from queue >-------------------------------

  template <typename T>
  void BlockingQueue<T>::clear()
  {
    std::lock_guard<std::mutex> l(mtx_);
    while (q_.size() > 0)
      q_.pop();
  }
  //----< return number of elements in queue >---------------------------

  template<typename T>
  size_t BlockingQueue<T>::size()
  {
    std::lock_guard<std::mutex> l(mtx_);
    return q_.size();
  }
}
#endif
