/* Keyleds -- Gaming keyboard tool
 * Copyright (C) 2017 Julien Hartmann, juli1.hartmann@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef KEYLEDS_PLUGINS_LUA_TYPES_H_1B6DBCFA
#define KEYLEDS_PLUGINS_LUA_TYPES_H_1B6DBCFA

#include "lua.hpp"
#include <type_traits>
#include <utility>

namespace keyleds { namespace plugin { namespace lua {

/****************************************************************************/

template <typename T> struct metatable {};

void registerType(lua_State *, const char * name, const luaL_reg * metaMethods);
bool isType(lua_State * lua, int index, const char * type);

/// Registers the metatable with lua
template <typename T> void registerType(lua_State * lua)
{
    using meta = metatable<typename std::remove_cv<T>::type>;
    registerType(lua, meta::name, meta::methods);
}

/// Tests whether value at index is of specified type
template <typename T> bool lua_is(lua_State * lua, int index)
{
    using meta = metatable<typename std::remove_cv<T>::type>;
    return isType(lua, index, meta::name);
}

/// Returns a reference to userdata object of a type, checking type beforehand
template <typename T> T & lua_check(lua_State * lua, int index)
{
    using meta = metatable<typename std::remove_cv<T>::type>;
    return *static_cast<T *>(luaL_checkudata(lua, index, meta::name));
}

/// Returns a reference to userdata object of type, without checking
template <typename T> T & lua_to(lua_State * lua, int index)
{
    return *static_cast<T *>(const_cast<void *>(lua_topointer(lua, index)));
}

/// Pushes an object into a userdata, on the stack
template <typename T>
typename std::enable_if<std::is_copy_constructible<T>::value &&
                        !std::is_reference<T>::value>::type
lua_push(lua_State * lua, const T & value)
{
    using meta = metatable<typename std::remove_cv<T>::type>;
    void * ptr = lua_newuserdata(lua, sizeof(T));
    new (ptr) T(value);
    luaL_getmetatable(lua, meta::name);
    lua_setmetatable(lua, -2);
}

template <typename T>
typename std::enable_if<std::is_move_constructible<T>::value &&
                        !std::is_reference<T>::value>::type
lua_push(lua_State * lua, T && value)
{
    using meta = metatable<typename std::remove_cv<T>::type>;
    void * ptr = lua_newuserdata(lua, sizeof(T));
    new (ptr) T(std::move(value));
    luaL_getmetatable(lua, meta::name);
    lua_setmetatable(lua, -2);
}

/// Emplaces an object directly into a userdata, on the stack
template <typename T, typename... Args>
typename std::enable_if<std::is_constructible<T, Args...>::value>::type
lua_emplace(lua_State * lua, Args &&... args)
{
    using meta = metatable<typename std::remove_cv<T>::type>;
    void * ptr = lua_newuserdata(lua, sizeof(T));
    new (ptr) T(std::forward<Args>(args)...);
    luaL_getmetatable(lua, meta::name);
    lua_setmetatable(lua, -2);
}

/****************************************************************************/

} } } // namespace keyleds::plugin::lua

#endif
