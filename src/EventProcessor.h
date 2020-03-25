#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

using std::condition_variable;
using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::thread;


class EventProcessor {
 public:
  enum class EventType { Login, ChatMsg, Logout, SignUp };

  /*
   * toString:  EventTypeId|member1|member2
   *
   *
   */
  struct Event {
    Event(const EventType& t) : eventType(t){};
    const EventType eventType;
    virtual string getEventInfo() = 0;
    virtual string toMsg() = 0;
  };

  struct LoginEvent final : Event {
    LoginEvent(const string& u, const string& p)
        : Event{EventType::Login}, username(u), passwd(p){};

    static shared_ptr<LoginEvent> create(const string& data) {
      shared_ptr<LoginEvent> ptr{};
      bool parseFailed = false;

      const int len = 256;
      char buff[len]{};
      if (data[0] == '0') {
        try {
          stringstream ss{data};
          ss.getline(buff, len, '|');
          // skip eventType id;
          ss.getline(buff, len, '|');
          string u{buff};
          ss.getline(buff, len, '|');
          string p{buff};
          ptr.reset(new LoginEvent(u, p));
        } catch (...) {
          parseFailed = true;
        }
      } else {
        parseFailed = true;
      }

      if (parseFailed) {
        string info{"LoginEvent parse failed for ["};
        info += data;
        info += "]";
        throw info;
      } else {
        return ptr;
      }
    }

    const string username;
    const string passwd;
    string getEventInfo() override {
      string s{"[Login, username="};
      s += username;
      s += "]";
      return s;
    }

    string toMsg() override {
      stringstream ss{};
      ss << static_cast<int>(eventType) << "|";
      ss << username << "|";
      ss << passwd << "|";
      return ss.str();
    }
  };

  struct ChatMsgEvent final : Event {
    ChatMsgEvent(const string& toUser_, const string& fromUser_,
                 const string& words_)
        : Event{EventType::ChatMsg},
          toUser(toUser_),
          fromUser(fromUser_),
          words(words_){};

    static shared_ptr<ChatMsgEvent> create(const string& data) {
      //cout << "for debug: ChatMsgEvent create input=[" << data << "]" << endl;

      shared_ptr<ChatMsgEvent> ptr{};
      bool parseFailed = false;

      const int len = 1024;
      char buff[len]{};
      if (data[0] == '1') {
        try {
          stringstream ss{data};
          ss.getline(buff, len, '|');
          // skip eventType id;
          ss.getline(buff, len, '|');
          string toUser{buff};
          ss.getline(buff, len, '|');
          string fromUser{buff};
          ss.getline(buff, len, '|');
          string words{buff};
          ptr.reset(new ChatMsgEvent(toUser, fromUser, words));
        } catch (...) {
          parseFailed = true;
        }
      } else {
        parseFailed = true;
      }

      if (parseFailed) {
        string info{"ERROR INFO: ChatMsgEvent parse failed for ["};
        info += data;
        info += "]";
        cout << info << endl;
        throw info;
      } else {
        return ptr;
      }
    }

    const string toUser;
    const string fromUser;
    const string words;
    string getEventInfo() override {
      string s{"[ChatMsg, toUser="};
      s += toUser;
      s += ", fromUser=";
      s += fromUser;
      s += ", words=";
      s += words;
      s += "]";
      return s;
    }

    string toMsg() override {
      stringstream ss{};
      ss << static_cast<int>(eventType) << "|";
      ss << toUser << "|";
      ss << fromUser << "|";
      ss << words << "|";
      return ss.str();
    }
  };

  struct LogoutEvent final : Event {
    LogoutEvent() : Event{EventType::Logout} {};

    static shared_ptr<LogoutEvent> create(const string& data) {
      shared_ptr<LogoutEvent> ptr{};
      bool parseFailed = false;

      const int len = 256;
      char buff[len]{};
      if (data[0] == '2') {
        try {
          ptr.reset(new LogoutEvent());
        } catch (...) {
          parseFailed = true;
        }
      } else {
        parseFailed = true;
      }

      if (parseFailed) {
        string info{"LogoutEvent parse failed for ["};
        info += data;
        info += "]";
        throw info;
      } else {
        return ptr;
      }
    }

