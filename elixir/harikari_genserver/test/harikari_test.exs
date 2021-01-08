defmodule HarikariTest do
  use ExUnit.Case
  doctest Harikari

  test "greets the world" do
    assert Harikari.hello() == :world
  end
end
