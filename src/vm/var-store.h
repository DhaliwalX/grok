#ifndef VAR_STORE_H_
#define VAR_STORE_H_

#include "object/object.h"
#include <map>
#include <string>

namespace grok {
namespace vm {

/// Value ::= Value stored in map
struct Value {
    std::shared_ptr<grok::obj::Object> O;

    Value(std::shared_ptr<grok::obj::Object> O)
        : O(O)  // A man with a Glass
    { }

    Value() = default;
};

/// MappedValues ::= value mapped to string
using MappedValues = std::map<std::string, Value>;

/// VStore ::= class for storing the variables and functions
/// where each of these variables can be accessed using there names
class VStore {
public:
    VStore();

    /// GetValue ::= returns the Value from the store
    Value GetValue(const std::string &N);

    /// StoreValue ::= stores the value from the string
    void StoreValue(const std::string &N, Value V);
private:
    MappedValues MV;
};

} // vm
} // grok

#endif
