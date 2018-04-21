#import <Cocoa/Cocoa.h>

void put_window_over_menu_bar(NSWindow *window)
{
    window.level = NSPopUpMenuWindowLevel;
}
