if ! grep -o "potato" ~/.zshrc; then
  echo "Negation works by prepending an exclamation mark"
else
  echo "Negation does not work by prepending an exclamation mark"
fi
