#include "Token.h"

std::ostream& operator<<(std::ostream& os, const Token& token) {
    return (os << token.kind());
}