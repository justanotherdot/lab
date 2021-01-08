defmodule Harikari do
  @moduledoc """
  Start a server that allows messages to kill it internally.

  Technically, _any_ process could send this message to the server.
  """
  use GenServer

  def init(args) do
    {:ok, args}
  end

  def handle_info(:implode, state) do
    {:stop, :normal, state}
  end

  def terminate(_, state) do
    IO.puts "Dying, here's my corpose: #{inspect state}"
  end
end
