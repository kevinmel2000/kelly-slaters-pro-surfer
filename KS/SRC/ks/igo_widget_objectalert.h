#ifndef INCLUDED_IGO_WIDGET_OBJECTALERT_H
#define INCLUDED_IGO_WIDGET_OBJECTALERT_H

#include "global.h"
#include "igo_widget.h"
#include "fepanel.h"


class ObjectAlertWidget : public IGOWidget
{
private:

	static const float TIME_FADE;
	static const float TIME_ANIMATE;
	static const float SPEED_HILITE_FLASH;

private:
	PanelQuad *	objectRoot;

	float		hiliteTime;		// amount of time highlight has been displayed
	float		fade;			// alpha of overlay
	int			fadeDir;		// 1 for fading in, -1 for fading out, 0 for no change

	bool FindNearestObject(void);

public:
	// Creators.
	ObjectAlertWidget();
	~ObjectAlertWidget();

	// Modifiers.
	void SetDisplay(const bool d = true);
	void Init(PanelFile & panel);
	void Update(const float dt);
	void Draw(void);
	void Hide(const bool fadeOut = true);
	void Show(const bool fadeIn = true);

	// Accessors.
	bool IsHiding(void) const { return (fadeDir == -1); }
};

#endif	INCLUDED_IGO_WIDGET_OBJECTALERT_H