import nox
import os

os.environ.update({"PDM_IGNORE_SAVED_PYTHON": "1"})


@nox.session()
def test(session):
    session.run("pdm", "install", "-G:all", external=True)
    session.run("pytest", "-m", "mock")
