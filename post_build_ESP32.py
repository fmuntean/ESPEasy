
print ("Adding Post Build: copy firmware to distribution folder")


from shutil import copyfile


import sys
print("Python version:",sys.version_info.major,".",sys.version_info.minor)


def copy_firmware(source, target, env):
    #print("Source: "+target[0].name)
    #print(env.Dump())
    print(".pio/build/"+target[0].name)
    bin = env['PROJECT_DIR']+"/dist/"+target[0].name
    copyfile(".pio/build/"+env['PIOENV']+"/"+target[0].name, bin)

    print ("Done!")


Import("env", "projenv")

# access to global build environment
#print(env)

# access to project build environment (is used source files in "src" folder)
#print(projenv)

#
# Dump build environment (for debug purpose)
# print(env.Dump())
#


# copy firmware to distribution folder
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin",copy_firmware)


