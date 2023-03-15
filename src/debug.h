#ifndef BF_DEBUG_H
#define BF_DEBUG_H

namespace Debug {
    constexpr auto debug =
#ifdef NDEBUG
            false
#else
            true
#endif
            ;

    template<typename T>
    constexpr T if_debug(T ifTrue, T ifFalse) {
        if constexpr (debug) {
            return ifTrue;
        } else {
            return ifFalse;
        }
    }
}
#endif
