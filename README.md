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

Additionally when you use something like conan for dependencies it pre-builds your libraries so that your client code can use it, when you use the above extra variables it means that the library is built with the following definition `JPH_DEBUG_RENDERER`, by default your client code will not have this defined and your program will not run due to jolt reporting mismatched defines, so make sure you add the following into your `CMakeLists.txt`:
```
add_definitions(-DJPH_DEBUG_RENDERER)
```
