#Import("env")
import shutil
import os


original = r'include\User_Setup.h'
target = r'.pio\libdeps\wemos_d1_mini32\TFT_eSPI\User_Setup.h'

os.remove(target)
shutil.copyfile(original, target)
