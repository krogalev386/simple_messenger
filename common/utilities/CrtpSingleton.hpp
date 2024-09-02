#pragma once

template <class T>
class CrtpSingleton {
   protected:
    CrtpSingleton()  = default;
    ~CrtpSingleton() = default;

   private:
    CrtpSingleton(const CrtpSingleton&) = delete;
    CrtpSingleton(CrtpSingleton&&)      = delete;

   public:
    static void init() { getInstance(); };
    static T&   getInstance() {
          static T instance;
          return instance;
    }
};
