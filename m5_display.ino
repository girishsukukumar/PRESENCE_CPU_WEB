#
#include <M5Unified.h>

M5Canvas popup(&M5.Display);
bool isPopupActive = false;

// Global coordinates storing the pop-up box placement
int popupX, popupY;
const int popupW = 240;
const int popupH = 130;

// Global coordinates for the visual "OK" button boundary (relative to the screen)
int okAbsoluteX, okAbsoluteY;
const int okW = 80;
const int okH = 30;

void drawMainUI() 
{
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 20);
    M5.Display.print("Gentle Steps Elderly Care");
    
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 60);
    //M5.Display.print("Tap anywhere on screen to trigger alert.");
}

void showPopup(const char *msg) 
{
    if (isPopupActive)
    {
      return;
    }

    // Center the pop-up window coordinates on the screen
    popupX = (M5.Display.width() - popupW) / 2;
    popupY = (M5.Display.height() - popupH) / 2;

    // Calculate where the OK button lies globally on the hardware glass panel
    // It sits horizontally centered inside the pop-up, near the bottom
    okAbsoluteX = popupX + ((popupW - okW) / 2);
    okAbsoluteY = popupY + popupH - okH - 15;

    // 1. Allocate Sprite Memory
    popup.createSprite(popupW, popupH);
    popup.fillScreen(TFT_DARKGRAY);
    popup.drawRect(0, 0, popupW, popupH, TFT_WHITE);

    // 2. Render Header
    popup.fillRect(2, 2, popupW - 4, 25, TFT_BLUE);
    popup.setTextColor(TFT_WHITE);
    popup.setTextSize(2);
    popup.drawString("Alert", 10, 6);

    // 3. Render Message Body Text
    popup.setTextColor(TFT_LIGHTGRAY);
    popup.setTextSize(1);

    popup.drawString(msg, 20, 45);
    //popup.drawString("System is stable.", 20, 65);

    // 4. Render the graphic "OK" Button inside the Sprite Canvas
    // Button coordinates are local to the sprite array canvas map (0 to popupW)
    int localBtnX = (popupW - okW) / 2;
    int localBtnY = popupH - okH - 15;
    popup.fillRect(localBtnX, localBtnY, okW, okH, TFT_GREEN);
    popup.drawRect(localBtnX, localBtnY, okW, okH, TFT_WHITE);
    popup.setTextColor(TFT_BLACK);
    popup.setTextSize(2);
    popup.drawString("OK", localBtnX + 28, localBtnY + 6);

    // 5. Push buffer elements to screen array matrix
    popup.pushSprite(popupX, popupY);
    isPopupActive = true;
}

void DisplayPopUp(const char *msg)
{
    hidePopup();
    showPopup(msg);
    return ;
}

void hidePopup() 
{
    if (!isPopupActive) 
    {
      return;
    }
    
    popup.deleteSprite(); // Deallocate RAM
    drawMainUI();         // Patch the background display UI layout
    isPopupActive = false;
}

void UI_Loop()
{
     //M5.update(); // Keeps track of recent screen touches

    // Check touch screen panel configurations
    if (M5.Touch.getCount() > 0) 
    {
        auto detail = M5.Touch.getDetail(0);

        // Check if user just pressed down their finger
        if (detail.wasPressed()) 
        {
            int touchX = detail.x;
            int touchY = detail.y;

            if (isPopupActive) 
            {
                // Determine if the touch event overlaps the boundary box of the OK button
                if (touchX >= okAbsoluteX && touchX <= (okAbsoluteX + okW) &&
                    touchY >= okAbsoluteY && touchY <= (okAbsoluteY + okH)) 
                {
                    hidePopup();
                }
            } 

        }
    }

    delay(10);
}
