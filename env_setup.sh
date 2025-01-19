poetry config virtualenvs.in-project true
poetry config virtualenvs.path "$(pwd)"/.venv
poetry config virtualenvs.create true
poetry env use python3.12
poetry install --no-interaction --no-root

# Add packages with Pip that don't build correctly with Poetry
poetry run pip install --upgrade pip
