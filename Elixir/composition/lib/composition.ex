defmodule Composition do
  @moduledoc """

  Function composition operators.

  """

  @doc """
  Function composition.

      iex> f = fn x -> x*2 end
      iex> g = fn x -> x-5 end
      iex> f_of_g = Composition.compose(f, g)
      iex> f_of_g.(7)
      4
      iex> g_of_f = Composition.compose(g, f)
      iex> g_of_f.(7)
      9

  """
  def compose(f, g) when is_function(g) do
    fn x -> f.(g.(x)) end
  end

  @doc """
  Infix composition operator.

      iex> import Composition
      iex> f = fn x -> x*2 end
      iex> g = fn x -> x-5 end
      iex> f_of_g = f <|> g
      iex> f_of_g.(7)
      4
      iex> g_of_f = g <|> f
      iex> g_of_f.(7)
      9

  """
  def f <|> g, do: compose(f, g)
end
