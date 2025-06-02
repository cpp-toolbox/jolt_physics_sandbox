# jolt physics sandbox
This project stands as a springboard to mess around with jolt from, it uses opengl3.3 to render the scene

# setup
This project uses conan to install the required libraries, In order to configure it correctly in your `~/.conan2/profiles/default` or any profile you like using add this to the file:

```
[settings]
...

[conf]
...
joltphysics/*:tools.cmake.cmaketoolchain:extra_variables={"DEBUG_RENDERER_IN_DEBUG_AND_RELEASE": "ON", "CPP_RTTI_ENABLED": "ON"}
```

If you've already built jolt, then the above configuration won't come into play immediately instead you must rebuild it like this:
```
conan install . --build="joltphysics/*"
```
