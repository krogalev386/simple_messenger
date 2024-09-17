#pragma once

template <class T>
class DynamicCrtpSingleton {
   protected:
    DynamicCrtpSingleton()  = default;
    ~DynamicCrtpSingleton() = default;

   private:
    DynamicCrtpSingleton(const DynamicCrtpSingleton&) = delete;
    DynamicCrtpSingleton(DynamicCrtpSingleton&&)      = delete;

   public:
    static void init() { instance_ptr = new T(); };
    static void destroy() { delete instance_ptr; }
    static T&   getInstance() { return *instance_ptr; };

   private:
    inline static T* instance_ptr = nullptr;
};
