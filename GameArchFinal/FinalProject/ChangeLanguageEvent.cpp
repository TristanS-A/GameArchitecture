#include "ChangeLanguageEvent.h"

ChangeLanguageEvent::ChangeLanguageEvent(Button::LanguageOptions languageToChangeTo) : GameEvent(CHANGE_LANGUAGE), mLanguageToChangeTo(languageToChangeTo)
{
}

ChangeLanguageEvent::~ChangeLanguageEvent()
{
}

Button::LanguageOptions ChangeLanguageEvent::getLanguageToChangeTo() const
{
	return mLanguageToChangeTo;
}
