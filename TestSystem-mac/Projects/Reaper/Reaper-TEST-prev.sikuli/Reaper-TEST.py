# NOTE: Reaper set the preferences options to the page where is the buffer size
# NOTE: Reaper Preferences => new shortcut cmd+shift+p
#
# NOTE: RX6 => export screenshot shortcut added shift+alt+cmd+e
# NOTE: RX6 => screenshot res set to 1024x600
# NOTE: RX6 => set tmp dir by hand for opening wave file, and saving screenshot
# NOTE: RX6 => take care to close all tabs at the beginning
import sys.argv
import shutil
import os

#
# parameters
#
plugName = "BL-Spatializer"
testNumber = "05"
bufferSize = 1024

# delays
appOpenDelay = 10

playDelay = 4 #10
screenshotDelay = 1 #2

# get the args from command line
if (len(sys.argv) > 1):
    plugName = sys.argv[1]
    testNumber = sys.argv[2]
    bufferSize = sys.argv[3]
            
#
# config
#
#appName = "/Applications/REAPER64.app"
procAppName = "/Applications/REAPER64.app//Contents/MacOS/REAPER"
        
projectsPath = "/Users/applematuer/Documents/Dev/plugin-development/TestSystem-mac"
projectDir = "Projects/Reaper/Projects/" + plugName 
projectName = plugName + "-TEST-" + testNumber + ".RPP"
tmpDir = projectsPath + "/" + "Tmp"

appSpectroName = "/Applications/iZotope RX 6 Audio Editor.app"

playDelay = playDelay - screenshotDelay

#
# open app with project
#
appArgs = projectsPath + "/" + projectDir + "/" + "Project/" + projectName
#app = App(appName)
#app.setUsing(appArgs) # not working
#app.open()

process = os.popen(procAppName + " " + appArgs)

wait(appOpenDelay)


# preferences
type("p", KeyModifier.CMD + KeyModifier.SHIFT)

wait(2)

doubleClick(Pattern("1557874407744.png").targetOffset(-21,-9))
wait(1)

# set buffer size
type(Key.BACKSPACE)
keyDown(Key.SHIFT)
type(str(bufferSize))
keyUp(Key.SHIFT)
wait(1)
type(Key.ENTER)
wait(1)

#
# start play
#
type(" ")

wait(screenshotDelay)

#
# take a screenshot
#
click(Pattern("1558042805025.png").targetOffset(-13,1))
wait(1)
plugWin = App.focusedWindow()
regionImg = capture(plugWin)
screenshotPath = projectsPath + "/" + projectDir + "/" + "Result/Screenshots/"
screenshotName = projectName + "-" + str(bufferSize) + "-snap" + ".png"
screenshotFullPath = screenshotPath + "/" + screenshotName
shutil.move(regionImg, screenshotFullPath)

# finish play
wait(playDelay)

# click somewhere in Reaper (for Chroma and GhostViewer
# (because this two plugs grab and catch keyboard shortcuts)
click("1558072120691.png")
wait(1)

#
# bounce selection
#
type("r", KeyModifier.ALT + KeyModifier.CMD)
wait(1)


click("1557875162389.png")


wait("1558110233397.png", FOREVER)

wait(1)

#click("1558110037575.png")
type(Key.ENTER)

wait(1)

#
#close Reaper
#
type("a", KeyModifier.CMD) # cmd+q

wait(1)
click(Pattern("1558044125487.png").targetOffset(13,6))

#
# generate the spectrogram image
#

# move the bounce
bounceSrcPath = projectsPath + "/" + projectDir + "/" + "Project/Bounces/bounce.wav"
shutil.move(bounceSrcPath, tmpDir)

# launch app
appSpectro = App(appSpectroName)
appSpectro.open()
wait("1558102180206.png", FOREVER)

#
# open bounce
#
type("o", KeyModifier.CMD)
wait(1)

type(Key.ENTER)
wait(1)

click(Pattern("1557929969974.png").targetOffset(-1,-14))
wait(1)

type(Key.ENTER)

wait("1558106388302.png", FOREVER)
wait(1)
        
#
# export screenshot
#
type("e", KeyModifier.SHIFT + KeyModifier.ALT + KeyModifier.CMD)
wait(1)

click(Pattern("1557931098113.png").targetOffset(-9,0))

wait(1)

click("1558103329102.png")

#type(Key.ENTER)

#wait(2)
wait(1)
waitVanish(Pattern("1558104691102.png").exact(), FOREVER)
wait(1)

# close tab
click(Pattern("1557934006031.png").targetOffset(-39,1))

wait(1)

spectrogramsSrcFullPath = tmpDir + "/" + "bounce.png" 
spectrogramsPath = projectsPath + "/" + projectDir + "/" + "Result/Spectrograms/"
spectrogramDstName = projectName + "-" + str(bufferSize) + "-spectro" + ".png"
spectrogramDstFullPath = spectrogramsPath + "/" + spectrogramDstName
shutil.move(spectrogramsSrcFullPath, spectrogramDstFullPath)

wait(1)

#
# move and rename the bounce
#
bounceDstPath = projectsPath + "/" + projectDir + "/" + "Result/Bounces"
bounceName = projectName + "-" + str(bufferSize) + "-bounce" + ".wav"
bounceFullPath = bounceDstPath + "/" + bounceName
bounceTmpPath = tmpDir + "/" + "bounce.wav"
shutil.move(bounceTmpPath, bounceFullPath)

wait(1)

#appSpectro.close() # failes to restart cleanly after
type("a", KeyModifier.CMD) # cmd+q
