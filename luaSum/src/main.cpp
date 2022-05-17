#include <iostream>
#include <string>
#include <format>
#include <stdexcept>

#include <lua.hpp>

int main(int argc, char* argv[]) {
    int32_t a{}, b{};

    if(argc == 1) {
        std::cerr << "No arguments were passed." << std::endl;
    }
    else if(argc == 3) {
        try {
            a = std::stoi(argv[1]);
            b = std::stoi(argv[2]);
        } catch ([[maybe_unused]] std::invalid_argument const& ex) {
            std::cerr << std::format("Invalid number: {} or {}.\n", argv[1], argv[2]);
        } catch ([[maybe_unused]] std::out_of_range const& ex) {
            std::cerr << std::format("Number out of range: {} or {}.\n", argv[1], argv[2]);
        }
    }
    else {
        std::cerr << "Too many arguments were passed." << std::endl;
    }

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, "script.lua") == LUA_OK) {
        lua_pop(L, lua_gettop(L));
    }
    else {
        puts(lua_tostring(L, lua_gettop(L)));
        lua_pop(L, lua_gettop(L));
    }

    lua_getglobal(L, "mySum");
    lua_pushinteger(L, a);
    lua_pushinteger(L, b);

    if (lua_pcall(L, 2, 1, 0) == LUA_OK) {
        if (lua_isinteger(L, -1)) {
            const int result = lua_tointeger(L, -1);

            lua_pop(L, 1);
            std::cerr << std::format("Result: {}\n", result);
        }
        lua_pop(L, lua_gettop(L));
    }
    else {
        puts(lua_tostring(L, lua_gettop(L)));
        lua_pop(L, lua_gettop(L));
    }

    lua_close(L);

    return 0;
}
