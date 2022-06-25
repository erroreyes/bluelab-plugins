import zipfile, os, fileinput, string, sys

scriptpath = os.path.dirname(os.path.realpath(__file__))
projectpath = os.path.abspath(os.path.join(scriptpath, os.pardir))

MajorStr = ""
MinorStr = "" 
BugfixStr = ""
BUNDLE_NAME = ""

def  main():
  if len(sys.argv) != 2:
    print("Usage: make_zip.py demo(0 or 1)")
    sys.exit(1)
  else:
    demo=int(sys.argv[1])

  # extract values from config.h
  for line in fileinput.input(projectpath + "/config.h",inplace=0):
    if "#define PLUG_VERSION_HEX " in line:
      #PLUG_VER_STR = string.lstrip(line, "#define PLUG_VERSION_HEX ")
      PLUG_VER_STR = line.lstrip("#define PLUG_VERSION_HEX ")
      PLUG_VER = int(PLUG_VER_STR, 16)
      MAJOR = PLUG_VER & 0xFFFF0000
      MAJORSTR = str(MAJOR >> 16)
      MINOR = PLUG_VER & 0x0000FF00
      MINORSTR = str(MINOR >> 8)
      BUGFIXSTR = str(PLUG_VER & 0x000000FF)
    
    if "#define BUNDLE_NAME " in line:
      #BUNDLE_NAME = string.lstrip(line, "#define BUNDLE_NAME ")
      BUNDLE_NAME = line.lstrip("#define BUNDLE_NAME ")

  FULLVERSIONSTR = MAJORSTR + "." + MINORSTR + "." + BUGFIXSTR
  
  installer = "\installer\BL-ReverbDepth-v" + FULLVERSIONSTR + "-Installer.exe"
   
  if demo:
    installer = "\installer\BL-ReverbDepth-v" + FULLVERSIONSTR + "[DEMO]" + "-Installer.exe"
   
  FILES_TO_ZIP = [
    projectpath + installer,
    #bluelab
    #projectpath + "\installer\changelog.txt",
    #projectpath + "\installer\known-issues.txt",
    projectpath + "\manual\BL-ReverbDepth_manual-EN.pdf" 
  ]

  ZIPNAME = "BL-ReverbDepth-v" + FULLVERSIONSTR + "-win.zip"
  
  if demo:
    ZIPNAME = "BL-ReverbDepth-v" + FULLVERSIONSTR + "[DEMO]-win.zip"
  
  zf = zipfile.ZipFile(projectpath + "\installer\/" + ZIPNAME, mode="w")

  for f in FILES_TO_ZIP:
    print("adding " + f)
    zf.write(f, os.path.basename(f));

  zf.close()
  print("wrote ")

if __name__ == '__main__':
  main()
