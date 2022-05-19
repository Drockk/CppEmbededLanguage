#include <iostream>
#include <string>
#include <format>
#include <stdexcept>
#include <chrono>

#include <lua.hpp>

int main(const int argc, char* argv[]) {
    int32_t n{};

    if (argc == 1) {
        std::cerr << "No arguments were passed." << std::endl;
    }
    else if (argc == 2) {
        try {
            n = std::stoi(argv[1]);
        }
        catch ([[maybe_unused]] std::invalid_argument const& ex) {
            std::cerr << std::format("Invalid number: {}.\n", argv[1]);
        }
        catch ([[maybe_unused]] std::out_of_range const& ex) {
            std::cerr << std::format("Number out of range: {}.\n", argv[1]);
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

    lua_getglobal(L, "myFactorial");
    lua_pushinteger(L, n);

    const auto start = std::chrono::high_resolution_clock::now();
    if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
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

    const auto stop = std::chrono::high_resolution_clock::now();

    std::cout << std::format("Runtime meassure: {}\n", duration_cast<std::chrono::microseconds>(stop - start));

    lua_close(L);

    return 0;
}