    string getEventInfo() override {
      string s{"[Logout,"};
      s += "]";
      return s;
    }

    string toMsg() override {
      stringstream ss{};
      ss << static_cast<int>(eventType) << "|";
      return ss.str();
    }
  };

  struct SignUpEvent final : Event {
    SignUpEvent(const string& u, const string& p)
        : Event{EventType::SignUp}, username(u), passwd(p){};

    static shared_ptr<SignUpEvent> create(const string& data) {
      shared_ptr<SignUpEvent> ptr{};
      bool parseFailed = false;

      const int len = 256;
      char buff[len]{};
      if (data[0] == '3') {
        try {
          stringstream ss{data};
          ss.getline(buff, len, '|');
          // skip eventType id;
          ss.getline(buff, len, '|');
          string u{buff};
          ss.getline(buff, len, '|');
          string p{buff};
          ptr.reset(new SignUpEvent(u, p));
        } catch (...) {
          parseFailed = true;
        }
      } else {
        parseFailed = true;
      }

      if (parseFailed) {
        string info{"SignUpEvent parse failed for ["};
        info += data;
        info += "]";
        throw info;
      } else {
        return ptr;
      }
    }

    const string username;
    const string passwd;
    string getEventInfo() override {
      string s{"[SignUp, username="};
      s += username;
      s += "]";
      return s;
    }

    string toMsg() override {
      stringstream ss{};
      ss << static_cast<int>(eventType) << "|";
      ss << username << "|";
      ss << passwd << "|";
      return ss.str();
    }
  };

 private:
  list<shared_ptr<Event>> eventList{};
  condition_variable cv{};
  mutex mx{};
  bool isStarted{false};

  void startHandler() {
    cout << "EventProcessor's handler start." << endl;

    isStarted = true;
    decltype(eventList) targetList{};
    std::unique_lock<std::mutex> lk{mx, std::defer_lock};
    shared_ptr<Event> event{};
    while (true) {
      if (!targetList.empty()) {
        try {
          event = targetList.front();
          targetList.pop_front();
          // cout << "for debug EventProcessor: got event:" <<
          // event->getEventInfo() << endl;
          processEvent(event);
        } catch (const exception& ex) {
          if (event != nullptr) {
            cout << "current event:" << event->getEventInfo() << endl;
            cout << "exception in processEvent:" << ex.what() << endl;
          } else {
            cout << "exception in processEvent:" << ex.what() << endl;
          }
        } catch (...) {
          if (event != nullptr) {
            cout << "current event:" << event->getEventInfo() << endl;
            cout << "unknown exception in processEvent.";
          } else {
            cout << "unknown exception in processEvent.";
          }
        }
      } else {
        lk.lock();
        cv.wait(lk, [this] { return !eventList.empty(); });
        targetList.swap(eventList);
        // cout << "for debug EventProcessor targetList swap." << endl;
        lk.unlock();
      }
    }
    isStarted = false;
  }

 public:
  EventProcessor() {
    //thread t(std::mem_fn(&EventProcessor::startHandler), std::ref(*this));
    thread t(&EventProcessor::startHandler, this);
    t.detach();
  };

  void push(shared_ptr<Event> evn) {
    mx.lock();
    eventList.push_back(evn);
    mx.unlock();
    cv.notify_one();
  };

  void push(const string& evnStr) {
    shared_ptr<EventProcessor::Event> event{};
    switch (evnStr[0]) {
      case '0':
        event = EventProcessor::LoginEvent::create(evnStr);
        break;
      case '1':
        event = EventProcessor::ChatMsgEvent::create(evnStr);
        break;
      case '2':
        event = EventProcessor::LogoutEvent::create(evnStr);
        break;
      case '3':
        event = EventProcessor::SignUpEvent::create(evnStr);
        break;
      default:
        break;
    }

    if (event != nullptr) {
      mx.lock();
      eventList.push_back(event);
      mx.unlock();
      cv.notify_one();
    } else {
      cout << "EventProcessor push parse failed: [" << evnStr << "]" << endl;
    }
  };

 protected:
  virtual void processEvent(shared_ptr<Event> evn) = 0;
};
