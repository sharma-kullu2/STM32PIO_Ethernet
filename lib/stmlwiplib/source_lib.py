import os
import shutil
Import("env")

def copy_folder(src_folder, dest_folder):
    try:
        # Check if the source folder exists
        if os.path.exists(src_folder) and os.path.isdir(src_folder):
            # Create the destination folder if it doesn't exist
            if not os.path.exists(dest_folder):
                os.makedirs(dest_folder)

            # List all items in the source folder
            items = os.listdir(src_folder)

            for item in items:
                src_item = os.path.join(src_folder, item)
                dest_item = os.path.join(dest_folder, item)

                if os.path.isdir(src_item):
                    # If it's a directory, recursively copy it
                    copy_folder(src_item, dest_item)
                else:
                    # If it's a file, copy it to the destination folder
                    shutil.copy2(src_item, dest_item)
        else:
            print("Error: Source folder does not exist or is not a directory:", src_folder)
    except Exception as e:
        print("Error copying folder:", e)

def list_folders(path):
    try:
        # List all items in the path
        items = os.listdir(path)

        # Filter and return only the folders
        folders = [item for item in items if os.path.isdir(os.path.join(path, item))]
        return folders
    except Exception as e:
        print("Error listing folders:", e)
        return []

def is_valid_path(path):
    return os.path.isdir(path)

def list_contains_all_values(lst, values_to_check):
    return all(value in lst for value in values_to_check)

lwip_path = env.GetProjectOption("custom_lwip_path")

if lwip_path:
    if is_valid_path(lwip_path):
        folders=list_folders(lwip_path)
        print('DIR :',folders)
        if list_contains_all_values(folders,['src','system']):
            src_path = os.path.join(lwip_path, 'src')
            if is_valid_path(src_path):
                src_folders=list_folders(src_path)
                print('DIR > SRC :',src_folders)
                if list_contains_all_values(src_folders,['api','core','netif','include']):
                    copy_folder(os.path.join(src_path, 'api'),'src/api')
                    copy_folder(os.path.join(src_path, 'core'),'src/core')
                    copy_folder(os.path.join(src_path, 'netif'),'src/netif')
                    copy_folder(os.path.join(src_path, 'include'),'include')
                    sys_path=os.path.join(lwip_path, 'system')
                    if is_valid_path(sys_path):
                        sys_folders=list_folders(sys_path)
                        print('DIR > SYS :',folders)
                        if list_contains_all_values(sys_folders,['OS','arch']):
                            copy_folder(os.path.join(sys_path, 'OS'),'src/OS')
                            copy_folder(os.path.join(sys_path, 'arch'),'include/arch')
                        else:
                            print("Error: SYS Folder does not contain all files required")
                    else:
                        print("Error: SYS Folder Path Incorrect")
                else:
                    print("Error: SRC Folder does not contain all files required")
            else:
                print("Error: SRC Folder Path Incorrect")
        else:
            print("Error: Top Folder don't have all files required")                 
    else:
        print("Error: lwip_path is not a valid directory:", lwip_path)
else:
    print("Error: lwip_path is not defined in platformio.ini")
