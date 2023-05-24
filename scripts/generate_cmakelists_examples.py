import glob
import os

PATHS = ['examples/']
LIB_NAME = 'highmap'

STUB = '''add_executable({} {}.cpp)
target_link_libraries({} {})
'''

if __name__ == '__main__':

    for p in PATHS:
        exdirs = glob.glob(p + '/*')

        for d in exdirs:
            if os.path.isdir(d):
                print(d)

                # generate CMakeLists.txt file
                example_name = os.path.basename(d)
                fname_cmake =  os.path.join(d, 'CMakeLists.txt')
                
                with open(fname_cmake, 'w') as f:
                    f.write(
                        STUB.format(
                            example_name,
                            example_name,
                            example_name,
                            LIB_NAME))
