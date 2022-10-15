from time import sleep

Import("env")
   


if "test" in env.GetBuildType():
    env.AddPostAction("upload", lambda *_, **__: sleep(1))