-- script.lua

function myFactorial(n)
    if n < 0 then
        error("Wrong argument")
    end
    if n > 1 then
        return myFactorial(n -1) * n
    else
        return 1
    end
end
