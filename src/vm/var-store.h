#ifndef VAR_STORE_H_
#define VAR_STORE_H_

#include "object/object.h"
#include "common/generic-stack.h"
#include <map>
#include <string>
#include <vector>

namespace grok {
namespace vm {

/// Value ::= Value stored in map
struct Value {
    std::shared_ptr<grok::obj::Object> O;
    std::shared_ptr<std::string> S;

    Value(std::shared_ptr<grok::obj::Object> O,
        std::string S = "")
        : O{ O }, S{ std::make_shared<std::string>(S) }
    { }

    Value() = default;
};

template <typename T>
static decltype(auto) GetObjectPointer(Value V)
{
    auto obj = V.O;
    auto pointer = obj->as<T>();
    return pointer;
}

/// MappedValues ::= value mapped to string
using MappedValues = std::map<std::string, Value>;
using MappedValuesHandle = std::shared_ptr<MappedValues>;
using VStoreInternalStack = GenericStack<MappedValuesHandle>;

/// VStore ::= class for storing the variables and functions
/// where each of these variables can be accessed using there names
class VStore {
public:
    VStore();

    /// GetValue ::= returns the Value from the store
    Value GetValue(const std::string &N);

    /// StoreValue ::= stores the value from the string
    void StoreValue(const std::string &N, Value V);

    /// CreateScope ::= start a new scope
    void CreateScope();

    /// RemoveScope ::= remove the scope and associated variables
    void RemoveScope();

    Value TryForOtherScopes(const std::string &N);
private:
    MappedValuesHandle MV;
    VStoreInternalStack VS;
};

} // vm
} // grok

#endif
