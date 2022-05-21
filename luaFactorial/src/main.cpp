#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>

#include <fmt/core.h>
#include <lua.hpp>

int main(const int argc, char* argv[]) {
    int32_t n{};

    if (argc == 1) {
        std::cerr << "No arguments were passed." << std::endl;
        return 1;
    }
    if(argc > 2) {
        std::cerr << "Too many arguments were passed." << std::endl;
        return 1;
    }

    try {
        n = std::stoi(argv[1]);
    }
    catch ([[maybe_unused]] std::invalid_argument const& ex) {
        std::cerr << fmt::format("Invalid number: {}.\n", argv[1]);
    }
    catch ([[maybe_unused]] std::out_of_range const& ex) {
        std::cerr << fmt::format("Number out of range: {}.\n", argv[1]);
    }


    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, "script.lua") == LUA_OK) {
        lua_pop(L, lua_gettop(L));
    }
    else {
        puts(lua_tostring(L, lua_gettop(L)));
        lua_pop(L, lua_gettop(L));
        return 1;
    }

    lua_getglobal(L, "binaryTree");
    lua_pushinteger(L, n);

    const auto start = std::chrono::high_resolution_clock::now();
    if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
        if (lua_isinteger(L, -1)) {
            const auto result = lua_tonumber(L, -1);

            lua_pop(L, 1);
            std::cerr << fmt::format("Result: {}\n", result);
        }
        lua_pop(L, lua_gettop(L));
    }
    else {
        puts(lua_tostring(L, lua_gettop(L)));
        lua_pop(L, lua_gettop(L));
        return 1;
    }

    const auto stop = std::chrono::high_resolution_clock::now();

    std::cout << fmt::format("Runtime meassure: {}ms\n", duration_cast<std::chrono::microseconds>(stop - start).count());

    lua_close(L);

    return 0;
}
