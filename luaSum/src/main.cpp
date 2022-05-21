#include <iostream>
#include <string>
#include <stdexcept>

#include <fmt/core.h>
#include <lua.hpp>

int main(const int argc, char* argv[]) {
    int32_t a{}, b{};

    if(argc == 1) {
        std::cerr << "No arguments were passed." << std::endl;
        return 1;
    }
    if(argc > 3) {
        std::cerr << "Too many arguments were passed." << std::endl;
        return 1;
    }

    try {
        a = std::stoi(argv[1]);
        b = std::stoi(argv[2]);
    } catch ([[maybe_unused]] std::invalid_argument const& ex) {
        std::cerr << fmt::format("Invalid number: {} or {}.\n", argv[1], argv[2]);
    } catch ([[maybe_unused]] std::out_of_range const& ex) {
        std::cerr << fmt::format("Number out of range: {} or {}.\n", argv[1], argv[2]);
    }

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, "script.lua") == LUA_OK) {
        lua_pop(L, lua_gettop(L));
    }
    else {
        puts(lua_tostring(L, lua_gettop(L)));
        lua_pop(L, lua_gettop(L));
        return -1;
    }

    lua_getglobal(L, "mySum");
    lua_pushinteger(L, a);
    lua_pushinteger(L, b);

    if (lua_pcall(L, 2, 1, 0) == LUA_OK) {
        if (lua_isinteger(L, -1)) {
            const auto result = lua_tonumber(L, -1);

            lua_pop(L, 1);
            std::cerr << fmt::format("Result: {}\n", static_cast<double>(result));
        }
        lua_pop(L, lua_gettop(L));
    }
    else {
        puts(lua_tostring(L, lua_gettop(L)));
        lua_pop(L, lua_gettop(L));
        return -1;
    }

    lua_close(L);

    return 0;
}
