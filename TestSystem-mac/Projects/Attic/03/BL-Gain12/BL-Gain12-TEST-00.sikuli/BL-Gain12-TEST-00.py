# NOTE: Reaper set the preferences options to the page where is the buffer size
# NOTE: Reaper Preferences => new shortcut cmd+shift+p
#
# NOTE: RX6 => export screenshot shortcut added shift+alt+cmd+e
# NOTE: RX6 => screenshot res set to 1024x600
# NOTE: RX6 => set tmp dir by hand for opening wave file, and saving screenshot
# NOTE: RX6 => take care to close all tabs at the beginning
import sys.argv
import shutil;

#
# parameters
#
plugName = "BL-Gain12"
testNumber = "00"
bufferSize = 1024

# delays
appOpenDelay = 6
playDelay = 10
screenshotDelay = 5
bounceDelay = 4

appSpectroOpenDelay = 6
saveSpectroDelay = 20

# get the args from command line
if (len(sys.argv) > 1):
    plugName = sys.argv[1]
    testNumber = sys.argv[2]
    bufferSize = sys.argv[3]

#
# config
#
appName = "/Applications/REAPER64.app"
projectsPath = "/Users/applematuer/Documents/Dev/plugin-development/TestSystem-mac"
projectDir = "Projects/Reaper/" + plugName 
projectName = plugName + "-TEST-" + testNumber + ".RPP"
tmpDir = projectsPath + "/" + "Tmp"

appSpectroName = "/Applications/iZotope RX 6 Audio Editor.app"

playDelay = playDelay - screenshotDelay

#
# open app with project
#
app = App(appName)
appArgs = projectsPath + "/" + projectDir + "/" + "Project/" + projectName
app.setUsing(appArgs)
app.open()

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
click("1557916365000.png")
wait(1)
plugWin = App.focusedWindow()
regionImg = capture(plugWin)
screenshotPath = projectsPath + "/" + projectDir + "/" + "Result/Screenshots/"
screenshotName = projectName + "-" + str(bufferSize) + "-snap" + ".png"
screenshotFullPath = screenshotPath + "/" + screenshotName
#shutil.move(regionImg, os.path.join(screenshotPath, screenshotName))
shutil.move(regionImg, screenshotFullPath)

# finish play
wait(playDelay)

#
# bounce selection
#
type("r", KeyModifier.ALT + KeyModifier.CMD)
wait(1)
click("1557875162389.png")
wait(bounceDelay)

click(Pattern("1557875353493.png").targetOffset(-47,2))

wait(1)

app.close()

#
# generate the spectrogram image
#

# move the bounce
bounceSrcPath = projectsPath + "/" + projectDir + "/" + "Project/Bounces/bounce.wav"
shutil.move(bounceSrcPath, tmpDir)

# launch app
appSpectro = App(appSpectroName)
appSpectro.open()
wait(appSpectroOpenDelay)

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
wait(1)

#
# export screenshot
#
type("e", KeyModifier.SHIFT + KeyModifier.ALT + KeyModifier.CMD)
wait(1)

click(Pattern("1557931098113.png").targetOffset(-9,0))

wait(1)

type(Key.ENTER)
wait(saveSpectroDelay)

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
