#include <pg.h>
#include <components/AnalogKeypad.h>
using namespace pg;
using namespace std::chrono;

// This program demonstrates the use of the Keypad class and how to setup
// a pollable Keypad Shield and respond to keypad events. The program 
// simply prints the button and event on the serial monitor. Description 
// and usage info can be found at the top of the <components/AnalogKeypad.h>
// file.

const char* button_name[] = { "down","up","left","right","select" }; // printable button names, just for demo.

enum class Keypad::Button::Tag  // Enumerates valid tags that identify Keypad::Button objects.
{
  Right = 0,
  Up,
  Down,
  Left,
  Select
};

// Button press event handler.
void keypadPressEvent(const Keypad::Button* button)
{
  const char* event_type = " button pressed";
  const char* btn = nullptr;

  switch (button->tag_)
  {
  case Keypad::Button::Tag::Down:
    btn = button_name[0];
    break;
  case Keypad::Button::Tag::Up:
    btn = button_name[1];
    break;
  case Keypad::Button::Tag::Left:
    btn = button_name[2];
    break;
  case Keypad::Button::Tag::Right:
    btn = button_name[3];
    break;
  case Keypad::Button::Tag::Select:
    btn = button_name[4];
    break;
  default:
    break;
  }
  if (btn)
  {
    Serial.print(btn);
    Serial.println(event_type);
  }
}

// Button release event handler.
void keypadReleaseEvent(const Keypad::Button* button)
{
  const char* event_type = " button released";
  const char* btn = nullptr;

  switch (button->tag_)
  {
  case Keypad::Button::Tag::Down:
    btn = button_name[0];
    break;
  case Keypad::Button::Tag::Up:
    btn = button_name[1];
    break;
  case Keypad::Button::Tag::Left:
    btn = button_name[2];
    break;
  case Keypad::Button::Tag::Right:
    btn = button_name[3];
    break;
  case Keypad::Button::Tag::Select:
    btn = button_name[4];
    break;
  default:
    break;
  }
  if (btn)
  {
    Serial.print(btn);
    Serial.println(event_type);
  }
}

// Button long-press event handler.
void keypadLongpressEvent(const Keypad::Button* button)
{
  const char* event_type = " button long pressed";
  const char* btn = nullptr;

  switch (button->tag_)
  {
  case Keypad::Button::Tag::Down:
    btn = button_name[0];
    break;
  case Keypad::Button::Tag::Up:
    btn = button_name[1];
    break;
  case Keypad::Button::Tag::Left:
    btn = button_name[2];
    break;
  case Keypad::Button::Tag::Right:
    btn = button_name[3];
    break;
  case Keypad::Button::Tag::Select:
    btn = button_name[4];
    break;
  default:
    break;
  }
  if (btn)
  {
    Serial.print(btn);
    Serial.println(event_type);
  }
}

// Keypad callback function.
void keypad_cb(const Keypad::Button* button, Keypad::Event event)
{
  static bool lp = false;

  switch (event)
  {
  case Keypad::Event::Press:
    keypadPressEvent(button);
    lp = false;
    break;
  case Keypad::Event::Longpress:
    if (!lp)
    {
      keypadLongpressEvent(button);
      lp = true; // prevents repetitious printing.
    }
    break;
  case Keypad::Event::Release:
    keypadReleaseEvent(button);
    lp = false;
    break;
  default:
    break;
  }
}

const pin_t KeypadInputPin = 0; // Keypad analog input pin (check Keypad Shield mfg specs.)
const milliseconds KeypadLongPressInterval = seconds(1); // Keypad long press interval.

// Button trigger level (check Keypad Shield mfg specs.)
const analog_t RightButtonTriggerLevel = 60;
const analog_t UpButtonTriggerLevel = 200;
const analog_t DownButtonTriggerLevel = 400;
const analog_t LeftButtonTriggerLevel = 600;
const analog_t SelectButtonTriggerLevel = 800;

// Keypad button objects.
Keypad::Button right_button(Keypad::Button::Tag::Right, RightButtonTriggerLevel);
Keypad::Button up_button(Keypad::Button::Tag::Up, UpButtonTriggerLevel);
Keypad::Button down_button(Keypad::Button::Tag::Down, DownButtonTriggerLevel);
Keypad::Button left_button(Keypad::Button::Tag::Left, LeftButtonTriggerLevel);
Keypad::Button select_button(Keypad::Button::Tag::Select, SelectButtonTriggerLevel);
Keypad::Button buttons[] = { right_button, up_button, down_button, left_button, select_button };

// Keypad object.
Keypad keypad({ &right_button,&up_button,&down_button,&left_button,&select_button }, KeypadInputPin, &keypad_cb, Keypad::LongPress::Hold, KeypadLongPressInterval);

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  while (1)
    keypad.poll();
}
