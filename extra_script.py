import os
import shutil
Import("env","projenv")

# Custom HEX from ELF
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", "-R", ".eeprom",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}.hex"
    ]), "Building $BUILD_DIR/${PROGNAME}.hex")
)

'''
# List of folders to delete
folders_to_delete = ['src', 'include']

# Get the current working directory
#print(projenv.Dump())
print(projenv['PROJECT_DIR'])

current_directory = projenv['PROJECT_DIR']
current_directory=os.path.join(current_directory,'lib\\stmlwiplib')
print(current_directory)
for folder in folders_to_delete:
    folder_path = os.path.join(current_directory, folder)
    
    if os.path.exists(folder_path) and os.path.isdir(folder_path):
        try:
            shutil.rmtree(folder_path)  # Remove an empty directory
            print(f"Deleted folder: {folder}")
        except OSError as e:
            print(f"Error deleting folder {folder}: {e}")
    else:
        print(f"Folder {folder} does not exist in the current directory or is not a directory.")

'''


