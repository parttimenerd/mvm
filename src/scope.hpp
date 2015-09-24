#pragma once

#include "utils.hpp"
#include "heapobject.hpp"

/**
 * @brief The Scope class
 */
class Scope : public HeapObject {

    Optional<SmartReference<Scope>> _parent;
    std::unordered_map<std::string, rref> variables;

public:

    Scope(Env *env, SmartReference<Scope> parent_scope);

    Scope(Env *env);

    SmartReference<Scope> root();

    bool isRoot();

    SmartReference<Scope> parent();

    /**
     * @brief Set the variables value.
     * @param varname variable name
     * @param obj new value
     */
    void set(std::string varname, bref obj);

    /**
     * @brief Set the variables value.
     * @param varname variable name
     * @param obj new value
     */
    void set(std::string varname, rref obj);

    /**
     * @brief Initializes the variable in this scope.
     *
     * @param varname name of the variable
     */
    void initVar(std::string varname);

    /**
     * @brief Initializes and set the variable
     * @param varname variable name
     * @param obj new value
     */
    void setHere(std::string varname, rref obj);

    /**
     * @brief Get the variables value.
     * @param varname variable name
     * @return value
     */
    rref get(std::string varname);

    /**
     * @brief Has this scope a variable with this name?
     * @param varname variable name
     * @param recursive visit parent scopes
     * @return
     */
    bool has(std::string varname, bool recursive = true);

    /**
     * @brief Create a child scope.
     * @return child scope
     */
    SmartReference<Scope> createChild();

    std::string str();

    std::string str_large();
};
