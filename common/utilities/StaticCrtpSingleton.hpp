#pragma once

template <class T>
class StaticCrtpSingleton {
   protected:
    StaticCrtpSingleton()  = default;
    ~StaticCrtpSingleton() = default;

   private:
    StaticCrtpSingleton(const StaticCrtpSingleton&) = delete;
    StaticCrtpSingleton(StaticCrtpSingleton&&)      = delete;

   public:
    static void init() { getInstance(); };
    static T&   getInstance() {
          static T instance;
          return instance;
    }
};
