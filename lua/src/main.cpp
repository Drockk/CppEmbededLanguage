#include <iostream>

#include <lua.hpp>

int main() {
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
    lua_pushinteger(L, 3);
    lua_pushinteger(L, 4);

    if (lua_pcall(L, 2, 1, 0) == LUA_OK) {
        if (lua_isinteger(L, -1)) {
            const int result = lua_tointeger(L, -1);

            lua_pop(L, 1);
            printf("Result: %d\n", result);
        }

        lua_pop(L, lua_gettop(L));
    }
    else {
        puts(lua_tostring(L, lua_gettop(L)));
        lua_pop(L, lua_gettop(L));
    }

    lua_close(L);

    system("PAUSE");

    return 0;
}
