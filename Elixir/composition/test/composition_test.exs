defmodule CompositionTest do
  import Composition

  use ExUnitProperties
  use ExUnit.Case
  doctest Composition

  property "`composition' is always <|>" do
    f = fn x -> x*2 end
    g = fn x -> x-5 end
    check all x <- integer() do
      assert (f <|> g).(x) == compose(f,g).(x)
    end
  end

  property "`composition' is just glue" do
    identity = fn x -> x end
    check all x <- integer() do
      assert (identity <|> identity).(x) == x
    end
  end
end
