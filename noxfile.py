import nox


@nox.session()
def test(session):
    session.run("uv", "sync", external=True)
    session.run("pytest", "-m", "mock")
