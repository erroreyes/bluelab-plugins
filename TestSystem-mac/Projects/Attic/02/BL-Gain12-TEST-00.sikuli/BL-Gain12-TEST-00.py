#openApp("/Applications/REAPER64.app/Contents/MacOS/REAPER")
#app = App("/Applications/REAPER64.app/Contents/MacOS/REAPER")
#app = App("/Applications/REAPER64.app)
#app.open()
#app.focus()
#app.close()

#openApp("/Applications/REAPER64.app")

#App.open("/Applications/REAPER64.app/Contents/MacOS/REAPER")

# screenshot using mac os
#type("4", KeyModifier.CMD + KeyModifier.SHIFT)
#type(" ")
#click("1557876150424.png")
#wait(1)



# NOTE: keep the plugin in windows in the center of the screen
# NOTE: set initial buffer size to 447
# NOTE: delete screenshots on the desktop before launching tests
# NOTE: set the preferences options to the page where is the buffer size
# NOTE: Reaper Preferences => cmd+shift+p
# NOTE: RX6 => export screenshot shortcut added shift+alt+cmd+e
# NOTE: RX6 => screenshot res set to 1024x600
# NOTE: RX6 => set tmp dir by hand for opening wave file, and saving screenshot
import sys.argv
import shutil;

#
# parameters
#
bufferSize = 1024

plugName = "BL-Gain12"
testNumber = "00"

appOpenTime = 6
playDelay = 10
screenshotDelay = 5
bounceTime = 4

appSpectroOpenTime = 6

saveSpectroTime = 20

#bufferSize = sys.argv[1]

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

wait(appOpenTime)
#app.focus()

#
# set buffer size
#
#type(",", KeyModifier.CMD) # not working
type("p", KeyModifier.CMD + KeyModifier.SHIFT)

wait(2)

doubleClick(Pattern("1557874407744.png").targetOffset(-21,-9))
wait(1)
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
screenshotName = "screenshot-" + projectName + "-" + str(bufferSize) + ".png"
screenshotFullPath = screenshotPath + "/" + screenshotName
#shutil.move(regionImg, os.path.join(screenshotPath, screenshotName))
shutil.move(regionImg, screenshotFullPath)

wait(playDelay)

#
# bounce selection
#
type("r", KeyModifier.ALT + KeyModifier.CMD)
wait(1)
click("1557875162389.png")
wait(bounceTime)
#type("Key.ENTER")
click(Pattern("1557875353493.png").targetOffset(-47,2))

wait(1)

app.close()

#
# generate the spectrogram image
#
bounceSrcPath = projectsPath + "/" + projectDir + "/" + "Project/Bounces/bounce.wav"
shutil.move(bounceSrcPath, tmpDir)

appSpectro = App(appSpectroName)
appSpectro.open()
wait(appSpectroOpenTime)

type("o", KeyModifier.CMD)
wait(1)

#
# open bounce
#

# doesn't work, keyboard issues
# set tmp dir in finder
#type("g", KeyModifier.SHIFT + KeyModifier.CMD)
#wait(1)
#type(Key.BACKSPACE)
#wait(1)
#keyDown(Key.SHIFT)
#wait(1)
#type(tmpDir)
#paste(tmpDir) #can't paste here in finder
#wait(1)
#keyUp(Key.SHIFT)

#wait(1)

type(Key.ENTER)
wait(1)

click(Pattern("1557929969974.png").targetOffset(-1,-14))
wait(1)

type(Key.ENTER)
wait(1)

#spectroWin = App.focusedWindow()
#regionImgSpectro = capture(spectroWin)
#spectrogramsPath = projectsPath + "/" + projectDir + "/" + "Result/Spectrograms/"
#spectrogramName = "spectrogram-" + projectName + "-" + str(bufferSize) + ".png"
#spectrogramFullPath = spectrogramPath + "/" + spectrogramName
#shutil.move(regionImgSpectro, spectrogramFullPath)

#
# export screenshot
#
type("e", KeyModifier.SHIFT + KeyModifier.ALT + KeyModifier.CMD)
wait(1)

click(Pattern("1557931098113.png").targetOffset(-9,0))

wait(1)

# doesn't work, keyboard issues
# set tmp dir in finder
#type("g", KeyModifier.SHIFT + KeyModifier.CMD)
#wait(1)
#type(Key.BACKSPACE)
#wait(1)
#keyDown(Key.SHIFT)
#wait(1)
#type(tmpDir)
#paste(tmpDir)
#wait(1)
#keyUp(Key.SHIFT)

#wait(1)

type(Key.ENTER)
wait(saveSpectroTime)

# close tab
click(Pattern("1557934006031.png").targetOffset(-39,1))

wait(1)

spectrogramsSrcFullPath = tmpDir + "/" + "bounce.png" 
spectrogramsPath = projectsPath + "/" + projectDir + "/" + "Result/Spectrograms/"
spectrogramDstName = "spectrogram-" + projectName + "-" + str(bufferSize) + ".png"
spectrogramDstFullPath = spectrogramsPath + "/" + spectrogramDstName
shutil.move(spectrogramsSrcFullPath, spectrogramDstFullPath)

wait(1)

#
# move and rename the bounce
#
bounceDstPath = projectsPath + "/" + projectDir + "/" + "Result/Bounces"
bounceName = "bounce-" + projectName + "-" + str(bufferSize) + ".wav"
bounceFullPath = bounceDstPath + "/" + bounceName
bounceTmpPath = tmpDir + "/" + "bounce.wav"
shutil.move(bounceTmpPath, bounceFullPath)

wait(1)

#appSpectro.close() # failes to restart cleanly after
type("a", KeyModifier.CMD) # cmd+q
