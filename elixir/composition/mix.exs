defmodule Composition.MixProject do
  use Mix.Project

  def project do
    [
      app: :composition,
      version: "0.1.0",
      elixir: "~> 1.6",
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:stream_data, "~> 0.4.2", only: :test},
      {:ex_doc, "~> 0.18.3"}
    ]
  end
end
