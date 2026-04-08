#pragma once
#include "CoreMinimal.h"

class FEditorEngine;

class FPIEToolbarWindow
{
public:
	/** MainMenuBar Begin/End 사이에서 호출한다. 메뉴바 중앙에 Play/Stop 버튼을 렌더링한다. */
	void RenderInMenuBar(FEditorEngine* Engine);
};
