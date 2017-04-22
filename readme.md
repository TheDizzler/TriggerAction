An Action RPG prototype. Using Assets from ChronoTrigger...because reasons.
My first attempt at a real-time fighting game and creating pseudo 3D effect with 2D sprites.


##Updates made to DXTKGui that have not been pushed to DXTKGui project
>- updates from Fortress of Horrors
>- added comments to Sprite,and slightly improved it (setPosition no long resets hitbox size and update was removed)
>- added ability to disable pop-up message in hrReporter in StringHelper (changes also made in GraphicsAsset)
>- made changes to AssetSet: can add animations
>- changed Animations: frametime is now held by individual frame
>- added prototype functions to Screen for actions to take when a joystick is removed (controllerRemoved(size_t controllerSlot)) and added (newController(HANDLE joyHandle))
>- added functional getText() to Dialog
>- made Dialog conform to a minimum size
>- updates to GUIFactory to create Dialog with possibilities of errors
>- Dialog, Buttons, TextLabels can change layerdepth accurately
	(layering gets wonky when less than 0.00001002f difference between Dialog and Button)
>- layerDepth default in GUIControl was set to .9f for more flexibility
>- FontSet default layerDepth set to .91f when no layerDepth called
>- realized most of FontSet doesn't do anything, making the class basically redundant
>- set Sprite default layerDepth .1f;
>- changed order of createDialog params
>- changed dialog->open() to show() because that seems more common
>- deleted superfluous KeyboardController
>- dialog is responsive to frame thickness now
>- removed useless functions in TextLabel: setToUnpressedState(), setToHoverState(), setToSelectedState()
>- added controllerRemoved(size_t controllerSlot) and newController(HANDLE joyHandle) to Screen
>- moved AssetSet into DXTKGui and implemented asset sets
>- DynamicDialogs 
>- TextLabel setFonts weren't exposed
>- fixed major timing issues in main.cpp
>- RectangleFrame more efficient (one draw call instead of 4)
>- TransitionEffects changed to be more end-user friendly
>- Dialogs broken down and refactored (Major changes!)
>- Interface class for IElement2Ds that can be turned into a TexturePanel