import nox
import os

os.environ.update({"PDM_IGNORE_SAVED_PYTHON": "1"})


@nox.session(python=["3.8", "3.9", "3.10", "3.11", "3.12"])
def test(session):
    session.run("pdm", "install", external=True)
    session.run("pytest")
