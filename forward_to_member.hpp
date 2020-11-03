#ifndef GATE_FORWARD_TO_MEMBER_HPP
  #define GATE_FORWARD_TO_MEMBER_HPP

  #include <utility> //forward

  #define FORWARD_TO_MEMBER(obj, member_func, cv_qualifier) \
    template<typename... Args> \
    inline auto member_func(Args&&... args) cv_qualifier { \
      return (obj).member_func(std::forward<Args>(args)...); \
    }

#endif
