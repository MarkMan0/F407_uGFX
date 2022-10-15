Import("env")

if "test" in env.GetBuildType():
    env.Append(CPPDEFINES=["TESTING",])