--Lua Table
Config = {
  title = "My Game Engine",
  fullscreen = false,
  resolution = {
    width = 800,
    height = 600
  },
}

function Factorial(n)
  if n == 1 then return 1 end
  return n * Factorial(n - 1)
end

print("Result of native C++ function Cube(3): "..cube(3))
