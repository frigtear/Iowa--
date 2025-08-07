#include "environment.h"

Environment::Environment() : parent_environment(nullptr) {};

Environment::Environment(Environment* env) : parent_environment(env) {};

Environment::dynamic_type Environment::get_variable_value(const std::string& name) const {
    if (variables.contains(name)) {
        return variables.at(name); 
    } else if (parent_environment != nullptr && parent_environment->has_reference(name)){
        return parent_environment->get_variable_value(name);
    }
    else{
        //throw std::runtime_error(std::string{"Error: variable '"} + name + "' does not exist");
    }
}

void Environment::add_variable(std::string name, dynamic_type variable){
    variables.emplace(std::move(name), std::move(variable));
}

void Environment::remove_variable(const std::string& name){
    variables.erase(name);
}

bool Environment::has_reference(const std::string& name) const{
    
    return variables.contains(name);
}

