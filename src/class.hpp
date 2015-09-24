#pragma once

#include "utils.hpp"
#include "baseobject.hpp"
#include "heapobject.hpp"

class Env;
class Scope;

/**
 * @brief A mvm class.
 *
 */
class Class : public BaseObject {

    /**
     * @brief name of the class
     */
    std::string name;

    SmartReference<Scope> parent_scope;

    Optional<SmartReference<Class>> parent;

    /**
     * @brief instance members of objects of this class
     */
    std::map<std::string, bref> instance_members;

public:

    Class(Env *env, SmartReference<Scope> parent_scope, std::string name);

    Class(Env *env, SmartReference<Scope> parent_scope, std::string name, std::map<std::string, bref> members)
        : Class(env, parent_scope, name) {
        instance_members = members;
    }

    Class(Env *env, SmartReference<Scope> parent_scope, std::string name, std::map<std::string, bref> members, SmartReference<Class> parent)
        : Class(env, parent_scope, name, members) { setParent(parent); }

    /**
     * @brief Checks the class
     * @param class_name name of the class
     * @return is this class the passed class or a child of it?
     */
    bool isClass(std::string class_name);

    /**
     * @brief Create an object of this class
     * @return object of this class
     */
    bref create(FunctionArguments &args);

    /**
     * @brief Checks for the member
     * @param member name of the member
     * @return does this class have the passed member?
     */
    bool hasMember(std::string member);

    /**
     * @brief Get the member
     * @param member name of the member
     * @return value of the member or nothing
     */
    bref getMember(std::string member);

    /**
     * @brief Sets the member to a new value
     * Doesn't affect old child classes.
     * @param member name of the member
     * @param value new value of the member
     * @return value
     */
    bref setMember(std::string member, bref value);

    /**
     * @brief Sets the parent of this class
     * @param parent new parent
     * @throws error if this class has already a parent class
     */
    void setParent(SmartReference<Class> parent);

    bool hasParent();

    void setFunctionMembers(std::vector<FunctionTuple> funcs);

    void setClassFunctions(std::vector<FunctionTuple> funcs);

    bool equalsClass(SmartReference<Class> other);

protected:

    void copyInstanceMembers(rref obj);
};
