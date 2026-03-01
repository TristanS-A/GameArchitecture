#pragma once
#include "GameEvent.h"
#include "Button.h"

class ChangeLanguageEvent : public GameEvent
{
public:
	ChangeLanguageEvent(Button::LanguageOptions languageToChangeTo);
	~ChangeLanguageEvent();

	Button::LanguageOptions getLanguageToChangeTo() const;

private:
	Button::LanguageOptions mLanguageToChangeTo;
};

